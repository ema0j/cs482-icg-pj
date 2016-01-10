#include "AgglomerativeLightTreeBuilder.h"


shared_ptr<LightTree> AgglomerativeLightTreeBuilder::Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, 
                                    uint32_t indirect, shared_ptr<ReportHandler> report)
{
    shared_ptr<LightTree> lightTree = shared_ptr<LightTree>(new LightTree());
    _SampleLights(lightTree, engine, scene, indirect, report);
    _ConstructTree(lightTree, report);
    return lightTree;
}

void AgglomerativeLightTreeBuilder::_ConstructTree( shared_ptr<LightTree> lightTree, shared_ptr<ReportHandler> report )
{
    lightTree->_orientedRoot = _ConstructSubTree<OrientedLight, OrientedLightTreeNode<RepData> >(lightTree->_orientedLights, report);
}
