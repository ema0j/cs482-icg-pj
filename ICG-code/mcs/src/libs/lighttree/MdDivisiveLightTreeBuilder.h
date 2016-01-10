#ifndef _MD_DIVISIVE_LIGHT_TREE_BUILDER_H_
#define _MD_DIVISIVE_LIGHT_TREE_BUILDER_H_
#include "LightTree.h"
#include "MdLightTreeBuilder.h"
#include "LightTreeBuilderUtil.h"

using namespace LightTreeBuilderUtil;

class MdDivisiveLightTreeBuilder : public MdLightTreeBuilder 
{
public:
    MdDivisiveLightTreeBuilder(shared_ptr<VirtualLightGenerator> gen) : MdLightTreeBuilder(gen) { };
    ~MdDivisiveLightTreeBuilder(void) {};
    virtual shared_ptr<LightTree> Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, const carray<float> &tsamples,
        uint32_t indirect, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>());
private:
    void        _ConstructTree(shared_ptr<LightTree> lightTree, vector<shared_ptr<SimpleVirtualLightCache> > &caches, shared_ptr<ReportHandler> report);
    template<typename LightType, typename NodeType>
    NodeType*   _ConstructSubTree(vector<LightType> &lights, const vector<vector<LightType>*> &lightList, shared_ptr<ReportHandler> report );
    template<typename NodeType>
    NodeType*   _Build(typename vector<NodePoint<NodeType> >::iterator start, typename vector<NodePoint<NodeType> >::iterator end);
    template<typename NodeType>
    NodeType*   _MakeLeaf( typename vector<NodePoint<NodeType> >::iterator start ) { return start->node; }
};

struct MdDivLessOp
{
    MdDivLessOp(uint32_t d) : dim(d) {}
    template<typename P>
    bool operator()(const P &d1, const P &d2) const 
    {
        return d1.point[dim] == d2.point[dim] ? (d1.node < d2.node) : d1.point[dim] < d2.point[dim];
    }
    uint32_t dim;
};

template<typename NodeType>
NodeType* MdDivisiveLightTreeBuilder::_Build(typename vector<NodePoint<NodeType> >::iterator start, typename vector<NodePoint<NodeType> >::iterator end)
{
    typedef typename vector<NodePoint<NodeType> >::iterator DataIter;
    assert(end > start);
    if (end - start == 1)
        return _MakeLeaf<NodeType>(start);

    NodeApproxT<NodeType>::range bbox = NodeApproxT<NodeType>::range::Empty();
    for (DataIter it = start; it != end; it++)
        bbox.Grow(it->point);

    uint32_t splitDim = bbox.GetSize().MaxComponentIndex();
    DataIter mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, MdDivLessOp(splitDim));

    NodeType *node = new NodeType();
    node->left = _Build<NodeType>(start, mid);
    node->right = _Build<NodeType>(mid, end);
    node->Update();
    return node;
}

template<typename LightType, typename NodeType>
NodeType* MdDivisiveLightTreeBuilder::_ConstructSubTree( vector<LightType> &lights, const vector<vector<LightType>*> &lightList, shared_ptr<ReportHandler> report )
{
    if (lightList.size() < 1)
        return NULL;

    if (report) report->beginActivity("initialize light input data");
    vector<NodePoint<NodeType> > inputData;
    for (uint32_t s = 0; s < SAMPLE_FRAME_NUM; s++)
    {
        vector<LightType> &lList = *lightList[s];
        for (uint32_t i = 0; i < lList.size(); i++)
        {
            lights.push_back(lList[i]);
            uint32_t idx = (uint32_t)(lights.size() - 1);
            NodeType *node = new NodeType(idx, lights[idx], s);
            inputData.push_back(NodePoint<NodeType> (node, _c));
        }
    }
    if (report) report->endActivity();

    if (lights.size() < 1)
        return NULL;

    if (report) report->beginActivity("build light tree by division");
    NodeType *node = _Build<NodeType>(inputData.begin(), inputData.end());
    if (report) report->endActivity();
    return node;
}


#endif // _MD_DIVISIVE_LIGHT_TREE_BUILDER_H_

