//#include <arma/armadillo>
#include "KnnMatrix.h"
#include <misc/threadUtils.h>
#include <vlutil/LightEval.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <imageio/imageio.h>
#include <vmath/streamMethods.h>
#include "vlutil/ListVirtualLightCache.h"
#include "KnnMatrixImpl.h"
#include "GatherGroupTbbImpl.h"
#include "vmath/range1.h"
#include "scene/material.h"
#include "misc/arrays.h"
#include <queue>

KnnMatrix::KnnMatrix(Scene *scene, RayEngine *engine, VirtualLightGenerator *gen, ReportHandler *report) 
    : _scene(scene), _engine(engine), _generator(gen), _report(report) 
{
    float radius = (_engine->ComputeBoundingBox().Diagonal() / 2.0f) * 0.05f;
    _clamp = radius * radius;
    _normScale = _engine->ComputeBoundingBox().Diagonal() / 8.0f;
}

void KnnMatrix::_SetBackground(Image<Vec3f> *image)
{
	for (uint32_t i = 0; i < _bkPixels.size(); i++)
	{
        BackgroundPixel &bkPixel = _bkPixels[i];
        Vec2i &pixel = bkPixel.pixel;
        image->ElementAt(pixel.x, image->Height() - pixel.y - 1) += bkPixel.background * bkPixel.strength;
	}
}


struct ComputeColNorm
{
	ComputeColNorm(carray2<Vec3f> &matrix, carray2<Vec3f> &norms, const vector<GatherGroup>	&groups) 
		: _matrix(matrix), _norms(norms), _groups(groups) { }
	void operator() (const blocked_range<uint32_t> &r) const {
		for (uint32_t g = r.begin(); g != r.end(); g++)
		{
			const GatherGroup &group = _groups[g];
			for (uint32_t i = 0; i < _matrix.width(); i++)
			{
				Vec3f n = Vec3f::Zero();
				for (int j= 0; j < group.neighbors.size(); j++)
				{
					uint32_t gidx = group.neighbors[j];
					const Vec3f &v = _matrix.at(i, gidx);
					n += v * v;
				}
				_norms.at(i, g) = n.Sqrt();
			}
		}
	}
	carray2<Vec3f>                      &_norms;
	carray2<Vec3f>				        &_matrix;
	const vector<GatherGroup>			&_groups;
};

struct FinalRenderThread
{
	typedef uint32_t argument_type;
	FinalRenderThread(KnnMatrix *knnMat, const vector<uint32_t> &indices, const vector<ScaleLight> &scaleLights, Image<Vec3f> *image, Image<uint32_t> *sampleImage) 
		: _knnMat(knnMat), _indices(indices), _scaleLights(scaleLights), _image(image), _sampleImage(sampleImage) {};
	void operator()(uint32_t g) const { _RenderGatherPoint(_indices[g]); }
    void _RenderGatherPoint( uint32_t g ) const {
        GatherPoint &gp = _knnMat->_gatherPoints[g];
        Vec3f L;
        for (uint32_t c = 0; c < _scaleLights.size(); c++)
        {
            const ScaleLight &lc = _scaleLights[c];
            uint32_t idx = lc.indices[gp.index];
            const Vec3f& weight = lc.weights[gp.index];
            if(!weight.IsZero())
                L += _knnMat->RenderCell(LightEvalUtil::EvalLight(_knnMat->_clamp), idx, gp) * weight;
        }
        {
            _image->ElementAt(gp.pixel.x, _image->Height() - gp.pixel.y - 1) += gp.emission + L * gp.strength * gp.weight;
            if(_sampleImage) _sampleImage->ElementAt(gp.pixel.x, _sampleImage->Height() - gp.pixel.y - 1) += (uint32_t)_scaleLights.size();
        }
    }
private:
	KnnMatrix                   *_knnMat;
	const vector<ScaleLight>    &_scaleLights;
	const vector<uint32_t>      &_indices;
	Image<Vec3f>				*_image;
	Image<uint32_t>				*_sampleImage;
};

