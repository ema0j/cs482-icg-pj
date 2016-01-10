#ifndef _LIGHT_CUT_INTEGRATOR_H_
#define _LIGHT_CUT_INTEGRATOR_H_
#include <lighttree/LightTree.h>
#include <image/image.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <scene/scene.h>
#include <ray/rayEngine.h>

class Lightcutter
{
public:
    Lightcutter(LightTree *lightTree, Scene *scene, RayEngine *engine, float error = 0.02f, uint32_t maxCutSize = 1000);
    ~Lightcutter(void);

    virtual void	Lightcut(Image<Vec3f> *image, uint32_t samples, Image<uint32_t> *cutImage, ReportHandler *report = 0);
    virtual Vec3f   EvaluateLightcut( const Ray &ray, uint32_t &cutSize );
protected:
    Vec3f           _EvalutateNode(const OrientedLightTreeNode *node, DifferentialGeometry& dp, const Vec3f &wo, float rayEpsilon, Material *ms);
    Vec3f           _EvalutateNode(const DirectionalLightTreeNode *node, DifferentialGeometry& dp, const Vec3f &wo, float rayEpsilon, Material *ms);

    Vec3f           _ComputeUpperBound(const OrientedLightTreeNode *node, DifferentialGeometry& dp, const Vec3f &wo, Material *ms);
    Vec3f           _ComputeUpperBound(const DirectionalLightTreeNode *node, DifferentialGeometry& dp, const Vec3f &wo, Material *ms);
    Vec3f           _BoundMaterial(const Range3f &bbox, const Vec3f &wo, const DifferentialGeometry & dp, const Material *m, bool dirLight );
    Vec3f           _EvaluateCut(DifferentialGeometry &dp, const Vec3f &wo, float rayEpsilon, Material *m, uint32_t &cs );
    Scene								*_scene;
    RayEngine							*_engine;
    uint32_t                            _maxCutSize;
    LightTree							*_lightTree;
    float                               _clamp;
    float                               _error;
};

#endif // _LIGHT_CUT_INTEGRATOR_H_
