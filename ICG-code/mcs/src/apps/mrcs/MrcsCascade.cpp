#include "MrcsCascade.h"
#include <ray/rayEngine.h>
#include <scene/scene.h>
#include <scene/background.h>
#include <scene/camera.h>
#include <vlutil/ListVirtualLightCache.h>
#include <sampler/pathSampler.h>
#include <tbbutils/tbbutils.h>
#include <vlutil/LightEval.h>
#include <gsl/gsl_blas.h>
#include <vmath/range1.h>


MrcsCascade::MrcsCascade(VirtualLightGenerator *gen, Scene *scene, RayEngine *engine)
    : _generator(gen), _scene(scene), _engine(engine)
{
    float radius = (_engine->ComputeBoundingBox().Diagonal() / 2.0f) * 0.05f;
    _clamp = radius * radius;
}

void MrcsCascade::_SetBackground(Image<Vec3f> *image)
{
	for (uint32_t i = 0; i < _bkPixels.size(); i++)
	{
		BackgroundPixel &bkPixel = _bkPixels[i];
		Vec2i &pixel = bkPixel.pixel;
		image->ElementAt(pixel.x, image->Height() - pixel.y - 1) += bkPixel.background * bkPixel.strength;
	}
}

void MrcsCascade::Render(uint32_t indirect, Image<Vec3f> *image, uint32_t samples, uint32_t rows, uint32_t columns, ReportHandler *report)
{
    _report = report;
    _GenerateLights(indirect);
    //_RenderRows(rows);

	// Matrix Sclicing
	//////////////////////////////////////////////////////
	_ShootGatherPoints(image->Width(), image->Height(), samples);
	_GroupGatherPoints(rows);
	_FindGatherGroupNeighbors();

	_matrix.Alloc(_lightList.GetSize(), (uint32_t)(_gpGroups.size()));

	_RenderReducedMatrix(_matrix);

	_SetBackground(image);
	///////////////////////////////////////////////////////
	// Cascade Clustering
    _MrcsCluster(columns, samples);
    _RenderFinalImage(image, samples);
}

void MrcsCascade::_GenerateLights(uint32_t indirect)
{
    ListVirtualLightCache cache(_lightList);
    _generator->Generate(indirect, &cache, 0.0f, _report);
}

struct CascadeMrcsReducedColumnThread
{
public:
    CascadeMrcsReducedColumnThread(MrcsCascade *renderer, const vector<Ray> &rays)
        : _renderer(renderer), _rays(rays) {};
    void operator()(const blocked_range<uint32_t> &r) const;
private:
    MrcsCascade                *_renderer;
    const vector<Ray>		    &_rays;
};

void CascadeMrcsReducedColumnThread::operator()( const blocked_range<uint32_t> &r ) const
{
	for (uint32_t j = r.begin(); j != r.end(); j++)
    {
        Ray ray = _rays[j];
        _renderer->_RenderRow(ray, j);
    }
}

void MrcsCascade::_ShootGatherPoints(uint32_t width, uint32_t height, uint32_t samples)
{
	if (_report) _report->beginActivity("sampling eye points");
	GatherPointShooter::Shoot(_scene, _engine, width, height, samples, _gatherPoints, _bkPixels, _report);
	if (_report) _report->endActivity();
}

void MrcsCascade::_KdGatherGroup(vector<GatherKdItem>::iterator start, vector<GatherKdItem>::iterator end)
{
	Range6f bbox = Range6f::Empty();
	for (vector<GatherKdItem>::iterator it = start; it != end; it++)
		bbox.Grow(it->p);
	if (end - start < _maxGatherGroupSize)
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

void MrcsCascade::_GroupGatherPoints(uint32_t seedNum)
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
	if (_report) _report->message(sout.str());
}

void MrcsCascade::_FindGatherGroupNeighbors()
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