struct FinalGroupRenderThread
{
	typedef uint32_t argument_type;
	FinalGroupRenderThread(KnnMatrix *knnMat, Image<Vec3f> *image, Image<uint32_t> *sampleImage)
		: _knnMat(knnMat), _groups(knnMat->_gpGroups), _lights(knnMat->_scaledLights), _image(image), _sampleImage(sampleImage) {
	}
	void operator()(uint32_t gg) const { 
        //TbbCounter counter((uint32_t)_groups[g].indices.size());
        //FinalRenderThread thread(_knnMat, _groups[g].indices, _lights[g], _image, _sampleImage);
        //parallel_while<FinalRenderThread> w;
        //w.run(counter, thread);
        GatherGroup &group = _groups[gg];
        vector<ScaleLight> &scaleLight = _lights[gg];
        for (int i = 0; i < group.indices.size(); i++)
        {
            int g = group.indices[i];
            GatherPoint &gp = _knnMat->_gatherPoints[g];
            Vec3f L;
            for (uint32_t c = 0; c < scaleLight.size(); c++)
            {
                const ScaleLight &lc = scaleLight[c];
                uint32_t idx = lc.indices[gp.index];
                const Vec3f& weight = lc.weights[gp.index];
                if(!weight.IsZero())
                    L += _knnMat->RenderCell(LightEvalUtil::EvalLight(_knnMat->_clamp), idx, gp) * weight;
            }
            {
                _image->ElementAt(gp.pixel.x, _image->Height() - gp.pixel.y - 1) += gp.emission + L * gp.strength * gp.weight;
                if(_sampleImage) _sampleImage->ElementAt(gp.pixel.x, _sampleImage->Height() - gp.pixel.y - 1) += (uint32_t)scaleLight.size();
            }
        }
	}
	KnnMatrix					*_knnMat;
	vector<GatherGroup>			&_groups;
	vector<vector<ScaleLight> > &_lights;
	Image<Vec3f>				*_image;
	Image<uint32_t>				*_sampleImage;
};


void KnnMatrix::Render(Image<Vec3f> *image, Image<uint32_t> *sampleImage, uint32_t samples, uint32_t indirect, uint32_t seedNum, uint32_t budget)
{
 	_GenerateLights(indirect);
	_ShootGatherPoints(image->Width(), image->Height(), samples);
	_GroupGatherPoints(seedNum);
	_FindGatherGroupNeighbors();

	carray2<Vec3f> matrix(_lightList.GetSize(), (uint32_t)(_gpGroups.size()));
	_RenderReducedMatrix(matrix);

	//vector<Range1i>			clusters;
	//carray<uint32_t>		lights(matrix.width());

    if(sampleImage) sampleImage->Set(0);
    _SetBackground(image);

    vector<vector<uint32_t> > clusters;
	//_NewInitialClusters(clusters, matrix, budget * 0.3f);
	_InitialClusters(clusters, matrix, budget * 0.3f, false);
	_RefineClusters(clusters, matrix, budget, samples, image, sampleImage);

    if (_report) _report->beginActivity("rendering final image");

    //TbbReportCounter counter((uint32_t)_gpGroups.size(), _report);
    //FinalGroupRenderThread thread(this, image, sampleImage);
    //parallel_while<FinalGroupRenderThread> w;
    //w.run(counter, thread);

	if (_report) _report->endActivity();
}


