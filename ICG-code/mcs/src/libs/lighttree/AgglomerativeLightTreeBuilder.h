#ifndef _AGGLOMERATIVE_LIGHT_TREE_BUILDER_H_H
#define _AGGLOMERATIVE_LIGHT_TREE_BUILDER_H_H
#include "LightTreeBuilder.h"


class AgglomerativeLightTreeBuilder : public LightTreeBuilder
{
public:
    AgglomerativeLightTreeBuilder(shared_ptr<VirtualLightGenerator> gen) : LightTreeBuilder(gen) {}
    virtual ~AgglomerativeLightTreeBuilder(void) {}
    virtual shared_ptr<LightTree> Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, 
        uint32_t indirect, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>());
    
private:
    void _ConstructTree( shared_ptr<LightTree> lightTree, shared_ptr<ReportHandler> report );
    template<typename LightType, typename NodeType>
    NodeType*   _ConstructSubTree(vector<LightType> &lights, shared_ptr<ReportHandler> report );
    template<typename NodeType>
    NodeType*   _BuildDivisiveTree(typename vector<NodePoint<NodeType> >::iterator start, typename vector<NodePoint<NodeType> >::iterator end);
    template<typename NodeType>
    NodeType*   _DivisiveMakeLeaf( typename vector<NodePoint<NodeType> >::iterator start ) { return start->node; }
    template<typename NodeType>
    NodeType*   _RebuildTreeAgglomerative(NodeType* root);
};

struct AggLessOp
{
    AggLessOp(uint32_t d) : dim(d) {}
    template<typename P>
    bool operator()(const P &d1, const P &d2) const 
    {
        return d1.point[dim] == d2.point[dim] ? (d1.node < d2.node) : d1.point[dim] < d2.point[dim];
    }
    uint32_t dim;
};

template<typename NodeType>
NodeType* AgglomerativeLightTreeBuilder::_BuildDivisiveTree(typename vector<NodePoint<NodeType> >::iterator start, typename vector<NodePoint<NodeType> >::iterator end)
{
    typedef typename vector<NodePoint<NodeType> >::iterator DataIter;
    assert(end > start);
    if (end - start == 1)
        return _DivisiveMakeLeaf<NodeType>(start);

    NodeApproxT<NodeType>::range bbox = NodeApproxT<NodeType>::range::Empty();
    for (DataIter it = start; it != end; it++)
        bbox.Grow(it->point);

    uint32_t splitDim = bbox.GetSize().MaxComponentIndex();
    DataIter mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, AggLessOp(splitDim));

    NodeType *node = new NodeType();
    node->left = _BuildDivisiveTree<NodeType>(start, mid);
    node->right = _BuildDivisiveTree<NodeType>(mid, end);
    node->Update();
    return node;
}

template<typename LightType, typename NodeType>
NodeType* AgglomerativeLightTreeBuilder::_ConstructSubTree( vector<LightType> &lights, shared_ptr<ReportHandler> report )
{
    if (lights.size() < 1)
        return NULL;
    if (report) report->beginActivity("initialize light input data");
    vector<NodePoint<NodeType> > inputData;
    for (uint32_t i = 0; i < lights.size(); i++)
    {
        NodeType *node = new NodeType(i, lights[i]);
        inputData.push_back(NodePoint<NodeType> (node, _c));
    }
    if (report) report->endActivity();

    if (report) report->beginActivity("build light tree by division");
    NodeType *root = _BuildDivisiveTree<NodeType>(inputData.begin(), inputData.end());
    if (report) report->endActivity();

    if (report) report->beginActivity("rebuild light tree by agglomerative");
    NodeType *node = _RebuildTreeAgglomerative<NodeType>(root);
    if (report) report->endActivity();  

    return node;
}

template<typename NodeType>
NodeType* AgglomerativeLightTreeBuilder::_RebuildTreeAgglomerative(NodeType* root)
{
    return NULL;
}


#endif // _AGGLOMERATIVE_LIGHT_TREE_BUILDER_H_H
