#include "DebugLightTreeBuilder.h"
shared_ptr<LightTree> DebugLightTreeBuilder::Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, uint32_t indirect, shared_ptr<ReportHandler> report)
{
    shared_ptr<LightTree > lightTree = shared_ptr<LightTree >(new LightTree());
    _SampleLights(lightTree, engine, scene, indirect, report);
    _ConstructTree(lightTree, report);
    return lightTree;
}

void DebugLightTreeBuilder::_ConstructTree( shared_ptr<LightTree> lightTree, shared_ptr<ReportHandler> report )
{
    lightTree->_orientedRoot = _ConstructSubTree<OrientedLight, OrientedLightTreeNode >(lightTree->_orientedLights, report);
    lightTree->_directionalRoot = _ConstructSubTree<DirLight, DirectionalLightTreeNode >(lightTree->_dirLights, report);
}