struct RefineClusterThread {
	typedef uint32_t argument_type;
	RefineClusterThread(KnnMatrix *knnMat, const vector<vector<uint32_t> > &clusters, vector<vector<ScaleLight> > &scaleLights, 
		carray2<Vec3f> &matrix, carray<Vec3f> &fullNorms, carray2<Vec3f> &colNorms, vector<GatherGroup> &gpGroups, const carray<uint64_t> &randSeeds, uint32_t budget, uint32_t samples, Image<Vec3f> *image, Image<uint32_t> *sampleImage) 
		: _knnMat(knnMat), _clusters(clusters), _scaleLights(scaleLights), _matrix(matrix), _norms(colNorms), _fullNorms(fullNorms), _gpGroups(gpGroups), _budget(budget), _samples(samples), _seeds(randSeeds), _image(image), _sampleImage(sampleImage) { } 
	void operator() (uint32_t g) const {

		const GatherGroup &gpGroup = _gpGroups[g];
		
		const Vec3f *norms = _norms.row(g);
        const vector<uint32_t> &samples = gpGroup.neighbors;
		carray<pair<uint32_t, float> > projection(_matrix.width());
        carray<Vec3f> line((uint32_t)samples.size());

		std::vector<pair<Range1i, float> > cluster_queue;
		cluster_queue.reserve(_budget);

        uint32_t off = 0;
        for (uint32_t c = 0; c < _clusters.size(); c++) {
            const vector<uint32_t> &cluster = _clusters[c];
            if (cluster.size() == 0)
                continue;

            Range1i range;
            range.SetMin(off);
			for (int32_t i = 0; i < cluster.size(); i++)
				projection.at(off++).first = cluster[i];
            range.SetMax(off);

			float cst = cost(projection, range, samples, norms);
			cluster_queue.push_back(make_pair(range, cst));

			push_heap(cluster_queue.begin(), cluster_queue.end(), [](pair<Range1i, float> &d1, pair<Range1i, float> &d2)->bool { return d1.second < d2.second;});
		}

		while(cluster_queue.size() < _budget)
		{
			float error = cluster_queue.front().second;
			if (error <= 0.0f)
				break;

			Range1i range = cluster_queue.front().first;
			assert(range.GetSize() >= 2);

			pop_heap(cluster_queue.begin(), cluster_queue.end(), [](pair<Range1i, float> &d1, pair<Range1i, float> &d2)->bool { return d1.second < d2.second;});
			cluster_queue.pop_back();
			
            //Distribution1Df dist(&norms[0], norms.size());
            //float pdf;
            //uint64_t largestIdx = dist.SampleDiscrete(Random01(), &pdf);
            //uint64_t secondIdx = dist.SampleDiscrete(Random01(), &pdf);
            //while(largestIdx == secondIdx)
            //    secondIdx = dist.SampleDiscrete(Random01(), &pdf);

			float largest = -FLT_MAX, second = -FLT_MAX;
			uint32_t largestIdx = -1, secondIdx = -1;
			for (int32_t i = range.GetMin(); i < range.GetMax(); i++) {
                uint32_t col = projection[i].first;
				float v = norms[col].Average();
				if (v > largest) {
					second = largest;
					secondIdx = largestIdx;
					largest = v;
					largestIdx = col;
				} 
				else if (v > second && v < largest) {
					second = v;
					secondIdx = col;
				}
			}
			assert(largestIdx != -1 && secondIdx != -1);

			
			for (uint32_t i = 0; i < samples.size(); i++) {
				uint32_t j = samples.at(i);
				line.at(i) = _matrix.at(largestIdx, j) - _matrix.at(secondIdx, j);
			}

			Range1f r = Range1f::Empty();
			uint32_t offset = 0;
			for (int32_t i = range.GetMin(); i < range.GetMax(); i++)
			{
				Vec3f dot; 
				uint32_t col = projection[i].first;
				for (uint32_t j = 0; j < samples.size(); j++)
					dot += _matrix.at(col, samples.at(j)) * line[j];
				float d = dot.Average();
				r.Grow(d);
				projection[i].second = d;
			}

			float pmid = r.GetCenter();
			pair<uint32_t, float> *start = projection.data() + range.GetMin();
			pair<uint32_t, float> *end = projection.data() + range.GetMax();
			pair<uint32_t, float> *middle = 0;
			middle = std::partition(start, end, [pmid](const pair<uint32_t, float> &a) { return a.second <= pmid; });
			if (middle == start || middle == end )
				middle = start + (end - start) / 2;

            //pair<uint32_t, float> *start = projection.data() + range.GetMin();
            //pair<uint32_t, float> *end = projection.data() + range.GetMax();
            //sort(start, end, [](const pair<uint32_t, float> &a, const pair<uint32_t, float> &b) { return a.second <= b.second; });
            //pair<uint32_t, float> *middle = 0;
            //pair<uint32_t, float> *it = start + 1;
            //float maxInteval = -FLT_MAX;
            //while(it != end)
            //{
            //    float inteval = it->second - (it-1)->second;
            //    if (inteval > maxInteval)
            //    {
            //        maxInteval = inteval;
            //        middle = it;
            //    }
            //    it++;
            //}
            //printf("%d, %d\n", middle - start, end - middle);
			assert(middle != start && middle != end);

			int rangMid = (int)(middle - start) + range.GetMin();
			Range1i range1(range.GetMin(), rangMid);
			Range1i range2(rangMid, range.GetMax());
			float cost1 = cost(projection, range1, samples, norms);
			float cost2 = cost(projection, range2, samples, norms);

			cluster_queue.push_back(make_pair(range1, cost1));
			push_heap(cluster_queue.begin(), cluster_queue.end(), [](pair<Range1i, float> &d1, pair<Range1i, float> &d2)->bool {
				return d1.second < d2.second;
			});
			cluster_queue.push_back(make_pair(range2, cost2));
			push_heap(cluster_queue.begin(), cluster_queue.end(), [](pair<Range1i, float> &d1, pair<Range1i, float> &d2)->bool {
				return d1.second < d2.second;
			});
		}

		/*uint64_t seed = _seeds.at(g);
		RandomPathSamplerStd::Engine e(seed);*/
		RandomPathSamplerStd sampler;
		//vector<ScaleLight> &scaleLight = _scaleLights[g];
        vector<ScaleLight> scaleLight;
		for(uint32_t k = 0; k < cluster_queue.size(); k++)
		{
			Range1i &range = cluster_queue[k].first;
			if (!range.IsValid())
				continue;

			//vector<pair<float, uint32_t> > norm_list;
			vector<Vec3f> cnorms;
			Vec3f cnormSum;
			float maxL = -1;
			uint32_t index = -1;
			Vec3f maxNorm;
			for (int32_t i = range.GetMin(); i < range.GetMax(); i++)
			{
				const Vec3f &v = _fullNorms[projection[i].first];
				cnorms.push_back(v);
				cnormSum += v;
				//norm_list.push_back(make_pair(v.Average(), projection[i].first));
    //            if (maxL < v.Average())
    //            {
    //                maxL = v.Average();
    //                index = projection[i].first;
    //                maxNorm = v;
    //            }
			}

			//sort(norm_list.begin(), norm_list.end(), [](pair<float, uint32_t>& d1, pair<float, uint32_t> &d2)->bool{return d1.first > d2.first;});
			//ScaleLight light;
			//for (uint32_t s = 0; s < _samples; s++)
			//{
			//	uint32_t idx = norm_list[s % norm_list.size()].second;
			//	light.indices.push_back(idx);
			//	Vec3f weight = cnormSum | _fullNorms[idx];
			//	light.weights.push_back(weight);
			//}
			//scaleLight.push_back(light);

			if (!cnormSum.IsZero())
			{
				Distribution1D<float, Vec3f> dist(&cnorms[0], (uint32_t)cnorms.size());
				float pdf;
				ScaleLight light;
				for (uint32_t s = 0; s < _samples; s++)
				{
					uint32_t idx = dist.SampleDiscrete(sampler.Next1D(), &pdf);
					light.indices.push_back(projection[range.GetMin() + idx].first);
					Vec3f weight = cnormSum | cnorms[idx];
					light.weights.push_back(weight);
				}
				scaleLight.push_back(light);
			}
			//else
			//{
			//	ScaleLight light;
			//	for (uint32_t s = 0; s < _samples; s++)
			//	{
			//		light.indices.push_back(index);
			//		Vec3f weight = cnormSum | maxNorm;
			//		light.weights.push_back(weight);
			//	}
			//	scaleLight.push_back(light);
			//}
		}
        TbbCounter counter((uint32_t)gpGroup.indices.size());
        FinalRenderThread thread(_knnMat, gpGroup.indices, scaleLight, _image, _sampleImage);
        parallel_while<FinalRenderThread> w;
        w.run(counter, thread);
	}