class CascadeReducedMatrixThread
{
public:
	typedef uint32_t argument_type;
	CascadeReducedMatrixThread(Image<Vec3f> &matrix, MrcsCascade *knnMat) : _matrix(matrix), _knnMat(knnMat) { };
	void operator()(const uint32_t &g) const;
private:
	Image<Vec3f>				&_matrix;
	MrcsCascade                *_knnMat;
};

void CascadeReducedMatrixThread::operator()(const uint32_t &g) const
{
	const GatherGroup &gpGroup = _knnMat->_gpGroups[g];
	uint32_t gpIdx = gpGroup.seed;
	const GatherPoint &gp = _knnMat->_gatherPoints[gpIdx];
	for (uint32_t i = 0; i < _matrix.Width(); i++)
		_matrix.ElementAt(i, g) = _knnMat->RenderCell(LightEvalUtil::EvalL(_knnMat->_clamp), i, gp);
}
void MrcsCascade::_RenderReducedMatrix(Image<Vec3f> &matrix)
{
	if (_report) _report->beginActivity("render reduced column");
	TbbReportCounter counter((uint32_t)_gpGroups.size(), _report);
	CascadeReducedMatrixThread thread(matrix, this);
	parallel_while<CascadeReducedMatrixThread> w;
	w.run(counter, thread);
	if (_report) _report->endActivity();
}


void MrcsCascade::_RenderRow(Ray &ray, uint32_t r )
{
    uint32_t maxSpecDepth = 5;
    Intersection isect;
    while(_engine->Intersect(ray, &isect))
    {
        BxdfUnion msu;
        isect.m->SampleReflectance(isect.dp, msu);
        if(msu.HasDelta())
        {
            if (maxSpecDepth == 0)
                break;

            maxSpecDepth--;
            BxdfSample sample = msu.SampleCos(DELTA_BXDF, -ray.D, isect.dp, Vec2f::Zero(), 0.0f);
            ray = Ray(isect.dp.P, sample.wi, isect.rayEpsilon, RAY_INFINITY, ray.time);
            continue;
        }
        else if (msu.HasSmooth())
        {
            Vec3f wo = -ray.D;
            for (uint32_t i = 0; i < _lightList.GetSize(); i++)
                _matrix.ElementAt(i, r) = _RenderCell(i, isect.dp, wo, isect.m, isect.rayEpsilon);
        }
        break;
    }
}

void MrcsCascade::_RenderRows(uint32_t rows)
{
    if(_report) _report->beginActivity("Render Reduced Column");
	_matrix.Alloc(_lightList.GetSize(), rows);
    vector<Ray> rays;
	StratifiedPathSamplerStd sampler;
	uint32_t samples = sampler.RoundSamples(rows);
    sampler.BeginPixel(samples);
    for (uint32_t i = 0; i < samples; i++)
    {
        Ray ray = _scene->MainCamera()->GenerateRay(sampler.Pixel(), 0.0f);
        rays.push_back(ray);
        sampler.NextPixelSample();
    }
    sampler.EndPixel();

    CascadeMrcsReducedColumnThread thread(this, rays);
    //TbbReportCounter counter((uint32_t)rays.size(), _report.get());
    //parallel_while<CascadeMrcsReducedColumnThread> w;
    //w.run(counter, thread);
    parallel_for(blocked_range<uint32_t>(0, samples), thread);

    //ImageIO::Save("reduced_column.exr", _matrix);
    if(_report) _report->endActivity();
}

Vec3f MrcsCascade::_RenderCell(uint32_t col, DifferentialGeometry &dp, Vec3f &wo, Material *m, float rayEpsilon)
{
    LightEvalUtil::EvalLight eval(_clamp);
    Vec3f L;
    switch (_lightList.GetLightType(col))
    {
    case OMNIDIR_LIGHT:
        assert(false); 
        L = Vec3f::Zero();
        break;
    case DIRECTIONAL_LIGHT:
        L = eval(*reinterpret_cast<DirLight*>(_lightList.GetLight(col)), 
            dp, wo, m, _engine, rayEpsilon);
        break;
    case ORIENTED_LIGHT:
		L = eval(*reinterpret_cast<OrientedLight*>(_lightList.GetLight(col)),
            dp, wo, m, _engine, rayEpsilon);
        break;
    }
    return L;
}

