#include "rayBVHEngineData.h"


uint32_t FlattenBVHTree(BVHBuildNode *node, vector<LinearBVHNode> &nodes, uint32_t *offset) 
{
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    uint32_t myOffset = (*offset)++;
    if (node->nPrimitives > 0) 
    {
        assert(!node->children[0] && !node->children[1]);
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    }
    else 
    {
        // Creater interior flattened BVH node
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        FlattenBVHTree(node->children[0], nodes, offset);
        linearNode->secondChildOffset = FlattenBVHTree(node->children[1], nodes, offset);
    }
    return myOffset;
}

void GroupBvh::BuildBVH()
{
    if (groups.size() == 0)
        return;

    vector<BVHItem> buildData;
    buildData.reserve(groups.size());
    for (uint32_t i = 0; i < groups.size(); ++i) 
    {
        Range3f bbox = groups[i]->ComputeBoundingBox();
        buildData.push_back(BVHItem(i, bbox));
    }

    vector<RayEngine* > ordered;
    ordered.reserve(groups.size());

    uint64_t totalNodes = 0;
    BVHBuildNode *root = RecursiveBuildBVH<RayEngine*>(SPLIT_SAH, buildData.begin(), buildData.end(), &totalNodes, groups, ordered);

    groups = ordered;

    bvhNodes.resize(totalNodes);
    uint32_t offset = 0;
    FlattenBVHTree(root, bvhNodes, &offset);
}

void SphereBvh::BuildBVH()
{
    if (centers.size() == 0)
        return;

    vector<BVHItem> buildData;
    buildData.reserve(centers.size());
    vector<uint32_t> prims(centers.size());
    for (uint32_t i = 0; i < centers.size(); ++i) 
    {
        Range3f bbox = ElementOperations::SphereBoundingBox(centers[i], radius[i]);
        buildData.push_back(BVHItem(i, bbox));
        prims[i] = i;
    }

    ordered.reserve(centers.size());
    uint64_t totalNodes = 0;
    BVHBuildNode *root = RecursiveBuildBVH<uint32_t>(SPLIT_SAH, buildData.begin(), buildData.end(), &totalNodes, prims, ordered);

    bvhNodes.resize(totalNodes);
    uint32_t offset = 0;
    FlattenBVHTree(root, bvhNodes, &offset);
}

void TriangleBvh::BuildBVH()
{
    if (faces.size() == 0)
        return;

    vector<BVHItem> buildData;
    buildData.reserve(faces.size());
    vector<uint32_t> prims(faces.size());
    for (uint32_t i = 0; i < faces.size(); ++i) 
    {
        Vec3f &v0 = positions[faces[i][0]];
        Vec3f &v1 = positions[faces[i][1]];
        Vec3f &v2 = positions[faces[i][2]];
        Range3f bbox = ElementOperations::TriangleBoundingBox(v0, v1, v2);
        buildData.push_back(BVHItem(i, bbox));
        prims[i] = i;
    }

    ordered.reserve(faces.size());
    uint64_t totalNodes = 0;
    BVHBuildNode *root = RecursiveBuildBVH<uint32_t>(SPLIT_SAH, buildData.begin(), buildData.end(), &totalNodes, prims, ordered);

    bvhNodes.resize(totalNodes);
    uint32_t offset = 0;
    FlattenBVHTree(root, bvhNodes, &offset);
}

void SegmentBvh::BuildBVH()
{
    if (segments.size() == 0)
        return;

    vector<BVHItem> buildData;
    buildData.reserve(segments.size());
    vector<uint32_t> prims(segments.size());
    for (uint32_t i = 0; i < segments.size(); ++i) 
    {
        Vec3f &v0 = positions[segments[i][0]];
        Vec3f &v1 = positions[segments[i][1]];
        float r = radius;
        Range3f bbox = ElementOperations::CylinderBoundingBox(v0, v1, r);
        buildData.push_back(BVHItem(i, bbox));
        prims[i] = i;
    }

    ordered.reserve(segments.size());
    uint64_t totalNodes = 0;
    BVHBuildNode *root = RecursiveBuildBVH<uint32_t>(SPLIT_SAH, buildData.begin(), buildData.end(), &totalNodes, prims, ordered);

    bvhNodes.resize(totalNodes);
    uint32_t offset = 0;
    FlattenBVHTree(root, bvhNodes, &offset);
}