	float cost(const carray<pair<uint32_t, float> > &projection, const Range1i &range, const vector<uint32_t> &nsamples, const Vec3f *norms) const {
		if (range.GetSize() <= 1)
		    return 0.0f;

		Vec3f nsum;
		Vec3f vsum;
		for (int32_t i = range.GetMin(); i < range.GetMax(); i++)
		{
			uint32_t col = projection[i].first;
			const Vec3f &n = norms[col];
			nsum += n;
		}
		for (uint32_t j = 0; j < nsamples.size(); j++)
		{
			Vec3f sum;
			for (int32_t i = range.GetMin(); i < range.GetMax(); i++)
			{
				const Vec3f &v = _matrix.at(projection[i].first, nsamples[j]); 
				sum += v;
			}
			vsum += sum * sum;
		}
		float cost = ((nsum * nsum) - vsum).Average();
		return max(0.0f, cost);
	}
    KnnMatrix                       *_knnMat;
	uint32_t						_budget;
	uint32_t						_samples;
	vector<vector<ScaleLight> >		&_scaleLights;
	const carray<uint64_t>			&_seeds;
	const carray2<Vec3f>			&_matrix;
	const carray2<Vec3f>			&_norms;
	const carray<Vec3f>				&_fullNorms;
	const vector<GatherGroup>		&_gpGroups;
	const vector<vector<uint32_t> > &_clusters;
    Image<Vec3f> *_image;
    Image<uint32_t> *_sampleImage;
};

void KnnMatrix::_RefineClusters(vector<vector<uint32_t> > &clusters, carray2<Vec3f> &matrix, uint32_t budget, uint32_t samples, Image<Vec3f> *image, Image<uint32_t> *sampleImage )
{
	_report->beginActivity("pre-compute norms");
	carray<Vec3f> fullNorms(matrix.width());
	ComputeFullColNorm computeFullNormThread(matrix, fullNorms);
	parallel_for(blocked_range<uint32_t>(0, fullNorms.size()), computeFullNormThread);

	carray2<Vec3f> colNorms(matrix.width(), _gpGroups.size());
	ComputeColNorm computeColNormThread(matrix, colNorms, _gpGroups);
	parallel_for(blocked_range<uint32_t>(0, (uint32_t)_gpGroups.size()), computeColNormThread);
	_report->endActivity();

	_report->beginActivity("refine clusters");
	_scaledLights.resize(_gpGroups.size());
	
	carray<uint64_t> randSeeds((uint32_t)_gpGroups.size());
	//for (uint32_t i = 0; i < randSeeds.size(); i++)
	//{
	//	static minstd_rand0 seeder;
	//	uint64_t seed = seeder();
	//	//randSeeds.at(i) = seed; 
	//	randSeeds.at(i) = 0; 
	//}

	RefineClusterThread thread(this, clusters, _scaledLights, matrix, fullNorms, colNorms, _gpGroups, randSeeds, budget, samples, image, sampleImage);
	TbbReportCounter counter((uint32_t)_gpGroups.size() ,_report);
	parallel_while<RefineClusterThread> w;
	w.run(counter, thread);
	//for (uint32_t g = 0; g < _gpGroups.size(); g++)
	//{
	//	thread(g);
	//}
	_report->endActivity();
}