void MrcsCascade::_MRCSClustering(gsl_matrix* input, RandomPathSamplerStd &sampler, uint32_t budget, vector<vector<uint32_t> > &clusters)
{
	gsl_vector* norms = gsl_vector_alloc(input->size2);
	for (uint32_t i = 0; i < input->size2; i++)
	{
		gsl_vector_view column = gsl_matrix_column(input, i);
		double d = gsl_blas_dnrm2(&column.vector);
		gsl_vector_set(norms, i, d);
	}

	gsl_vector* sinput = gsl_vector_alloc(input->size1);
	for (uint32_t j = 0; j < input->size1; j++)
	{
		gsl_vector_view row = gsl_matrix_row(input, j);
		double s = gsl_blas_dasum(&row.vector);
		gsl_vector_set(sinput, j, s);
	}

	double snorm = gsl_blas_dasum(norms);
	gsl_vector* alphas = gsl_vector_alloc(input->size2);
	{
		gsl_vector *y = gsl_vector_alloc(input->size2);
		gsl_blas_dgemv(CblasTrans, 1.0, input, sinput, 0.0, y);

		gsl_vector_memcpy(alphas, norms);
		gsl_vector_scale(alphas, snorm);

		gsl_vector_sub(alphas, y);
		gsl_vector_free(y);
	}

	map<uint32_t, double> centers;
	Distribution1Dd alphaDist(alphas->data, (uint32_t)alphas->size);
	if (!alphaDist.IsValid())
	{
		while (centers.size() < budget * 0.66f) // initial clustering only 2/3
		{
			uint32_t idx = min(static_cast<uint32_t>(sampler.Next1D() * alphas->size), static_cast<uint32_t>(alphas->size - 1));
			centers[idx] += alphas->size;
		}
	}
	else
	{
		uint32_t s = (uint32_t)(budget * 0.66f);
		while (s--)
		{
			double pdf;
			uint32_t idx = static_cast<uint32_t>(alphaDist.SampleDiscrete(sampler.Next1D(), &pdf));
			centers[idx] += pdf == 0.0 ? 0.0 : 1.0 / pdf;
		}
	}
	assert(centers.size() > 0);

	// top-down splitting
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

	gsl_matrix *mdist = gsl_matrix_alloc(knorms->size, norms->size);
	{
		gsl_matrix_view x = gsl_matrix_view_vector(knorms, knorms->size, 1);
		gsl_matrix_view y = gsl_matrix_view_vector(norms, norms->size, 1);
		gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &x.matrix, &y.matrix, 0.0, mdist);

		gsl_matrix *col2 = gsl_matrix_alloc(kcolumns->size2, input->size2);
		gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, kcolumns, input, 0.0, col2);

		gsl_matrix_sub(mdist, col2);
		gsl_matrix_free(col2);
	}

	clusters.resize(kcolumns->size2);
	for (uint32_t i = 0; i < input->size2; i++)
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
		clusters[mini].push_back(i);
	}
	gsl_matrix_free(mdist);

	vector<pair<double, uint32_t> > heap;
	uint32_t nCluster = 0;
	for (uint32_t i = 0; i < clusters.size(); i++)
	{
		if (clusters[i].size() != 0)
		{
			double cost = _ComputeCost(clusters[i], norms, input);
			heap.push_back(make_pair(cost, i));
			push_heap(heap.begin(), heap.end());
		}
	}

	while (clusters.size() < budget)
	{
		pop_heap(heap.begin(), heap.end());
		if (heap.back().first <= 0.0)
			break;
		uint32_t index = heap.back().second;
		double cost = heap.back().first;
		heap.pop_back();

		// Project down to random line;
		vector<uint32_t> &cluster = clusters.at(index);
		gsl_vector *randLine = gsl_vector_alloc(input->size1);

		for (uint32_t i = 0; i < input->size1; i++)
			gsl_vector_set(randLine, i, sampler.Next1D());

		Range1f r = Range1f::Empty();
		vector<pair<double, uint32_t> > line(cluster.size());
		for (uint32_t i = 0; i < line.size(); i++)
		{
			uint32_t l = cluster[i];
			double proj = 0.0;
			gsl_blas_ddot(&gsl_matrix_column(input, l).vector, randLine, &proj);
			line[i] = make_pair(proj, l);
			r.Grow((float)proj);
		}
		gsl_vector_free(randLine);

		float pmid = r.GetCenter();
		vector<pair<double, uint32_t> >::iterator middle;
		middle = std::partition(line.begin(), line.end(), [pmid](const pair<double, uint32_t> &a) { return a.first < pmid; });
		assert(middle - line.begin() >= 1);
		assert(line.end() - middle >= 1);

		clusters.resize(clusters.size() + 1);

		for (vector<pair<double, uint32_t> >::iterator it = line.begin(); it != middle; it++)
			clusters.back().push_back(it->second);
		double cost2 = _ComputeCost(clusters[clusters.size() - 1], norms, input);
		if (cost2 > cost)
			continue;
		assert(cost2 <= cost);
		clusters[index].clear();
		for (vector<pair<double, uint32_t> >::iterator it = middle; it != line.end(); it++)
			clusters[index].push_back(it->second);
		double cost1 = _ComputeCost(clusters[index], norms, input);
		if (cost1 > cost)
			continue;
		assert(cost1 <= cost);
		heap.push_back(make_pair(cost1, index));
		push_heap(heap.begin(), heap.end());
		heap.push_back(make_pair(cost2, (uint32_t)clusters.size() - 1));
		push_heap(heap.begin(), heap.end());
	}

	gsl_vector_free(alphas);
	gsl_vector_free(norms);
	//gsl_matrix_free(input);
}

