#ifndef _GATHER_TREE_BUILDER_H_
#define _GATHER_TREE_BUILDER_H_

#include <lighttree/GatherTree.h>
#include <scene/scene.h>
#include <ray/rayEngine.h>
#include <image/image.h>
#include <misc/report.h>
#include <sampler/pathSampler.h>

struct GpKdItem
{
    uint32_t    idx;
    Vec6f       point;
};


class GatherTreeBuilder
{
public:
    GatherTreeBuilder(Scene *scene, RayEngine *engine, uint32_t width, uint32_t height, uint32_t samples, float c);
    ~GatherTreeBuilder(void);

    GatherNode*				Build(uint32_t i, uint32_t j, uint64_t seed, vector<GatherPoint> &points, Vec3f *background); 
protected:
    GatherNode*             _BuildTree( vector<GatherPoint> &points, PathSampler* sampler);
    GatherNode*             _Build( vector<GpKdItem>::iterator start, vector<GpKdItem>::iterator end, vector<GatherPoint> &points, PathSampler* sampler);
    GatherNode*             _MakeLeaf( vector<GpKdItem>::iterator it, vector<GatherPoint> &points );
    float                   _BoundGlossyCos( const Range3f &gbox );

private:
    uint32_t                _width;
    uint32_t                _height;
    Vec2f                   _pixelSize;
    uint32_t                _samples;
    Scene					*_scene;
    RayEngine				*_engine;
    float                   _normalScale;
    float                   _radius;

    Vec6f                   _ComputeKdPoint( GatherPoint &gp );
	void					_UpdateNode(GatherNode* node, PathSampler *sampler);

};
#endif