void KnnMatrix::_ShootGatherPoints(uint32_t width, uint32_t height, uint32_t samples)
{
    if (_report) _report->beginActivity("sampling eye points");
    GatherPointShooter::Shoot(_scene, _engine, width, height, samples, _gatherPoints, _bkPixels, _report);
    if (_report) _report->endActivity();
}

void KnnMatrix::_GenerateLights( uint32_t indirect )
{
    if (_report) _report->beginActivity("generate virtual lights");
    ListVirtualLightCache cache(_lightList);
    _generator->Generate(indirect, &cache);
    if (_report) _report->endActivity();

    cout << "direct VPL:\t" << cache.DirectLightNum() << endl;
    cout << "indirect VPL:\t" << cache.IndirectLightNum() << endl;
    cout << "total VPL:\t " << cache.LightNum() << endl;
}

void KnnMatrix::RenderGatherGroup( Image<Vec3f> *gpImage )
{
	RandomPathSamplerStd sampler;
	if (_report) _report->beginActivity("rendering gather point cluster");
	for (uint32_t i = 0; i < _gpGroups.size(); i++)
	{
		const GatherGroup &gpGroup = _gpGroups[i];
		Vec3f color(sampler.Next1D(),sampler.Next1D(),sampler.Next1D());

		for (uint32_t j = 0; j < gpGroup.indices.size(); j++)
		{
			GatherPoint& gp = _gatherPoints[gpGroup.indices[j]];
			gpImage->ElementAt(gp.pixel.x, gpImage->Height()- gp.pixel.y - 1) = color;
		}
		if (_report) _report->progress(i / (float)_gpGroups.size(), 1);
	}
	if (_report) _report->endActivity();
}

void KnnMatrix::_KdGatherGroup( vector<GatherKdItem>::iterator start, vector<GatherKdItem>::iterator end)
{
    Range6f bbox = Range6f::Empty();
    for (vector<GatherKdItem>::iterator it = start; it != end; it++)
        bbox.Grow(it->p);
    if(end - start < _maxGatherGroupSize)
    {
        _gpGroups.push_back(GatherGroup());
        GatherGroup &group = _gpGroups.back();
        for (vector<GatherKdItem>::iterator it = start; it != end; it++)
        {
            group.indices.push_back(it->idx);
            GatherPoint &gp = _gatherPoints[it->idx];
            group.bbox.Grow(gp.isect.dp.P);
            group.normal += gp.isect.dp.N;
        }
        group.normal.Normalize();
        assert(!group.normal.IsZero());
		uint32_t sidx = min((uint32_t)(_sampler.Next1D() * group.indices.size()), (uint32_t)(group.indices.size() - 1));
        group.seed = group.indices[sidx];
    }
    else
    {
        uint32_t dim = bbox.GetSize().MaxComponentIndex();
        //vector<GatherKdItem>::iterator mid = start + (end - start) / 2;
        //std::nth_element(start, mid, end, [dim](const GatherKdItem &d1, const GatherKdItem &d2) { return d1.p[dim] < d2.p[dim]; });

        float pmid = bbox.GetCenter()[dim];
        vector<GatherKdItem>::iterator mid = 
            std::partition(start, end, [dim, pmid](const GatherKdItem &a) { return a.p[dim] < pmid; });

        _KdGatherGroup(start, mid);
        _KdGatherGroup(mid, end);
    }
}