void MrcsCascade::_MRCSReprLight(vector<vector<uint32_t>> &clusters, RandomPathSamplerStd &sampler, 
	vector<Vec3f> &colorNorms, vector<uint32_t> &r_light)
{
	for (uint32_t k = 0; k < clusters.size(); k++)
	{
		vector<uint32_t> &c = clusters[k];
		if (c.size() <= 0)
			continue;

		vector<Vec3f> cnorms;
		Vec3f cnormSum;
		for (uint32_t i = 0; i < c.size(); i++)
		{
			Vec3f &v = colorNorms[c[i]];
			cnorms.push_back(v);
			cnormSum += v;
		}

		if (!cnormSum.IsZero())
		{
			Distribution1D<float, Vec3f> dist(&cnorms[0], (uint32_t)cnorms.size());
			float pdf;
			//_scaledLights.push_back(ScaledLight());
			//ScaledLight &light = _scaledLights.back();
#ifdef MULTI_REP
			for (uint32_t s = 0; s < samples; s++)
			{
				uint64_t idx = dist.SampleDiscrete(_Random01(), &pdf);
				light.idx.push_back(c[idx]);
				light.weight.push_back(cnormSum | cnorms[idx]);
			}
#else
			uint64_t idx = dist.SampleDiscrete(sampler.Next1D(), &pdf);
			//light.idx = c[idx];
			//light.weight = cnormSum | cnorms[idx];

			r_light.push_back(c[idx]);
#endif
		}
	}
}

