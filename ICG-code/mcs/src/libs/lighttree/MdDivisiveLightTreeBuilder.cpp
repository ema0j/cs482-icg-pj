#include "MdDivisiveLightTreeBuilder.h"

shared_ptr<LightTree> MdDivisiveLightTreeBuilder::Build( shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, const carray<float> &tsamples, uint32_t indirect, shared_ptr<ReportHandler> report /*= shared_ptr<ReportHandler>()*/ )
{
    shared_ptr<LightTree> lightTree = shared_ptr<LightTree>(new LightTree<MdRepData>());
    vector<shared_ptr<SimpleVirtualLightCache> > caches;
    _SampleLights(caches, engine, scene, tsamples, indirect, report);
    _ConstructTree(lightTree, caches, report);
    return lightTree;
}

void MdDivisiveLightTreeBuilder::_ConstructTree(shared_ptr<LightTree> lightTree, vector<shared_ptr<SimpleVirtualLightCache> > &caches, shared_ptr<ReportHandler> report)
{
    vector<vector<OrientedLight>*>  orientedLightList;
    vector<vector<DirLight>*>       dirLightList;

    for (uint32_t s = 0; s < SAMPLE_FRAME_NUM; s++)
    {
        shared_ptr<SimpleVirtualLightCache> cache = caches[s];
        orientedLightList.push_back(&cache->OrientedLights());
        dirLightList.push_back(&cache->DirectionalLights());
    }

    lightTree->_orientedRoot = _ConstructSubTree<OrientedLight, OrientedLightTreeNode>(lightTree->_orientedLights, orientedLightList, report);
    lightTree->_directionalRoot = _ConstructSubTree<DirLight, DirectionalLightTreeNode>(lightTree->_dirLights, dirLightList, report);
}