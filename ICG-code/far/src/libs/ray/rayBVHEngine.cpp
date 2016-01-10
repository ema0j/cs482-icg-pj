#include "rayBVHEngine.h"
#include "rayTesselationCache.h"

Intervalf BvhEngineGroupNode::ValidInterval()
{
    return Intervalf::Invalid();
}

bool BvhEngineGroupNode::Intersect(const Ray& r, Intersection* intersection)
{
    Ray ray = r;
    if (!_data->bvhNodes.size()) 
        return false;
    bool hit = false;
    Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
    uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
    // Follow ray through BVH nodes to find primitive intersections
    uint32_t todoOffset = 0, nodeNum = 0;
    uint32_t todo[64];
    while (true) {
        const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
        // Check ray against BVH node
        if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
        {
            if (node->nPrimitives > 0) {
                // Intersect ray with primitives in leaf BVH node
                for (uint32_t i = 0; i < node->nPrimitives; ++i)
                {
                    if (_data->groups[node->primitivesOffset+i]->Intersect(ray, intersection))
                    {
                        ray.tMax = intersection->t;
                        hit = true;
                    }
                }
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
            else {
                // Put far BVH node on _todo_ stack, advance to near node
                if (dirIsNeg[node->axis]) {
                    todo[todoOffset++] = nodeNum + 1;
                    nodeNum = node->secondChildOffset;
                }
                else {
                    todo[todoOffset++] = node->secondChildOffset;
                    nodeNum = nodeNum + 1;
                }
            }
        }
        else {
            if (todoOffset == 0) break;
            nodeNum = todo[--todoOffset];
        }
    }
    return hit;
}

bool BvhEngineGroupNode::IntersectAny(const Ray& r)
{
    if (!_data->bvhNodes.size()) 
        return false;

    Ray ray = r;
    Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
    uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
    uint32_t todo[64];
    uint32_t todoOffset = 0, nodeNum = 0;
    while (true) {
        const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
        if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
        {
            // Process BVH node _node_ for traversal
            if (node->nPrimitives > 0) {
                for (uint32_t i = 0; i < node->nPrimitives; ++i) {
                    if (_data->groups[node->primitivesOffset+i]->IntersectAny(ray)) 
                    {
                            return true;
                    }
                    else {
                    }
                }
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
            else {
                if (dirIsNeg[node->axis]) {
                    /// second child first
                    todo[todoOffset++] = nodeNum + 1;
                    nodeNum = node->secondChildOffset;
                }
                else {
                    todo[todoOffset++] = node->secondChildOffset;
                    nodeNum = nodeNum + 1;
                }
            }
        }
        else {
            if (todoOffset == 0) break;
            nodeNum = todo[--todoOffset];
        }
    }
    return false;
}

void BvhEngineGroupNode::CollectStats(StatsManager& stats)
{
    for(size_t p = 0; p < _data->groups.size(); p++) 
        _data->groups[p]->CollectStats(stats);
}

Range3f BvhEngineGroupNode::ComputeBoundingBox()
{
    Range3f bbox = Range3f::Empty();
    for(size_t i = 0; i < _data->groups.size(); i ++)
        bbox.Grow(_data->groups[i]->ComputeBoundingBox());
    return bbox;
}

float BvhEngineGroupNode::ComputeAverageArea()
{
    float area = 0;
    for(size_t i = 0; i < _data->groups.size(); i ++)
        area += _data->groups[i]->ComputeAverageArea();
    return area;
}