void KnnMatrix::_GroupGatherPoints( uint32_t seedNum )
{
    _maxGatherGroupSize = max<uint32_t>(1, (uint32_t)(_gatherPoints.size() / seedNum));
    // Compute Gather Point Bounding Box
    Range3f	gpBBox = Range3f::Empty();
    for (uint32_t i = 0; i < _gatherPoints.size(); i++)
    {
        GatherPoint &gp = _gatherPoints[i];
        Vec3f &P = gp.isect.dp.P;
        gpBBox.Grow(P);
    }
    _normScale = gpBBox.GetSize().Average() / 8.0f;
	_diagonal = gpBBox.Diagonal() / 32.0f;

	vector<GatherKdItem> items(_gatherPoints.size());
	for (uint32_t i = 0; i < _gatherPoints.size(); i++)
	{
		GatherPoint &gp = _gatherPoints[i];
		GatherKdItem &item = items[i];
		item.idx = i;
		item.p = Vec6f(gp.isect.dp.P, gp.isect.dp.N * _normScale);
    }

    //_KmeanGatherGroup(items, seedNum);
    _KdGatherGroup(items.begin(), items.end());

    stringstream sout;
    sout << "Gather group numbers: " << _gpGroups.size();
    if(_report) _report->message(sout.str());
}

void KnnMatrix::_FindGatherGroupNeighbors()
{
	vector<GatherKdItem> data;
	for (uint32_t i = 0; i < _gpGroups.size(); i++)
	{
		GatherGroup &gpGroup = _gpGroups[i];
		Vec3f P = gpGroup.bbox.GetCenter();
		Vec3f &N = gpGroup.normal;
		data.push_back(GatherKdItem(i, Vec6f(P, N * _normScale)));
	}

    KdTree<GatherKdItem> *kdTree = new KdTree<GatherKdItem>(data);
    for (uint32_t g = 0; g < _gpGroups.size(); g++)
    {
        GatherGroup &gpGroup = _gpGroups[g];
        Vec6f P(gpGroup.bbox.GetCenter(), gpGroup.normal * _normScale);

        // find mini matrix
        SeedProcess proc(8);
        float r2 = FLT_MAX;
        kdTree->Lookup(P, proc, r2);

        for (uint32_t i = 0; i < proc.foundSeeds; i++)
        {
			CloseSeed &cSeed = proc.closeSeeds[i];
			uint32_t index = cSeed.seedKdItem->idx;
			gpGroup.neighbors.push_back(index);
        }
    }
	delete kdTree;
}


class RenderReducedMatrixThread
{
public:
    typedef uint32_t argument_type;
    RenderReducedMatrixThread(carray2<Vec3f> &matrix, KnnMatrix *knnMat) : _matrix(matrix), _knnMat(knnMat) { };
    void operator()(const uint32_t &g) const;
private:
    carray2<Vec3f>				&_matrix;
    KnnMatrix	                *_knnMat;
};

void RenderReducedMatrixThread::operator()(const uint32_t &g) const
{
	const GatherGroup &gpGroup = _knnMat->_gpGroups[g];
	uint32_t gpIdx = gpGroup.seed;
	const GatherPoint &gp = _knnMat->_gatherPoints[gpIdx];
	for (uint32_t i = 0; i < _matrix.width(); i++)
		_matrix.at(i, g) = _knnMat->RenderCell(LightEvalUtil::EvalL(_knnMat->_clamp), i, gp);
}

void KnnMatrix::_RenderReducedMatrix(carray2<Vec3f> &matrix)
{
	if (_report) _report->beginActivity("render reduced column");
	TbbReportCounter counter((uint32_t)_gpGroups.size(), _report);
	RenderReducedMatrixThread thread(matrix, this);
	parallel_while<RenderReducedMatrixThread> w;
	w.run(counter, thread);
	if (_report) _report->endActivity();
}


void KnnMatrix::_NewInitialClusters(vector<vector<uint32_t> > &clusters, carray2<Vec3f> &matrix, uint32_t budget)
{
	RandomPathSamplerStd sampler;
	if (_report) _report->beginActivity("initial clustering");

	carray2<float> normalizedMatrix(matrix.width(), matrix.height());
	carray<float> norms(matrix.width());

	for (uint32_t i = 0; i < matrix.width(); i++)
	{
		float sum = 0;
		for (uint32_t j = 0; j < matrix.height(); j++)
		{
			float v = matrix.at(i, j).GetLength();
			sum += v * v;
		}
		float n = sqrt(sum);
		norms.at(i) = max(n, 0.0f);
		for (uint32_t j = 0; j < matrix.height(); j++)
		{
			float v = n <= 0.0f ? 0.0f : matrix.at(i, j).GetLength() / n;
			assert(v >= 0.0f);
			normalizedMatrix.at(i, j) = v;
		}
	}

	set<uint32_t> center_set;
	Distribution1Df dist(norms.data(), norms.size());
	if (!dist.IsValid()) {
		while (center_set.size() < budget)	{
			uint32_t idx = static_cast<uint32_t>(sampler.Next1D() * norms.size());
			center_set.insert(idx);
		}
	} else {
		uint32_t s = budget;
		while (s--) {
			float pdf; 
			uint32_t idx = static_cast<uint32_t>(dist.SampleDiscrete(sampler.Next1D(), &pdf));
			center_set.insert(idx);
		}
	}

	vector<uint32_t> centers(center_set.begin(), center_set.end());
	clusters.resize(centers.size());
	for (uint32_t i = 0; i < normalizedMatrix.width(); i++)
	{
		int mini = -1;
		double mind = FLT_MAX;
		for (uint32_t c = 0; c < centers.size(); c++)
		{
			uint32_t cindex = centers[c];
			float d = 0;
			for (uint32_t j = 0; j < normalizedMatrix.height(); j++)
			{
				float v = normalizedMatrix.at(i, j) - normalizedMatrix.at(cindex, j);
				d += v * v;
				assert(d >= 0.0f);
			}

			if (mind >= d)
			{
				mind = d;
				mini = c;
				assert(mini < clusters.size());
			}
		}
		clusters[mini].push_back(i);
	}

	if (_report) _report->endActivity();
}


