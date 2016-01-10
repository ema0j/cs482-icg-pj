#ifndef _GATHER_TREE_BUILDER_H_
#define _GATHER_TREE_BUILDER_H_

#include <lighttree/GatherTree.h>
#include <scene/scene.h>
#include <ray/rayEngine.h>
#include <image/image.h>
#include <misc/report.h>


class GatherForestBuilder
{
public:
    GatherForestBuilder(void);
    ~GatherForestBuilder(void);
    shared_ptr<Image<shared_ptr<GatherTree> > > Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, uint32_t width, uint32_t height, uint32_t samples, carray<float> &timeInstantces, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>()); 
protected:
    shared_ptr<GatherTree> _BuildTree( vector<GatherPoint> &points );
    uint32_t _SplitPoints(vector<GatherNode> &tree, vector<GatherPoint> &gps, vector<GatherPoint>::iterator start, vector<GatherPoint>::iterator end );
private:
    uint32_t _tsample;
    float        __rand();
};
#endif