#ifndef _RAY_BVH_ENGINE_DATA_H_
#define _RAY_BVH_ENGINE_DATA_H_
#include <stdint.h>
#include <misc/stdcommon.h>
#include "rayEngine.h"

struct BVHItem 
{
    BVHItem() {}
    BVHItem(uint32_t i, const Range3f &b) : idx(i), bbox(b) { centroid = bbox.GetCenter(); }
    uint32_t        idx;
    Range3f         bbox;
    Vec3f           centroid;
};

struct BVHBuildNode 
{
    // BVHBuildNode Public Methods
    BVHBuildNode() { children[0] = children[1] = NULL; }
    void InitLeaf(uint32_t first, uint32_t n, const Range3f &b) 
    {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
    }
    void InitInterior(uint8_t axis, BVHBuildNode *c0, BVHBuildNode *c1) 
    {
        children[0] = c0;
        children[1] = c1;
        bounds = Range3f::Union(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }
    Range3f bounds;
    BVHBuildNode *children[2];
    uint8_t splitAxis;
    uint32_t firstPrimOffset, nPrimitives;
};

struct LinearBVHNode {
    Range3f bounds;
    union
    {
        uint32_t primitivesOffset;    // leaf
        uint32_t secondChildOffset;   // interior
    };

    uint32_t nPrimitives;  // 0 -> interior node
    uint8_t axis;         // interior node: xyz
};

struct ComparePoints {
    ComparePoints(int d) { dim = d; }
    int dim;
    bool operator()(const BVHItem &a,
        const BVHItem &b) const {
            return a.centroid[dim] < b.centroid[dim];
    }
};

struct CompareToBucket {
    CompareToBucket(int split, int num, int d, const Range3f &b)
        : centroidBounds(b)
    { splitBucket = split; nBuckets = num; dim = d; }
    bool operator()(const BVHItem &p) const
    {
        int b = (uint32_t)(nBuckets * ((p.centroid[dim] - centroidBounds.GetMin()[dim]) /
            (centroidBounds.GetMax()[dim] - centroidBounds.GetMin()[dim])));
        if (b == nBuckets) b = nBuckets-1;
        assert(b >= 0 && b < nBuckets);
        return b <= splitBucket;
    }