void KnnMatrix::_InitialClusters(vector<vector<uint32_t> > &clusters, carray2<Vec3f> &matrix, uint32_t budget, bool randProj )
{
	RandomPathSamplerStd sampler;
    if (_report) _report->beginActivity("initial clustering");

    gsl_matrix* origin = gsl_matrix_alloc(matrix.height(), matrix.width());
    for(uint32_t j = 0; j < matrix.height(); j++)
        for (uint32_t i = 0; i < matrix.width(); i++)
            gsl_matrix_set(origin, j, i, matrix.at(i, j).GetLength());

    gsl_matrix* input;
    // random projection?
    if(randProj)
    {
        gsl_matrix* rand_mat = gsl_matrix_alloc(50, matrix.height());
        for(uint32_t j = 0; j < 50; j++)
            for (uint32_t i = 0; i < matrix.height(); i++)
                gsl_matrix_set(rand_mat, j, i, sampler.Next1D());
        input = gsl_matrix_alloc(50, matrix.width());
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, rand_mat, origin, 0.0, input);
        gsl_matrix_free(origin);
    }
    else
        input = origin;


	gsl_vector* norms = gsl_vector_alloc(input->size2);

    vector<uint32_t> nzIndices;
    vector<uint32_t> zIndices;

    vector<double> nzNorms;
    nzNorms.reserve(norms->size);
    for (uint32_t i = 0; i < input->size2; i++)
    {
		gsl_vector_view column = gsl_matrix_column(input, i);
		double d = gsl_blas_dnrm2(&column.vector);
        gsl_vector_set(norms, i, d);
        if (d > 0.0)
        {
            nzNorms.push_back(d);
            nzIndices.push_back(i);
        }
        else
            zIndices.push_back(i);
    }

	gsl_matrix* nzinput = gsl_matrix_alloc(input->size1, nzNorms.size());
	gsl_vector* nznorms = gsl_vector_alloc(nzNorms.size());
	for (uint32_t i = 0; i < nzNorms.size(); i++)
	{
		gsl_vector_set(nznorms, i, nzNorms[i]);
		gsl_vector_view column = gsl_matrix_column(input, nzIndices[i]);
		gsl_matrix_set_col(nzinput, i, &column.vector);
	}

    gsl_vector* sinput = gsl_vector_alloc(nzinput->size1);
    for (uint32_t j = 0; j < nzinput->size1; j++)
    {
        gsl_vector_view row = gsl_matrix_row(nzinput, j);
        double s = gsl_blas_dasum(&row.vector);
        gsl_vector_set(sinput, j, s);
    }

    double snorm = gsl_blas_dasum(nznorms);
    gsl_vector* alphas = gsl_vector_alloc(nzinput->size2);
    {
        gsl_vector *y = gsl_vector_alloc(nzinput->size2);
        gsl_blas_dgemv(CblasTrans, 1.0, nzinput, sinput, 0.0, y);

        gsl_vector_memcpy(alphas, nznorms);
        gsl_vector_scale(alphas, snorm);

        gsl_vector_sub(alphas, y);
        gsl_vector_free(y);
    }

    map<uint32_t, double> centers;
    Distribution1Dd alphaDist(alphas->data, (uint32_t)alphas->size);
    sampler.BeginPixel(budget);
    if (!alphaDist.IsValid())
    {
        while (centers.size() < budget)
        {
            uint32_t idx = static_cast<uint32_t>(sampler.Time() * alphas->size);
            centers[nzIndices[idx]] += alphas->size;
            sampler.NextPixelSample();
        }
    }
    else
    {
        uint32_t s = budget;
        while (s--)
        {
            double pdf; 
            uint32_t idx = static_cast<uint32_t>(alphaDist.SampleDiscrete(sampler.Time(), &pdf));
            centers[nzIndices[idx]] += pdf == 0.0 ? 0.0 : 1.0 / pdf;
            sampler.NextPixelSample();
        }
    }
    sampler.EndPixel();
    assert(centers.size() > 0);

    gsl_vector* knorms = gsl_vector_alloc((uint32_t)centers.size());
    gsl_matrix* kcolumns = gsl_matrix_alloc(input->size1, (uint32_t)centers.size());
    map<uint32_t, double>::iterator it = centers.begin();
    for (uint32_t idx = 0; it != centers.end(); idx++, it++)
    {
        uint32_t i = (uint32_t)it->first;
        assert(i >= 0 && i < norms->size);
        double n = gsl_vector_get(norms, i);
        gsl_vector_set(knorms, idx, n * it->second);

        gsl_vector_view column = gsl_matrix_column(input, i);
        gsl_matrix_set_col(kcolumns, idx, &column.vector);
        gsl_vector_view kcolumn = gsl_matrix_column(kcolumns, idx);
        gsl_vector_scale(&kcolumn.vector, it->second);
    }

    gsl_matrix *mdist = gsl_matrix_alloc(knorms->size, nznorms->size);
    {
		gsl_matrix_view x = gsl_matrix_view_vector(knorms, knorms->size, 1);
		gsl_matrix_view y = gsl_matrix_view_vector(nznorms, nznorms->size, 1);
		gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &x.matrix, &y.matrix, 0.0, mdist);

        gsl_matrix *col2 = gsl_matrix_alloc(kcolumns->size2, nzinput->size2);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, kcolumns, nzinput, 0.0, col2);

        gsl_matrix_sub(mdist, col2);
        gsl_matrix_free(col2);
    }

    clusters.resize(kcolumns->size2);
    for (uint32_t i = 0; i < nzinput->size2; i++)
    {
        int mini = -1;
        double mind = FLT_MAX;
        for (uint32_t j = 0; j < kcolumns->size2; j++)
        {
            double dist = max(0.0, gsl_matrix_get(mdist, j, i));
            if (mind >= dist)
            {
                mind = dist;
                mini = j;
            }
        }
        clusters[mini].push_back(nzIndices[i]);
    }
    gsl_matrix_free(mdist);
	gsl_matrix_free(nzinput);
	gsl_vector_free(nznorms);
	gsl_vector_free(alphas);
	gsl_vector_free(norms);
	gsl_matrix_free(input);

	// fill up lights and ranges

    if (zIndices.size())
		clusters.push_back(zIndices);

    if (_report) _report->endActivity();
}

