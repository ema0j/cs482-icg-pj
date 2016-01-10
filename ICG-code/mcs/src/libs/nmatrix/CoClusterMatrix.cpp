//#include <arma/armadillo>
//#include "CoClusterMatrix.h"
//#include <vlutil/LightEval.h>
//#include <sampler/pathSampler.h>
//#include <misc/report.h>
//#include <imageio/imageio.h>
//#include <vmath/streamMethods.h>
//#include <vlutil/ListVirtualLightCache.h>
//#include "NMatUtils.h"
//#include "GatherGroupTbbImpl.h"
//
//using namespace arma;
//extern string g_DebugFileName;
//
//mt19937 CoClusterMatrix::_rng;
//
//CoClusterMatrix::CoClusterMatrix(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, shared_ptr<VirtualLightGenerator> gen, 
//    shared_ptr<PathSampler> sampler, uint32_t width, uint32_t height, uint32_t samples, shared_ptr<ReportHandler> report) 
//    : _width(width), _height(height), _samples(samples), _scene(scene), _engine(engine), _generator(gen), _sampler(sampler), _report(report) 
//{
//    _nRow = 100;
//    _nCol = 100;
//}
//
//void CoClusterMatrix::Preprocess(uint32_t indirect)
//{
//    _GenerateLights(indirect);
//    _ShootGatherPoints();
//    _InitGatherCluster();
//}
//
//void CoClusterMatrix::_ShootGatherPoints()
//{
//    //if (_report) _report->beginActivity("sampling eye points");
//    //ShootGatherPointThread thread(_scene.get(), _engine.get(), _width, _height, _samples);
//
//    //parallel_while<ShootGatherPointThread> w;
//    //TbbReportCounter counter(_height, _report.get());
//    //w.run( counter, thread );
//
//    //_bkPixels.assign(thread._bkPixels.begin(), thread._bkPixels.end());
//    //_gatherPoints.assign(thread._gatherPoints.begin(), thread._gatherPoints.end());
//    //if (_report) _report->endActivity();
//}
//
//void CoClusterMatrix::_GenerateLights( uint32_t indirect )
//{
//    if (_report) _report->beginActivity("generate virtual lights");
//    shared_ptr<ListVirtualLightCache> cache = shared_ptr<ListVirtualLightCache>(new ListVirtualLightCache(_lightList));
//    _generator->Generate(indirect, cache);
//    if (_report) _report->endActivity();
//}
//
//float ComputeCost( vector<uint32_t> &cls, arma::rowvec & norms, arma::mat &input) 
//{
//    double nsum = 0.0f;
//    double vsum = 0.0f;
//    for (uint32_t i = 0; i < cls.size(); i++)
//    {
//        uint32_t col = cls[i];
//        nsum += norms(col);
//    }
//    for (uint32_t j = 0; j < input.n_rows; j++)
//    {
//        double sum = 0.0f;
//        for (uint32_t i = 0; i < cls.size(); i++)
//        {
//            double v = input.at(j, cls[i]);
//            sum += v;
//        }
//        vsum += sum * sum;
//    }
//
//    return (float)max(0.0, (nsum * nsum) - vsum);
//}
//
//void Clustering( mat input, arma::rowvec norms, uint32_t budget, vector<vector<uint32_t> > &clusters )
//{
//    norms.set_size(input.n_cols);
//
//    for (uint32_t i = 0; i < input.n_cols; i++)
//        norms[i] = norm(input.col(i), 2);
//
//    rowvec sinput = trans(sum(input, 1));
//    double snorm = sum(norms);
//    rowvec alphas = (snorm * norms) - (sinput * input);
//
//    map<uint32_t, double> centers;
//    Distribution1Dd alphaDist(alphas.memptr(), alphas.n_cols);
//    if (!alphaDist.IsValid())
//    {
//        while (centers.size() < budget * 0.66f)
//        {
//            uint32_t idx = static_cast<uint32_t>(CoClusterMatrix::Rand01() * alphas.n_cols);
//            centers[idx] += alphas.n_cols;
//        }
//    }
//    else
//    {
//        uint32_t s = (uint32_t)(budget * 0.66f);
//        while (s--)
//        {
//            double pdf; 
//            uint32_t idx = static_cast<uint32_t>(alphaDist.SampleDiscrete(CoClusterMatrix::Rand01(), &pdf));
//            centers[idx] += 1.0 / pdf;
//        }
//    }
//
//
//    rowvec	kNorm(static_cast<uint32_t>(centers.size()));
//    mat	kCenter(input.n_rows, static_cast<uint32_t>(centers.size()));
//    map<uint32_t, double>::iterator it = centers.begin();
//    for (uint32_t idx = 0; it != centers.end(); idx++, it++)
//    {
//        uint32_t i = (uint32_t)it->first;
//        assert(i >= 0 && i < norms.n_cols);
//        kNorm(idx) = norms(i) * it->second;
//        kCenter.col(idx) = input.col(i) * it->second;
//    }
//
//    mat norm2 = (trans(kNorm) * norms);
//    mat col2 = (trans(kCenter) * input);
//    mat mdist = norm2 - col2;
//
//    clusters.resize(kCenter.n_cols);
//    for (uint32_t i = 0; i < input.n_cols; i++)
//    {
//        int mini = -1;
//        double mind = FLT_MAX;
//        for (uint32_t j = 0; j < kCenter.n_cols; j++)
//        {
//            double dist = max(0.0, mdist(j, i));
//            if (mind > dist)
//            {
//                mind = dist;
//                mini = j;
//            }
//        }
//        clusters[mini].push_back(i);
//    }
//
//    vector<pair<float, uint32_t> > heap;
//    uint32_t nCluster = 0;
//    for (uint32_t i = 0; i < clusters.size(); i++)
//    {
//        if (clusters[i].size() != 0)
//        {
//            nCluster++;
//            float cost = ComputeCost(clusters[i], norms, input);
//            heap.push_back(make_pair<float, uint32_t>(cost, i));
//            push_heap(heap.begin(), heap.end());
//        }
//    }
//
//    while (nCluster < budget)
//    {
//        pop_heap(heap.begin(), heap.end());
//        uint32_t clsIdx = heap.back().second;
//        heap.pop_back();
//        nCluster--;
//
//        rowvec rand_line = randu<rowvec>(input.n_rows);
//
//        // Project down to random line;
//        vector<uint32_t> &cluster = clusters[clsIdx];
//        vector<pair<float, uint32_t> > line(cluster.size());
//        for (uint32_t i = 0; i < line.size(); i++)
//        {
//            uint32_t l = cluster[i];
//            float proj = (float)dot(input.col(l), rand_line);
//            line[i] = pair<float, uint32_t>(proj, l);
//        }
//
//        //// divide line
//        sort(line.begin(), line.end());
//        vector<pair<float, uint32_t> >::iterator middle = line.begin() + 1;
//        float maxInterval = -FLT_MAX;
//        for (vector<pair<float, uint32_t> >::iterator it = line.begin() + 1; it != line.end(); it++)
//        {
//            float interval = (it)->first - (it - 1)->first;
//            if (interval > maxInterval)
//            {
//                maxInterval = interval;
//                middle = it;
//            }
//        }
//        assert(maxInterval >= 0.0f);
//
//        clusters.resize(clusters.size() + 1);
//
//        for (vector<pair<float, uint32_t> >::iterator it = line.begin(); it != middle; it++)
//            clusters[clusters.size() - 1].push_back(it->second);
//        float cost2 = ComputeCost(clusters[clusters.size() - 1], norms, input);
//        clusters[clsIdx].clear();
//        for (vector<pair<float, uint32_t> >::iterator it = middle; it != line.end(); it++)
//            clusters[clsIdx].push_back(it->second);
//        float cost1 = ComputeCost(clusters[clsIdx], norms, input);
//        heap.push_back(make_pair<float, uint32_t>(cost1, clsIdx));
//        push_heap(heap.begin(), heap.end());
//        nCluster++;
//        heap.push_back(make_pair<float, uint32_t>(cost2, (uint32_t)clusters.size() - 1));
//        push_heap(heap.begin(), heap.end());
//        nCluster++;
//    }
//}
//
//
//void CoClusterMatrix::Step()
//{
//    _LightClustering();
//	_GatherClustering();
//
//	_nRow *= 2;
//	_nCol *= 2;
//    
//}
//
//void CoClusterMatrix::_LightClustering()
//{
//    if (_report) _report->beginActivity("render reduced column");
//    _matrix.Alloc(_lightList.GetSize(), (uint32_t)_gpGroups.size());
//    TbbReportCounter2D counter(_lightList.GetSize(), (uint32_t)_gpGroups.size(), _report.get());
//    //RenderRedColThread thread(_matrix, this);
//    //parallel_while<RenderRedColThread> w;
//    //w.run(counter, thread);
//    if (_report) _report->endActivity();
//
//    mat input((uint32_t)_gpGroups.size(), _lightList.GetSize());
//    for(uint32_t j = 0; j < (uint32_t)_gpGroups.size(); j++)
//        for (uint32_t i = 0; i < _lightList.GetSize(); i++)
//            input.at(j, i) = _matrix.ElementAt(i, j).Average();
//
//    vector<vector<uint32_t> > clusters;
//    arma::rowvec norms;
//    Clustering(input, norms, _nCol, clusters);
//
//    _ltGroups.clear();
//    //for(uint32_t k = 0; k < clusters.size(); k++)
//    //{
//    //    vector<uint32_t> &c = clusters[k];
//    //    if (c.size() <= 0)
//    //        continue;
//
//    //    vector<Vec3f> cnorms;
//    //    Vec3f cnormSum;
//
//    //    _ltGroups.resize(_ltGroups.size() + 1);
//    //    LightGroup &ltGroup = _ltGroups[_ltGroups.size() - 1];
//    //    for (uint32_t i = 0; i < c.size(); i++)
//    //    {
//    //        Vec3f n;
//    //        for(uint32_t j = 0; j < _matrix.Height(); j++)
//    //        {
//    //            Vec3f v = _matrix.ElementAt(c[i], j);
//    //            n += v;
//    //        }
//    //        cnorms.push_back(n);
//    //        cnormSum += n;
//    //        ltGroup.indices.push_back(c[i]);
//    //    }
//
//    //    Distribution1D<float, Vec3f> dist(&cnorms[0], cnorms.size());
//    //    float pdf;
//    //    uint64_t idx = dist.SampleDiscrete(Rand01(), &pdf);
//    //    ltGroup.repIdx = ltGroup.indices[idx];
//    //    ltGroup.weight = (cnormSum | cnorms[idx]);
//    //}
//
//    ImageIO::Save(g_DebugFileName + ".redcol.exr", _matrix);
//}
//
//
//void CoClusterMatrix::Render()
//{
//   
//}
//
//void CoClusterMatrix::_InitGatherCluster()
//{
//    vector<uint32_t> centers;
//    for (uint32_t i = 0; i < _nRow; i++)
//    {
//        uint32_t idx = (uint32_t)(_gatherPoints.size() * Rand01());
//        centers.push_back(idx);
//    }
//
//    vector<concurrent_vector<uint32_t> > clusters(centers.size());
//
//    GatherPointAssign assign(centers, _gatherPoints, clusters);
//    GatherPointUpdate update(centers, _gatherPoints, clusters);
//    for (uint32_t i = 0; i < 3; i++)
//    {
//        for (uint32_t i = 0; i < clusters.size(); i++)
//            clusters[i].clear();
//        parallel_for(blocked_range<uint32_t>(0, (uint32_t)_gatherPoints.size()), assign);
//        parallel_for(blocked_range<uint32_t>(0, (uint32_t)centers.size()), update);
//    }
//
//    for (uint32_t i = 0; i < clusters.size(); i++)
//    {
//        if (clusters[i].size() == 0)
//            continue;
//        _gpGroups.resize(_gpGroups.size() + 1);
//        GatherGroup &gpGroup = _gpGroups[_gpGroups.size() - 1];
//        gpGroup.repIdx = centers[i];
//        gpGroup.indices.assign(clusters[i].begin(), clusters[i].end());
//    }
//
//    cout << _gpGroups.size() << " initial gather groups number." << endl;
//}
//
//void CoClusterMatrix::RenderGatherClusters()
//{
//    if (_report) _report->beginActivity("rendering gather point cluster");
//    _image = shared_ptr<Image<Vec3f> >(new Image<Vec3f>(_width, _height));
//    for (uint32_t i = 0; i < _gpGroups.size(); i++)
//    {
//        GatherGroup &gpGroup = _gpGroups[i];
//        Vec3f color(Rand01(), Rand01(), Rand01());
//
//        for (uint32_t j = 0; j < gpGroup.indices.size(); j++)
//        {
//            GatherPoint& gp = _gatherPoints[gpGroup.indices[j]];
//			if (gpGroup.indices[j] == gpGroup.repIdx)
//				_image->ElementAt(gp.pixel.x, _height - gp.pixel.y - 1) = Vec3f::Zero();
//			else
//				_image->ElementAt(gp.pixel.x, _height - gp.pixel.y - 1) = color;
//        }
//        if (_report) _report->progress(i / (float)_gpGroups.size(), 1);
//    }
//    if (_report) _report->endActivity();
//}
//
//void CoClusterMatrix::_GatherClustering()
//{
//	if (_report) _report->beginActivity("render reduced row");
//	_matrix.Alloc((uint32_t)_ltGroups.size(), (uint32_t)_gatherPoints.size());
//	TbbReportCounter2D counter((uint32_t)_ltGroups.size(), (uint32_t)_gatherPoints.size(), _report.get());
//	//RenderRedRowThread thread(_matrix, this);
//	//parallel_while<RenderRedRowThread> w;
//	//w.run(counter, thread);
//	if (_report) _report->endActivity();
//
//	mat input((uint32_t)_ltGroups.size(), (uint32_t)_gatherPoints.size());
//	for(uint32_t j = 0; j < (uint32_t)_gatherPoints.size(); j++)
//		for (uint32_t i = 0; i < (uint32_t)_ltGroups.size(); i++)
//			input.at(i, j) = _matrix.ElementAt(i, j).Average();
//
//
//	vector<vector<uint32_t> > clusters;
//	arma::rowvec norms;
//	Clustering(input, norms, _nRow, clusters);
//
//	_gpGroups.clear();
//	for(uint32_t k = 0; k < clusters.size(); k++)
//	{
//		vector<uint32_t> &c = clusters[k];
//		if (c.size() <= 0)
//			continue;
//
//		vector<Vec3f> cnorms;
//		Vec3f cnormSum;
//
//		_gpGroups.resize(_gpGroups.size() + 1);
//		GatherGroup &gpGroup = _gpGroups[_gpGroups.size() - 1];
//		for (uint32_t j = 0; j < c.size(); j++)
//		{
//			Vec3f n;
//			for(uint32_t i = 0; i < _matrix.Width(); i++)
//				n += _matrix.ElementAt(i, c[j]);
//
//			cnorms.push_back(n);
//			cnormSum += n;
//			gpGroup.indices.push_back(c[j]);
//		}
//
//		Distribution1D<float, Vec3f> dist(&cnorms[0], cnorms.size());
//		float pdf;
//		//uint64_t idx = (uint32_t)(gpGroup.indices.size() * Rand01());
//		uint64_t idx = dist.SampleDiscrete(Rand01(), &pdf);
//		gpGroup.repIdx = gpGroup.indices[idx];
//	}
//}
//
//void CoClusterMatrix::RenderLightClusters()
//{
//	if (_report) _report->beginActivity("rendering light clusters");
//	_image = shared_ptr<Image<Vec3f> >(new Image<Vec3f>(_width, _height));
//	TbbReportCounter counter((uint32_t)_gatherPoints.size(), _report.get());
//	RenderLightGroupThread thread(*_image, this);
//	parallel_while<RenderLightGroupThread> w;
//	w.run(counter, thread);
//	if (_report) _report->endActivity();
//}
//