    int splitBucket, nBuckets, dim;
    const Range3f &centroidBounds;
};

enum SplitStrategy
{
    SPLIT_MIDDLE,
    SPLIT_EQUAL_COUNTS,
    SPLIT_SAH
};

#define MAX_PRIMS_IN_NODE 4

template<typename T>
BVHBuildNode* RecursiveBuildBVH(SplitStrategy splitStrategy, vector<BVHItem>::iterator start, vector<BVHItem>::iterator end, uint64_t *totalNodes, 
    const vector<T>& primitives, vector<T>& orderedPrims)
{
    assert(start < end);
    if (start >= end)
        return NULL;

    (*totalNodes)++;
    BVHBuildNode *node = new BVHBuildNode();
    // Compute bounds of all primitives in BVH node
    Range3f bbox = Range3f::Empty();
    for (vector<BVHItem>::iterator i = start; i != end; ++i)
        bbox.Grow(i->bbox);

    uint32_t nPrimitives = (uint32_t)(end - start);
    if (nPrimitives == 1)
    {
        // Create leaf _BVHBuildNode_
        uint32_t firstPrimOffset = (uint32_t)orderedPrims.size();
        for (vector<BVHItem>::iterator i = start; i != end; ++i)
        {
            uint64_t idx = i->idx;
            orderedPrims.push_back(primitives[idx]);
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
        return node;
    }
    else
    {
        // Compute bound of primitive centroids, choose split dimension _dim_
        Range3f centroidBBox = Range3f::Empty();
        for (vector<BVHItem>::iterator i = start; i != end; ++i)
            centroidBBox.Grow(i->centroid);
        int dim = centroidBBox.GetSize().MaxComponentIndex();

        // Partition primitives into two sets and build children
		vector<BVHItem>::iterator mid = start + (end - start) / 2;
        if (centroidBBox.GetMax()[dim] == centroidBBox.GetMin()[dim]) 
        {
            // Create leaf _BVHBuildNode_
            uint32_t firstPrimOffset = (uint32_t)orderedPrims.size();
            for (vector<BVHItem>::iterator i = start; i != end; ++i) 
            {
                uint32_t primNum = i->idx;
                orderedPrims.push_back(primitives[primNum]);
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
            return node;
        }

        // Partition primitives based on _splitMethod_
        switch (splitStrategy)
        {
        case SPLIT_MIDDLE:
            {
                // Partition primitives through node's midpoint
                float pmid = 0.5f * (centroidBBox.GetMin()[dim] + centroidBBox.GetMax()[dim]);
				mid = std::partition(start, end, 
					[dim, pmid](const BVHItem &a) -> bool { return a.centroid[dim] < pmid; });
            }
            break;
        case SPLIT_EQUAL_COUNTS:
            {
                // Partition primitives into equally-sized subsets
                mid = start + (end - start) / 2;
                std::nth_element(start, mid, end, ComparePoints(dim));
            }
            break;
        case SPLIT_SAH: default:
            {
                // Partition primitives using approximate SAH
                if (nPrimitives <= 4) 
                {
                    // Partition primitives into equally-sized subsets
                    mid = start + (end - start) / 2;
                    std::nth_element(start, mid, end, ComparePoints(dim));
                }
                else 
                {
                    // Allocate _BucketInfo_ for SAH partition buckets
                    const int nBuckets = 12;
                    struct BucketInfo 
                    {
                        BucketInfo() : bounds(Range3f::Empty()) { count = 0; }
                        int count;
                        Range3f bounds;
                    };
                    BucketInfo buckets[nBuckets];

                    // Initialize _BucketInfo_ for SAH partition buckets
                    for (vector<BVHItem>::iterator i = start; i != end; ++i) 
                    {
                        int b = (uint32_t)(nBuckets *
                            ((i->centroid[dim] - centroidBBox.GetMin()[dim]) /
                            (centroidBBox.GetMax()[dim] - centroidBBox.GetMin()[dim])));
                        if (b == nBuckets) b = nBuckets-1;
                        assert(b >= 0 && b < nBuckets);
                        buckets[b].count++;
                        buckets[b].bounds.Grow(i->bbox);
                    }

                    // Compute costs for splitting after each bucket
                    float cost[nBuckets-1];
                    for (int i = 0; i < nBuckets-1; ++i) 
                    {
                        Range3f b0 = Range3f::Empty();
                        Range3f b1 = Range3f::Empty();
                        int count0 = 0, count1 = 0;
                        for (int j = 0; j <= i; ++j) {
                            b0.Grow(buckets[j].bounds);
                            count0 += buckets[j].count;
                        }
                        for (int j = i+1; j < nBuckets; ++j) {
                            b1.Grow(buckets[j].bounds);
                            count1 += buckets[j].count;
                        }
                        cost[i] = .125f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) /
                            bbox.SurfaceArea();
                    }

                    // Find bucket to split at that minimizes SAH metric
                    float minCost = cost[0];
                    uint32_t minCostSplit = 0;
                    for (int i = 1; i < nBuckets-1; ++i) 
                    {
                        if (cost[i] < minCost) 
                        {
                            minCost = cost[i];
                            minCostSplit = i;
                        }
                    }

                    // Either create leaf or split primitives at selected SAH bucket
                    if (nPrimitives > MAX_PRIMS_IN_NODE || minCost < nPrimitives) 
                    {
                            mid = std::partition(start, end,
                                CompareToBucket(minCostSplit, nBuckets, dim, centroidBBox));
                    }

                    else 
                    {
                        // Create leaf _BVHBuildNode_
                        uint32_t firstPrimOffset = (uint32_t)orderedPrims.size();
                        for (vector<BVHItem>::iterator i = start; i != end; ++i) 
                        {
                            uint32_t primNum = i->idx;
                            orderedPrims.push_back(primitives[primNum]);
                        }
                        node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
                    }
                }
            }
            break;
        }
        node->InitInterior(dim,
            RecursiveBuildBVH(splitStrategy, start, mid,
            totalNodes, primitives, orderedPrims),
            RecursiveBuildBVH(splitStrategy, mid, end,
            totalNodes, primitives, orderedPrims));
        return node;
    }
}

uint32_t FlattenBVHTree(BVHBuildNode *node, vector<LinearBVHNode> &nodes, uint32_t *offset);

struct GroupBvh
{
    ~GroupBvh()
    {
        for (uint32_t i = 0; i < groups.size(); i++)
        {
            if (groups[i])
                delete groups[i];
        }
    }
    void                    BuildBVH();
    vector<RayEngine*>      groups;
    Intervalf               interval;
    vector<LinearBVHNode>   bvhNodes;
};

struct TriangleBvh
{
    TriangleBvh(carray<Vec3f> &pos, carray<Vec3f> &norm, carray<Vec2f> &texcoord, carray<Vec3i> &fs)
        : positions(pos), faces(fs), normals(norm), uvs(texcoord) 
    {
        BuildBVH();
    }
    carray<Vec3f>           &positions;
    carray<Vec3i>           &faces;
    carray<Vec2f>           &uvs;
    carray<Vec3f>           &normals;
    vector<uint32_t>        ordered;
    vector<LinearBVHNode>   bvhNodes;
    void                    BuildBVH();
};

struct SphereBvh
{
    SphereBvh() 
    { 
        BuildBVH(); 
    };
    SphereBvh(const Vec3f &pos, float r) : centers(1, pos), radius(1, r) 
    { 
        assert(centers.size() == radius.size()); 
        BuildBVH();
    }
    SphereBvh(const carray<Vec3f> &pos, const carray<float> &r)
        : centers(pos), radius(r) 
    { 
        assert(centers.size() == radius.size()); 
        BuildBVH();
    }
    carray<Vec3f>           centers;
    carray<float>           radius;
    vector<uint32_t>        ordered;
    vector<LinearBVHNode>   bvhNodes;
    void                    BuildBVH();
};

struct SegmentBvh
{
    SegmentBvh(float r, carray<Vec3f> &pos, carray<Vec3f> &tangs, carray<Vec2f> &texcoords, carray<float> &radiusArray, carray<Vec2i> &seg)
        : radius(r), positions(pos), tangents(tangs), uvs(texcoords), radiuss(radiusArray), segments(seg) 
    {
        BuildBVH();
    }
    float radius;
    carray<Vec3f>           &positions;
    carray<Vec3f>           &tangents;
    carray<Vec2f>           &uvs;
    carray<float>           &radiuss;
    carray<Vec2i>           &segments;
    vector<uint32_t>        ordered;
    vector<LinearBVHNode>   bvhNodes;
    void                    BuildBVH();
};


#endif // _RAY_BVH_ENGINE_DATA_H_

