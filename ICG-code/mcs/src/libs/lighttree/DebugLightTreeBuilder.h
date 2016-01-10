#ifndef _DEBUG_LIGHT_TREE_BUILDER_H_
#define _DEBUG_LIGHT_TREE_BUILDER_H_
#include "LightTreeBuilder.h"
#include <set>

using std::set;

class DebugLightTreeBuilder : public LightTreeBuilder
{
public:
    DebugLightTreeBuilder(shared_ptr<VirtualLightGenerator> gen) : LightTreeBuilder(gen) {}
    ~DebugLightTreeBuilder(void) {}
    virtual shared_ptr<LightTree > Build(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, uint32_t indirect, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>());
protected:
    void        _ConstructTree( shared_ptr<LightTree > lightTree, shared_ptr<ReportHandler> report );
    template<typename LightType, typename NodeType>
    NodeType*   _ConstructSubTree(vector<LightType> &lights, shared_ptr<ReportHandler> report );
};

template<typename LightType, typename NodeType>
NodeType* DebugLightTreeBuilder::_ConstructSubTree( vector<LightType> &lights, shared_ptr<ReportHandler> report )
{
    if (lights.size() < 1)
        return NULL;
    if (report) report->beginActivity("initialize light input data");
    set<NodeType*> active;
    for (uint32_t i = 0; i < lights.size(); i++)
    {
        NodeType *node = new NodeType(lights[i]);
        active.insert(node);
    }
    if (report) report->endActivity();

    typedef typename set<NodeType*>::iterator setiter;
    if (report) report->beginActivity("build light tree by division");
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
