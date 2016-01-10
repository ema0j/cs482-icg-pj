#include "DivisiveLightTreeBuilder.h"
#include "BuilderLightCache.h"
#include <scene/vlight.h>

LightTree* DivisiveLightTreeBuilder::Build(Scene *scene, RayEngine *engine, uint32_t indirect, ReportHandler *report )
{
    LightTree *lightTree = new LightTree();
    _SampleLights(lightTree, engine, scene, indirect, report);
	lightTree->_orientedRoot = _ConstructSubTree<OrientedLight, OrientedLightTreeNode>(lightTree->_orientedLights, report);
	lightTree->_directionalRoot = _ConstructSubTree<DirLight, DirectionalLightTreeNode>(lightTree->_dirLights, report);
    return lightTree;
}

template<>
void DivisiveLightTreeBuilder::_UpdateNode(OrientedLightTreeNode *node)
{
	OrientedLightTreeNode *l = node->left;
	OrientedLightTreeNode *r = node->right;
	node->bbox = Range3f::Union(l->bbox, r->bbox);
	node->cone = FastConef::Union(l->cone, r->cone);
	node->L = l->L + r->L;
	node->light = (sampler.Next1D() < (l->L.Average() / node->L.Average())) ? l->light : r->light;
}

template<>
void DivisiveLightTreeBuilder::_UpdateNode(DirectionalLightTreeNode *node)
{
	DirectionalLightTreeNode *l = node->left;
	DirectionalLightTreeNode *r = node->right;
	node->bbox = Range3f::Union(l->bbox, r->bbox);
	node->L = l->L + r->L;
	node->light = (sampler.Next1D() < (l->L.Average() / node->L.Average())) ? l->light : r->light;
}

template<>
void DivisiveLightTreeBuilder::_UpdateNode(OmniDirLightTreeNode *node)
{
	OmniDirLightTreeNode *l = node->left;
	OmniDirLightTreeNode *r = node->right;
	node->bbox = Range3f::Union(l->bbox, r->bbox);
	node->L = l->L + r->L;
	node->light = (sampler.Next1D() < (l->L.Average() / node->L.Average())) ? l->light : r->light;
}


MdLightTree* DivisiveMdLightTreeBuilder::Build(Scene *scene, RayEngine *engine, uint32_t indirect, ReportHandler *report )
{
	MdLightTree *lightTree = new MdLightTree();
	_SampleLights(lightTree, engine, scene, indirect, report);
	lightTree->_orientedRoot = _ConstructSubTree<OrientedLight, MdOrientedLightTreeNode>(lightTree->_orientedLights, report);
	lightTree->_directionalRoot = _ConstructSubTree<DirLight, MdDirectionalLightTreeNode>(lightTree->_dirLights, report);
	return lightTree;
}


template<>
void DivisiveMdLightTreeBuilder::_UpdateNode(MdOrientedLightTreeNode *node)
{
	MdOrientedLightTreeNode *l = node->left;
	MdOrientedLightTreeNode *r = node->right;
	node->bbox = Range3f::Union(l->bbox, r->bbox);
	node->cone = FastConef::Union(l->cone, r->cone);
	node->L = l->L + r->L;
	float prob = l->L.Average() / node->L.Average();
#ifdef MULTI_REP
	for (uint32_t i = 0; i < MD_REP_SLOTS; i++)
	{
		if(l->lights[i] && r->lights[i])
			node->lights[i] = (sampler.Next1D() < prob) ? l->lights[i] : r->lights[i];
		else
			node->lights[i] = l->lights[i] ? l->lights[i] : r->lights[i];
	}
#else
	node->light = (sampler.Next1D() < (l->L.Average() / node->L.Average())) ? l->light : r->light;
#endif // MULTI_REP

}

template<>
void DivisiveMdLightTreeBuilder::_UpdateNode(MdDirectionalLightTreeNode *node)
{
	MdDirectionalLightTreeNode *l = node->left;
	MdDirectionalLightTreeNode *r = node->right;
	node->bbox = Range3f::Union(l->bbox, r->bbox);
	node->L = l->L + r->L;
	float prob = l->L.Average() / node->L.Average();
#ifdef MULTI_REP
	for (uint32_t i = 0; i < MD_REP_SLOTS; i++)
	{
		if(l->lights[i] && r->lights[i])
			node->lights[i] = (sampler.Next1D() < prob) ? l->lights[i] : r->lights[i];
		else
			node->lights[i] = l->lights[i] ? l->lights[i] : r->lights[i];
	}
#else
	node->light = (sampler.Next1D() < (l->L.Average() / node->L.Average())) ? l->light : r->light;
#endif
}

template<>
void DivisiveMdLightTreeBuilder::_UpdateNode(MdOmniDirLightTreeNode *node)
{
	MdOmniDirLightTreeNode *l = node->left;
	MdOmniDirLightTreeNode *r = node->right;
	node->bbox = Range3f::Union(l->bbox, r->bbox);
	node->L = l->L + r->L;
	float prob = l->L.Average() / node->L.Average();
#ifdef MULTI_REP
	for (uint32_t i = 0; i < MD_REP_SLOTS; i++)
	{
		if(l->lights[i] && r->lights[i])
			node->lights[i] = (sampler.Next1D() < prob) ? l->lights[i] : r->lights[i];
		else
			node->lights[i] = l->lights[i] ? l->lights[i] : r->lights[i];
	}
#else
	node->light = (sampler.Next1D() < (l->L.Average() / node->L.Average())) ? l->light : r->light;
#endif
}
