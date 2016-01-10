#ifndef _LIGHT_TREE_BUILDER_H_
#define _LIGHT_TREE_BUILDER_H_
#include <lighttree/LightTree.h>
#include <scene/scene.h>
#include <ray/rayEngine.h>
#include <misc/report.h>
#include <scene/vlight.h>
#include <lightgen/LightGenerator.h>
#include "LightTreeBuilderUtil.h"
#include "matutil/SimpleVirtualLightCache.h"

class MdLightTreeBuilder
{
public:
    MdLightTreeBuilder(shared_ptr<VirtualLightGenerator> gen) : _generator(gen) {}
    virtual ~MdLightTreeBuilder(void) {}
    virtual shared_ptr<LightTree> Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, const carray<float> &tsamples,
                                                    uint32_t indirect, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>()) = 0;
protected:
    void                                _SampleLights(vector<shared_ptr<SimpleVirtualLightCache> > &caches, shared_ptr<RayEngine> engine, shared_ptr<Scene> scene, 
                                                      const carray<float> &tsamples, uint32_t indirect, shared_ptr<ReportHandler> report);    
    float                               _c;
    shared_ptr<VirtualLightGenerator>   _generator;
};
#endif