void KnnMatrix::_KmeanGatherGroup( vector<GatherKdItem> items, uint32_t seedNum )
{
    vector<Vec6f> seeds;
    StratifiedPathSamplerStd sampler;
    seedNum = sampler.RoundSamples(seedNum);
    sampler.BeginPixel(seedNum);
    for (uint32_t i = 0; i < seedNum; i++)
    {
        Ray ray = _scene->MainCamera()->GenerateRay(sampler.Pixel(), sampler.Lens(), 0.0f);
        Intersection isect;
        if(_engine->Intersect(ray, &isect))
        {
            BxdfUnion msu;
            isect.m->SampleReflectance(isect.dp, msu);
            if (msu.HasSmooth())
                seeds.push_back(Vec6f(isect.dp.P, isect.dp.N * _normScale));
        }
        sampler.NextPixelSample();
    }
    sampler.EndPixel();

    vector<concurrent_vector<uint32_t> > clusters(seeds.size());

    GatherPointAssign assign(seeds, items, clusters);
    GatherPointUpdate update(seeds, items, clusters);
    for (uint32_t i = 0; i < 4; i++)
    {
        for (uint32_t i = 0; i < clusters.size(); i++)
            clusters[i].clear();
        parallel_for(blocked_range<uint32_t>(0, (uint32_t)items.size()), assign);
        parallel_for(blocked_range<uint32_t>(0, (uint32_t)seeds.size()), update);
    }

    for (uint32_t i = 0; i < clusters.size(); i++)
    {
        if (clusters[i].size() == 0)
            continue;
        _gpGroups.push_back(GatherGroup());
        GatherGroup &group = _gpGroups.back();
        for (concurrent_vector<uint32_t>::iterator it = clusters[i].begin(); it != clusters[i].end(); it++)
        {
            group.indices.push_back(items[*it].idx);
            GatherPoint &gp = _gatherPoints[items[*it].idx];
            group.bbox.Grow(gp.isect.dp.P);
            group.normal += (gp.isect.dp.N);
        }
        group.normal.Normalize();
    }
}


