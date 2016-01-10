#ifndef rayBvhNode_h__
#define rayBvhNode_h__
#include <stdio.h>
#include <vmath/range3.h>

class StatsManager;
struct _BvhNode
{
    _BvhNode() : bbox(Range3f::INVALID) {}
    virtual void Print(FILE* f, int depth) = 0;
    virtual void CollectStats(StatsManager& stats, int depth) = 0;
    Range3f		bbox;
    bool		isLeaf;
};

struct _BvhInternalNode : public _BvhNode
{
    _BvhInternalNode() : leftChild(NULL), rightChild(NULL) { isLeaf = false; }
    virtual void Print(FILE* f, int depth);
    virtual void CollectStats(StatsManager& stats, int depth);
    _BvhNode* leftChild;
    _BvhNode* rightChild;
};

struct _BvhLeafNode : public _BvhNode
{
    _BvhLeafNode() { isLeaf = true; }
    virtual void Print(FILE* f, int depth);
    virtual void CollectStats(StatsManager& stats, int depth);
    uint32_t	start;
    uint32_t	end;
};

#endif // rayBvhNode_h__
