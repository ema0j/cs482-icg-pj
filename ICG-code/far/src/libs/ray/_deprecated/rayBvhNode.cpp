#include "rayBvhNode.h"
#include <misc/stats.h>
#include <vmath/vec3.h>
#include "rayPrimitive.h"

void _BvhInternalNode::CollectStats(StatsManager& stats, int depth)
{
    StatsCounterVariable* internalNodes = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Bvh Internal Nodes");
    StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Bvh Memory");
    internalNodes->Increment();
    memory->Increment(sizeof(*this));
    if(leftChild)  leftChild->CollectStats(stats, depth+1);
    if(rightChild) rightChild->CollectStats(stats, depth+1);
}

void _BvhInternalNode::Print(FILE* f, int depth)
{
    for(int i = 0; i < depth * 2; i ++) 
        fputc(' ', f);
    Vec3f M = bbox.GetMax();
    Vec3f m = bbox.GetMin();
    fprintf(f, "internal bbox: %f %f %f %f %f %f\n", M.x, M.y, M.z, m.x, m.y, m.z);
    leftChild->Print(f, depth+1 );
    rightChild->Print(f, depth+1 );
}

void _BvhLeafNode::CollectStats(StatsManager& stats, int depth)
{
    StatsCounterVariable* leafNodes = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Bvh Leaf Nodes");
    StatsDistributionVariable* leafPrims = stats.GetVariable<StatsDistributionVariable>("Ray Engine", "Bvh Leaf Prims");
    StatsDistributionVariable* leafDepth = stats.GetVariable<StatsDistributionVariable>("Ray Engine", "Bvh Leaf Depth");
    StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Bvh Memory");
    leafNodes->Increment();
    leafPrims->Value(end - start);
    leafDepth->Value(depth);
    memory->Increment(sizeof(*this) + (end - start) * sizeof(RayPrimitive*));
}

void _BvhLeafNode::Print(FILE* f, int depth)
{
    for(int i = 0; i < depth*2; i ++) fputc(' ', f);
    Vec3f M = bbox.GetMax();
    Vec3f m = bbox.GetMin();
    fprintf(f, "leaf with %d, bbox: %f %f %f %f %f %f\n", end - start, M.x, M.y, M.z, m.x, m.y, m.z);
}
