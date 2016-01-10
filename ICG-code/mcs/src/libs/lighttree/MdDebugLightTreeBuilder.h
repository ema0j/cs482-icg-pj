#ifndef _DEBUG_LIGHT_TREE_BUILDER_H_
#define _DEBUG_LIGHT_TREE_BUILDER_H_
#include <set>
#include "MdLightTreeBuilder.h"

using std::set;

class MdDebugLightTreeBuilder : public MdLightTreeBuilder
{
public:
    MdDebugLightTreeBuilder(shared_ptr<VirtualLightGenerator> gen) : MdLightTreeBuilder(gen) {}
    ~MdDebugLightTreeBuilder(void) {}
    virtual shared_ptr<LightTree> Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, const carray<float> &tsamples, uint32_t indirect, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>());
protected:
    void        _ConstructTree( shared_ptr<LightTree> lightTree, vector<shared_ptr<SimpleVirtualLightCache> > &caches, shared_ptr<ReportHandler> report );
    template<typename LightType, typename NodeType>
    NodeType*   _ConstructSubTree(vector<LightType> &lights, const vector<vector<LightType>*> &lightList, shared_ptr<ReportHandler> report );
};

template<typename LightType, typename NodeType>
NodeType* MdDebugLightTreeBuilder::_ConstructSubTree( vector<LightType> &lights, const vector<vector<LightType>*> &lightList, shared_ptr<ReportHandler> report )
{
    if (lightList.size() < 1)
        return NULL;
    if (report) report->beginActivity("initialize light input data");

    set<NodeType*> active;
    for (uint32_t s = 0; s < SAMPLE_FRAME_NUM; s++)
    {
        vector<LightType> &lList = *lightList[s];
        for (uint32_t i = 0; i < lList.size(); i++)
        {
            lights.push_back(lList[i]);
            uint32_t idx = (uint32_t)(lights.size() - 1);
            NodeType *node = new NodeType(idx, lights[idx], s);
            active.insert(node);
        }
    }
    if (report) report->endActivity();

    if (lights.size() < 1)
        return NULL;

    typedef typename set<NodeType*>::iterator setiter;
    if (report) report->beginActivity("debug build light tree");
    while (active.size() > 1)
    {
        float bestD = FLT_MAX;
        NodeType* left = NULL;
        NodeType* right = NULL;
        for(setiter itA = active.begin(); itA != active.end(); itA++)
        {
            for (setiter itB = active.begin(); itB != active.end(); itB++)
            {
                if (*itA == *itB)
                    continue;
                float dist = LightTreeBuilderUtil::Distance(*itA, *itB, _c);
                if (dist < bestD)
                {
                    bestD = dist;
                    left = *itA;
                    right = *itB;
                }
            }
        }
        NodeType *newNode = new NodeType();
        newNode->left = left;
        newNode->right = right;
        newNode->Update();

        active.erase(left);
        active.erase(right);
        active.insert(newNode);
    }
    if (report) report->endActivity();
    return *active.begin();
}


#endif // _DEBUG_LIGHT_TREE_BUILDER_H_
