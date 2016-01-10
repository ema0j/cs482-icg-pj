//#ifndef _CO_CLUSTER_MATRIX_H_
//#define _CO_CLUSTER_MATRIX_H_
//#include <ray/intersection.h>
//#include <lightgen/LightData.h>
//#include <image/image.h>
//#include <sampler/Distribution1D.h>
//#include <lightgen/LightGenerator.h>
//#include "vmath/random.h"
//#include <gpshoot/GatherPointShooter.h>
//
//class Scene;
//class RayEngine;
//class PathSampler;
//class ReportHandler;
//
//class CoClusterMatrix
//{
//public:
//    CoClusterMatrix(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, shared_ptr<VirtualLightGenerator> gen, shared_ptr<PathSampler> sampler, uint32_t width, uint32_t height, uint32_t samples, shared_ptr<ReportHandler> report);
//    void                                    Preprocess(uint32_t indirect);
//    void                                    Step();
//
//    void                                    Render();
//    void                                    RenderGatherClusters();
//	void									RenderLightClusters();
//    template<typename T>
//    Vec3f                                   RenderCell(const T &t, uint32_t col, uint32_t row );
//    template<typename T>
//    Vec3f                                   RenderCell(const T &t, uint32_t col, GatherPoint &gp );
//    GatherPoint&                            GetGatherPoint(uint32_t i) { return _gatherPoints[i]; }
//    GatherGroup&                            GetGatherGroup(uint32_t i) { return _gpGroups[i]; }
//    LightGroup&                             GetLightGroup(uint32_t i) { return _ltGroups[i]; }
//    uint32_t								LightGroupNum() const { return (uint32_t)_ltGroups.size(); }
//    shared_ptr<Image<Vec3f> >               GetImage() const { return _image; }
//
//    static float                            Rand01() { return uniform1D01<float>(_rng); }
//protected:
//    void                                    _LightClustering();
//	void									_GatherClustering();
//    void								    _ShootGatherPoints();
//    void								    _GenerateLights(uint32_t indirect);
//    void                                    _InitGatherCluster();
//	
//	Image<Vec3f>							_matrix;
//    shared_ptr<Image<Vec3f> >               _image;
//private:
//    shared_ptr<ReportHandler>               _report;
//    shared_ptr<PathSampler>                 _sampler;
//    shared_ptr<Scene>                       _scene;
//    shared_ptr<RayEngine>                   _engine;
//    shared_ptr<VirtualLightGenerator>       _generator;
//    uint32_t                                _width;
//    uint32_t                                _height;
//    uint32_t                                _samples;
//
//    uint32_t                                _nRow;
//    uint32_t                                _nCol;
//
//    vector<GatherPoint>                     _gatherPoints;
//    vector<BackgroundPixel>                 _bkPixels;
//    LightList                               _lightList;
//
//    vector<GatherGroup>                     _gpGroups;
//    vector<LightGroup>                      _ltGroups;
//
//    static mt19937                          _rng;
//};
//
//template<typename T>
//Vec3f CoClusterMatrix::RenderCell(const T &t, uint32_t col, uint32_t row )
//{
//    GatherPoint &gp = _gatherPoints[row];
//    return RenderCell(t, col, gp);
//}
//
//template<typename T>
//Vec3f CoClusterMatrix::RenderCell(const T &t, uint32_t col, GatherPoint &gp )
//{
//    Vec3f L;
//    switch (_lightList.GetLightType(col))
//    {
//    case OMNIDIR_LIGHT:
//        assert(false); 
//        L = Vec3f::Zero();
//        break;
//    case DIRECTIONAL_LIGHT:
//        L = t(*reinterpret_cast<DirLight*>(_lightList.GetLight(col)), 
//            gp.isect.dp, gp.wo, gp.isect.m, _engine, gp.isect.rayEpsilon);
//        break;
//    case ORIENTED_LIGHT:
//        L = t(*reinterpret_cast<OrientedLight*>(_lightList.GetLight(col)), 
//            gp.isect.dp, gp.wo, gp.isect.m, _engine, gp.isect.rayEpsilon);
//        break;
//    }
//    return L;
//}
//#endif // _CO_CLUSTER_MATRIX_H_