void MrcsCascade::_MrcsCluster( uint32_t budget, uint32_t samples) // #columns, #sample (1)
{
	RandomPathSamplerStd sampler;
    if(_report) _report->beginActivity("Mrcs Cluster");
    vector<vector<uint32_t> >   clusters;

    vector<Vec3f>               colorNorms;
    gsl_matrix* origin = gsl_matrix_alloc(_matrix.Height(), _matrix.Width());
    for (uint32_t i = 0; i < origin->size2; i++) 
    {
        Vec3f norm;
        for(uint32_t j = 0; j < origin->size1; j++) 
        {
            Vec3f &v = _matrix.ElementAt(i, j);
            gsl_matrix_set(origin, j, i, v.GetLength());
            norm += v * v;
        }
        colorNorms.push_back(norm.Sqrt());
    }

	//random projection
	gsl_matrix* rand_mat = gsl_matrix_alloc(50, _matrix.Height()); 
	for(uint32_t j = 0; j < 50; j++)
		for (uint32_t i = 0; i < _matrix.Height(); i++)
			gsl_matrix_set(rand_mat, j, i, sampler.Next1D());
	gsl_matrix* input = gsl_matrix_alloc(50, _matrix.Width());
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, rand_mat, origin, 0.0, input); // rand_mat*origin = input
	gsl_matrix_free(origin);

	gsl_vector* norms = gsl_vector_alloc(input->size2); // norms of columns
	for (uint32_t i = 0; i < input->size2; i++)
	{
		gsl_vector_view column = gsl_matrix_column(input, i);
		double d = gsl_blas_dnrm2(&column.vector);
		gsl_vector_set(norms, i, d);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	////////// CLUSTERING ///////////////////////////////////////////////////////////////////
	uint32_t num_main_light = (uint32_t)(budget * (50.0/100.0));
	vector<uint32_t> r_light;
	_MRCSClustering(input, sampler, num_main_light, clusters);
	_MRCSReprLight(clusters, sampler, colorNorms, r_light);
	//Post-processing : get representative light index / remove lights which sim > threshold / 
	//					make new input matrix based on random projected matrix
	//cout << "# first clustering : " << r_light.size() << endl;

	double threshold = 0.0001;
	vector<uint32_t> low_clusters;
	vector<vector<uint32_t>> high_clusters;
	for (uint32_t k = 0; k < clusters.size(); k++)
	{
		vector<uint32_t> &c = clusters[k];
		if (c.size() <= 0)
			continue;

		uint32_t repr = r_light[k];
		double minvalue = FLT_MAX;
		vector<uint32_t> high_cluster;
		for (uint32_t r = 0; r < clusters[k].size(); r++)
		{
			double diff = abs(gsl_vector_get(norms, repr) - gsl_vector_get(norms, clusters[k][r]));
			if (diff != 0.0 && diff > threshold)
				low_clusters.push_back(clusters[k][r]);
			else
				high_cluster.push_back(clusters[k][r]);
		}
		high_clusters.push_back(high_cluster);
	}
	cout << "*** After post-processing, #lights : " << low_clusters.size() << endl;

	// new input matrix
	gsl_matrix* new_input = gsl_matrix_alloc(50, low_clusters.size());
	vector<Vec3f> new_colorNorms;
	map<uint32_t, uint32_t> idx_mapper;
	clusters.clear();
	r_light.clear();
	for (uint32_t k = 0; k < low_clusters.size(); k++)
	{
		gsl_vector_view column = gsl_matrix_column(input, low_clusters[k]);
		gsl_matrix_set_col(new_input, k, &column.vector);
		new_colorNorms.push_back(colorNorms[low_clusters[k]]);
		idx_mapper[k] = low_clusters[k];
	}

	_MRCSClustering(new_input, sampler, budget - num_main_light, clusters);
	//_MRCSReprLight(clusters, sampler, new_colorNorms, r_light);
	for (uint32_t k = 0; k < clusters.size(); k++)
	{
		vector<uint32_t> &c = clusters[k];
		if (c.size() <= 0)
			continue;

		vector<Vec3f> cnorms;
		Vec3f cnormSum;
		for (uint32_t i = 0; i < c.size(); i++)
		{
			Vec3f &v = new_colorNorms[c[i]];
			cnorms.push_back(v);
			cnormSum += v;
		}

		if (!cnormSum.IsZero())
		{
			Distribution1D<float, Vec3f> dist(&cnorms[0], (uint32_t)cnorms.size());
			float pdf;
			_scaledLights.push_back(ScaledLight());
			ScaledLight &light = _scaledLights.back();
#ifdef MULTI_REP
			for (uint32_t s = 0; s < samples; s++)
			{
				uint64_t idx = dist.SampleDiscrete(_Random01(), &pdf);
				light.idx.push_back(c[idx]);
				light.weight.push_back(cnormSum | cnorms[idx]);
			}
#else
			uint64_t idx = dist.SampleDiscrete(sampler.Next1D(), &pdf);
			light.idx = idx_mapper.find(c[idx])->second;
			light.weight = cnormSum | cnorms[idx];

			r_light.push_back(c[idx]);
#endif
		}
	}
	for (uint32_t k = 0; k < high_clusters.size(); k++)
	{
		vector<uint32_t> &c = high_clusters[k];
		if (c.size() <= 0)
			continue;

		vector<Vec3f> cnorms;
		Vec3f cnormSum;
		for (uint32_t i = 0; i < c.size(); i++)
		{
			Vec3f &v = colorNorms[c[i]];
			cnorms.push_back(v);
			cnormSum += v;
		}

		if (!cnormSum.IsZero())
		{
			Distribution1D<float, Vec3f> dist(&cnorms[0], (uint32_t)cnorms.size());
			float pdf;
			_scaledLights.push_back(ScaledLight());
			ScaledLight &light = _scaledLights.back();
#ifdef MULTI_REP
			for (uint32_t s = 0; s < samples; s++)
			{
				uint64_t idx = dist.SampleDiscrete(_Random01(), &pdf);
				light.idx.push_back(c[idx]);
				light.weight.push_back(cnormSum | cnorms[idx]);
			}
#else
			uint64_t idx = dist.SampleDiscrete(sampler.Next1D(), &pdf);
			light.idx = c[idx];
			light.weight = cnormSum | cnorms[idx];

			r_light.push_back(c[idx]);
#endif
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////

    if(_report) _report->endActivity();
}

double MrcsCascade::_ComputeCost( const vector<uint32_t> &cluster, gsl_vector* norms, gsl_matrix* input )
{
    if (cluster.size() <= 1)
        return 0.0;

    double nsum = 0.0f;
    double vsum = 0.0f;
    for (uint32_t i = 0; i < cluster.size(); i++)
    {
        uint32_t col = cluster[i];
        double n = gsl_vector_get(norms, col);
        nsum += n;
    }
    for (uint32_t j = 0; j < input->size1; j++)
    {
        double sum = 0.0f;
        for (uint32_t i = 0; i < cluster.size(); i++)
        {
            double v = gsl_matrix_get(input, j, cluster[i]); 
            sum += v;
        }
        vsum += sum * sum;
    }
    return max(0.0, (nsum * nsum) - vsum);
}

struct FinalMrcsCascadeThread
{
public:
    typedef uint32_t argument_type;

	FinalMrcsCascadeThread(MrcsCascade *renderer, Image<Vec3f> *image, Image<uint64_t> *seeds, uint32_t samples);
    void operator()(uint32_t j) const;
    void operator()(const blocked_range2d<uint32_t> &r) const;
private:
    uint32_t                            _samples;
    Vec2f                               _pixelSize;
	Image<uint64_t>						*_randSeeds;
    Image<Vec3f>						*_image;
    MrcsCascade                        *_renderer;
    Scene                               *_scene;
};


FinalMrcsCascadeThread::FinalMrcsCascadeThread(MrcsCascade *renderer, Image<Vec3f> *image, Image<uint64_t> *seeds, uint32_t samples)
    : _renderer(renderer), _image(image), _randSeeds(seeds), _samples(samples)
{
    _scene = _renderer->_scene;
    _pixelSize = Vec2f(1.0f / _image->Width(), 1.0f / _image->Height()); 
}

void FinalMrcsCascadeThread::operator()(uint32_t j) const
{
	uint64_t seed = _randSeeds->ElementAt(j);
	StratifiedPathSamplerStd::Engine e(seed);
	StratifiedPathSamplerStd sampler(e);

    for (uint32_t i = 0; i < _image->Width(); i++)
    {
        Vec2i pixel(i, j);
        if (_samples == 1)
        {
            Vec2f puv = (Vec2f(pixel) + Vec2f((float)0.5f, (float)0.5f)) * _pixelSize;
            Ray ray = _scene->MainCamera()->GenerateRay(puv, Vec2f((float)0.5f, (float)0.5f), 0.0f);
            _image->ElementAt(i, _image->Height() - j - 1) = _renderer->_RenderRay(ray, 0);
        }
        else
        {
            Vec3f L;
            sampler.BeginPixel(_samples);
            for (uint32_t s = 0; s < _samples; s++)
            {
                Vec2f puv = (Vec2f(pixel) + sampler.Pixel()) * _pixelSize;
                Ray ray = _scene->MainCamera()->GenerateRay(puv, sampler.Lens(), sampler.Time());
                L += _renderer->_RenderRay(ray, s) / (float)_samples;
                sampler.NextPixelSample();
            }
            sampler.EndPixel();
            _image->ElementAt(i, _image->Height() - j - 1) = L;
        }
    }
}


void MrcsCascade::_RenderFinalImage(Image<Vec3f> *image, uint32_t samples ) // fill the image
{
    if(_report) _report->beginActivity("Render Final Image");

	Image<uint64_t> randSeeds(image->Width(), image->Height());
	for (uint32_t i = 0; i < randSeeds.Size(); i++)
	{
		static minstd_rand0 seeder;
		uint64_t seed = seeder();
		randSeeds.ElementAt(i) = seed; 
	}

	FinalMrcsCascadeThread thread(this, image, &randSeeds, samples);

    TbbReportCounter counter(image->Height(), _report);
	parallel_while<FinalMrcsCascadeThread> w;
    w.run(counter, thread);
    if(_report) _report->endActivity();
}

Vec3f MrcsCascade::_RenderRay( Ray ray, uint32_t s )
{
    Vec3f throughput = Vec3f::One();

    uint32_t maxSpecDepth = 10;
    Intersection isect;
    bool hit = false;
    while(_engine->Intersect(ray, &isect))
    {
        BxdfUnion msu;
        isect.m->SampleReflectance(isect.dp, msu);
        if(msu.HasDelta())
        {
            if (maxSpecDepth == 0)
                break;
            maxSpecDepth--;
            BxdfSample sample = msu.SampleCos(DELTA_BXDF, -ray.D, isect.dp, Vec2f::Zero(), 0.0f);
            throughput *= sample.brdfCos;
            ray = Ray(isect.dp.P, sample.wi, isect.rayEpsilon, RAY_INFINITY, ray.time);
            continue;
        }
        else if (msu.HasSmooth())
        {
            Vec3f wo = -ray.D;
            Vec3f emission = msu.Emission(wo, isect.dp);
            Vec3f L;
            for (uint32_t i = 0; i < _scaledLights.size(); i++)
            {
                ScaledLight &light = _scaledLights[i];
#ifdef MULTI_REP
                L += _RenderCell(light.idx[s], isect.dp, wo, isect.m, isect.rayEpsilon) * light.weight[s];
#else
                L += _RenderCell(light.idx, isect.dp, wo, isect.m, isect.rayEpsilon) * light.weight;
#endif  
            }
            return emission + throughput * L;
        }
        break;
    }
    if (!hit && maxSpecDepth != 0 && _scene->MainBackground())
    {
        return throughput * _scene->MainBackground()->SampleBackground(ray.D, ray.time);
    }
    return Vec3f::Zero();
}


