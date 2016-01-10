#ifndef _DIVISIVE_LIGHT_TREE_BUILDER_H_
#define _DIVISIVE_LIGHT_TREE_BUILDER_H_
#include "LightTreeBuilder.h"
#include <sampler/pathSampler.h>

using namespace LightTreeBuilderUtil;

class DivisiveLightTreeBuilder : public LightTreeBuilder 
{
public:
	DivisiveLightTreeBuilder(VirtualLightGenerator *gen) : LightTreeBuilder(gen) { };
	~DivisiveLightTreeBuilder(void) {};
	LightTree* Build(Scene *scene, RayEngine *engine, uint32_t indirect, ReportHandler *report = 0);
private:
	void        _ConstructTree(LightTree* lightTree, ReportHandler *reportt);
	template<typename LightType, typename NodeType>
	NodeType*   _ConstructSubTree(vector<LightType> &lights, ReportHandler *report);
	template<typename NodeType>
	NodeType*   _BuildSubTree(typename vector<LightTreeBuildItem<NodeType> >::iterator start, typename vector<LightTreeBuildItem<NodeType> >::iterator end);
	template<typename NodeType>
	NodeType*   _MakeLeaf( typename vector<LightTreeBuildItem<NodeType> >::iterator start ) { return start->node; }
	template<typename NodeType>
	void		_UpdateNode(NodeType *node);
	RandomPathSamplerStd	sampler;
};


template<typename NodeType>
NodeType* DivisiveLightTreeBuilder::_BuildSubTree(typename vector<LightTreeBuildItem<NodeType> >::iterator start, typename vector<LightTreeBuildItem<NodeType> >::iterator end)
{
    typedef typename vector<LightTreeBuildItem<NodeType> >::iterator DataIter;
    assert(end > start);
    if (end - start == 1)
        return _MakeLeaf<NodeType>(start);

    Range6f bbox = Range6f::Empty();
    for (DataIter it = start; it != end; it++)
        bbox.Grow(it->point);

    uint32_t dim = bbox.GetSize().MaxComponentIndex();
    DataIter mid = start + (end - start) / 2;
	std::nth_element(start, mid, end, 
		[dim](const LightTreeBuildItem<NodeType> &d1, const LightTreeBuildItem<NodeType> &d2)->bool {
		return d1.point[dim] == d2.point[dim] ? (d1.node < d2.node) : d1.point[dim] < d2.point[dim]; 
	});

    NodeType *node = new NodeType();
    node->left = _BuildSubTree<NodeType>(start, mid);
    node->right = _BuildSubTree<NodeType>(mid, end);
    _UpdateNode(node);
    return node;
}

template<typename LightType, typename NodeType>
NodeType* DivisiveLightTreeBuilder::_ConstructSubTree( vector<LightType> &lights, ReportHandler *report )
{
    if (lights.size() < 1)
        return NULL;
    if (report) report->beginActivity("initialize light input data");
    vector<LightTreeBuildItem<NodeType> > inputData;
    for (uint32_t i = 0; i < lights.size(); i++)
    {
        NodeType *node = new NodeType(lights[i]);
        inputData.push_back(LightTreeBuildItem<NodeType> (node, _c));
    }
    if (report) report->endActivity();

    if (report) report->beginActivity("build light tree by division");
    NodeType *node = _BuildSubTree<NodeType>(inputData.begin(), inputData.end());
    if (report) report->endActivity();
    return node;
}


class DivisiveMdLightTreeBuilder : public LightTreeBuilder 
{
public:
	DivisiveMdLightTreeBuilder(VirtualLightGenerator *gen) : LightTreeBuilder(gen) { };
	~DivisiveMdLightTreeBuilder(void) {};
	MdLightTree* Build(Scene *scene, RayEngine *engine, uint32_t indirect, ReportHandler *report = 0);
private:
	void        _ConstructTree(MdLightTree* lightTree, ReportHandler *reportt);
	template<typename LightType, typename NodeType>
	NodeType*   _ConstructSubTree(vector<LightType> &lights, ReportHandler *report);
	template<typename NodeType>
	NodeType*   _BuildSubTree(typename vector<LightTreeBuildItem<NodeType> >::iterator start, typename vector<LightTreeBuildItem<NodeType> >::iterator end);
	template<typename NodeType>
	NodeType*   _MakeLeaf( typename vector<LightTreeBuildItem<NodeType> >::iterator start ) { return start->node; }
	template<typename NodeType>
	void		_UpdateNode(NodeType *node);
	RandomPathSamplerStd	sampler;
};


template<typename LightType, typename NodeType>
NodeType* DivisiveMdLightTreeBuilder::_ConstructSubTree( vector<LightType> &lights, ReportHandler *report )
{
	if (lights.size() < 1)
		return NULL;
	if (report) report->beginActivity("initialize light input data");
	vector<LightTreeBuildItem<NodeType> > inputData;
	for (uint32_t i = 0; i < lights.size(); i++)
	{
#ifdef MULTI_REP
		uint32_t index = static_cast<uint32_t>(MD_REP_SLOTS * sampler.Next1D());
		NodeType *node = new NodeType(lights[i], index);
#else
		NodeType *node = new NodeType(lights[i]);
#endif
		inputData.push_back(LightTreeBuildItem<NodeType> (node, _c));
	}
	if (report) report->endActivity();

	if (report) report->beginActivity("build light tree by division");
	NodeType *node = _BuildSubTree<NodeType>(inputData.begin(), inputData.end());
	if (report) report->endActivity();
	return node;
}

template<typename NodeType>
NodeType* DivisiveMdLightTreeBuilder::_BuildSubTree(typename vector<LightTreeBuildItem<NodeType> >::iterator start, typename vector<LightTreeBuildItem<NodeType> >::iterator end)
{
	typedef typename vector<LightTreeBuildItem<NodeType> >::iterator DataIter;
	assert(end > start);
	if (end - start == 1)
		return _MakeLeaf<NodeType>(start);

	Range6f bbox = Range6f::Empty();
	for (DataIter it = start; it != end; it++)
		bbox.Grow(it->point);

	uint32_t dim = bbox.GetSize().MaxComponentIndex();
	DataIter mid = start + (end - start) / 2;
	std::nth_element(start, mid, end, 
		[dim](const LightTreeBuildItem<NodeType> &d1, const LightTreeBuildItem<NodeType> &d2)->bool {
			return d1.point[dim] == d2.point[dim] ? (d1.node < d2.node) : d1.point[dim] < d2.point[dim]; 
	});

	NodeType *node = new NodeType();
	node->left = _BuildSubTree<NodeType>(start, mid);
	node->right = _BuildSubTree<NodeType>(mid, end);
	_UpdateNode(node);
	return node;
}

#endif // _DIVISIVE_LIGHT_TREE_BUILDER_H_
