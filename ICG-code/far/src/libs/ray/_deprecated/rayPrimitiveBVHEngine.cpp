#include "rayPrimitiveBVHEngine.h"
#include <scene/intersectionMethods.h>

#define _BVH_MAX_DEPTH_ 64
#define _BVH_MAX_PRIMITIVE_IN_NODE_ 8

struct _BVHPrimitiveInfo
{
	_BVHPrimitiveInfo(uint32_t idx, Range3f bbox) : _idx(idx), _bbox(bbox)
	{
		_centroid = bbox.GetCenter();
	}
	uint32_t _idx;
	Range3f		 _bbox;
	Vec3f		 _centroid;
};

RayPrimitiveBVHEngine::RayPrimitiveBVHEngine() : _root(NULL)
{
}


RayPrimitiveBVHEngine::~RayPrimitiveBVHEngine(void)
{
}

void RayPrimitiveBVHEngine::CollectStats(StatsManager& stats)
{
	RayPrimitiveEngine::CollectStats(stats);
	_root->CollectStats(stats, 0);
}

void RayPrimitiveBVHEngine::Print(FILE* f) {
	_root->Print(f, 0);
}

struct _BvhTodo
{
    _BvhNode* node;
    float tMax;
    float tMin;
};


bool RayPrimitiveBVHEngine::Intersect(const Ray& ray, Intersection* isect) 
{
	// find the ray distances at the intersection with the box
	Ray r = ray;
	if(!IntersectBoundingBox(_cachedBBox, r, r.tMin, r.tMax))
	{
		return false;
	}

    float tMin = r.tMin;
    float tMax = r.tMax;

    _BvhTodo todo[_BVH_MAX_DEPTH_];
    uint32_t todoOffset = 0;

    _BvhNode* node = _root;
    RayPrimitive* hitPrimitive = 0;

    while (node) 
    {
        if (node->isLeaf)
        {
            _BvhLeafNode *leafNode = static_cast<_BvhLeafNode*>(node);
            for (uint32_t i = leafNode->start; i < leafNode->end; i++)
            {
                shared_ptr<RayPrimitive> prim = _primitives[i];
                if(prim->Intersect(r, isect)) 
                {
                    hitPrimitive = prim.get();
                    r.tMax = isect->t;
                }
            }
        }
        else
        {
            _BvhInternalNode *interNode = static_cast<_BvhInternalNode*>(node);
            if (IntersectBoundingBox(interNode->leftChild->bbox, r, tMin, tMax))
            {
                todo[todoOffset].node = interNode->leftChild;
                todo[todoOffset].tMax = tMax;
                todo[todoOffset].tMin = tMin;
                todoOffset++;
            }
            if (IntersectBoundingBox(interNode->rightChild->bbox, r, tMin, tMax))
            {
                todo[todoOffset].node = interNode->rightChild;
                todo[todoOffset].tMax = tMax;
                todo[todoOffset].tMin = tMin;
                todoOffset++;
            }
        }

        if(todoOffset > 0) 
        {
            todoOffset--;
            node = todo[todoOffset].node;
            tMax = todo[todoOffset].tMax;
            tMin = todo[todoOffset].tMin;
        } 
        else 
        {
            node = NULL;
        }
    }
	return hitPrimitive != 0;
}

bool RayPrimitiveBVHEngine::IntersectAny(const Ray& ray) 
{
    // find the ray distances at the intersection with the box
    Ray r = ray;
    if(!IntersectBoundingBox(_cachedBBox, r, r.tMin, r.tMax))
    {
        return false;
    }

    float tMin = r.tMin;
    float tMax = r.tMax;

    _BvhTodo todo[_BVH_MAX_DEPTH_];
    uint32_t todoOffset = 0;

    _BvhNode* node = _root;
    while (node) 
    {
        if (node->isLeaf)
        {
            _BvhLeafNode *leafNode = static_cast<_BvhLeafNode*>(node);
            for (uint32_t i = leafNode->start; i < leafNode->end; i++)
            {
                shared_ptr<RayPrimitive> prim = _primitives[i];
                if(prim->Intersect(r)) 
                {
                    return true;
                }
            }
        }
        else
        {
            _BvhInternalNode *interNode = static_cast<_BvhInternalNode*>(node);
            if (IntersectBoundingBox(interNode->leftChild->bbox, r, tMin, tMax))
            {
                todo[todoOffset].node = interNode->leftChild;
                todo[todoOffset].tMax = tMax;
                todo[todoOffset].tMin = tMin;
                todoOffset++;
            }
            if (IntersectBoundingBox(interNode->rightChild->bbox, r, tMin, tMax))
            {
                todo[todoOffset].node = interNode->rightChild;
                todo[todoOffset].tMax = tMax;
                todo[todoOffset].tMin = tMin;
                todoOffset++;
            }
        }
        if(todoOffset > 0) 
        {
            todoOffset--;
            node = todo[todoOffset].node;
            tMax = todo[todoOffset].tMax;
            tMin = todo[todoOffset].tMin;
        } 
        else 
        {
            node = NULL;
        }
    }
    return false;
}


RayPrimitiveBVHEngineBuilder::RayPrimitiveBVHEngineBuilder(BvhSplitMethod splitMethod) 
    : _maxDepth(_BVH_MAX_DEPTH_), _maxPrims(_BVH_MAX_PRIMITIVE_IN_NODE_), _splitMethod(splitMethod)
{
}

void RayPrimitiveBVHEngineBuilder::_InitAcceleration(shared_ptr<RayPrimitiveEngine> e, const Intervalf& time)
{
	shared_ptr<RayPrimitiveBVHEngine> kde = dynamic_pointer_cast<RayPrimitiveBVHEngine>(e);

	vector<Range3f> bboxes(kde->_primitives.size()); 
	bboxes.resize(kde->_primitives.size());

	Range3f bbox = _ComputeBoundingBox(kde->_primitives, bboxes, time);
	orderedPrims.clear();
	orderedPrims.reserve(kde->_primitives.size());

	//if(_maxDepth <= 0) {
	//	_maxDepth = (int)(8 + 1.3f * log((float)boxedPrims.size()));
	//	// _maxDepth = (int)(8 + 2 * log((float)boxedPrims.size()));
	//	// printf("estimated maxdepth: %d\n", _maxDepth);
	//}

	vector<_BVHPrimitiveInfo> buildData;
	buildData.reserve(kde->_primitives.size());
	for (uint32_t i = 0; i < kde->_primitives.size(); ++i) {
		buildData.push_back(_BVHPrimitiveInfo(i, bboxes[i]));
	}

	kde->_root = _BuildNode(kde->_primitives, buildData, 0, kde->_primitives.size(), bbox, 0);
	kde->_primitives = orderedPrims;
	kde->_cachedBBox = bbox;
}

uint32_t RayPrimitiveBVHEngineBuilder::_MaxDimension(const Range3f &bbox)
{
	Vec3f size = bbox.GetSize();
	if(size[0] > size[1] && size[0] > size[2]) {
		return 0;
	} else if(size[1] > size[2]) {
		return 1;
	} else {
		return 2;
	}
}

_BvhNode* RayPrimitiveBVHEngineBuilder::_BuildNode(vector<shared_ptr<RayPrimitive> >& prims, 
	vector<_BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end, Range3f &bbox, int depth)
{
	if (end - start < _maxPrims || depth == _maxDepth)
		return _MakeLeaf(prims, buildData, start, end, bbox);
	else
	{
		uint32_t dim = 0;
		unsigned mid = _FindSplit(buildData, start, end);

		_BvhInternalNode *node = new _BvhInternalNode();
		Range3f b1, b2;
		node->leftChild = _BuildNode(prims, buildData, start, mid, b1, depth + 1);
		node->rightChild = _BuildNode(prims, buildData, mid, end, b2, depth + 1);

		bbox = Range3f::Union(b1, b2);
		return node;
	}
}

_BvhNode* RayPrimitiveBVHEngineBuilder::_MakeLeaf(vector<shared_ptr<RayPrimitive> >& prims, 
	vector<_BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end, Range3f &bbox)
{
	_BvhLeafNode *leafNode = new _BvhLeafNode();
	assert(start >= 0 && end <= prims.size() && start <= end);
	leafNode->start = orderedPrims.size();
	bbox = Range3f::INVALID;
	for (uint32_t i = start; i < end; i++)
	{
		orderedPrims.push_back(prims[buildData[i]._idx]);
		bbox.Grow(buildData[i]._bbox);
	}
	leafNode->bbox = bbox;
	leafNode->end = orderedPrims.size();
	return leafNode;
}

struct CompareToMid {
	CompareToMid(int d, float m) { dim = d; mid = m; }
	int dim;
	float mid;
	bool operator()(const _BVHPrimitiveInfo &a) const {
		return a._centroid[dim] < mid;
	}
};

struct ComparePoints {
	ComparePoints(int d) { dim = d; }
	int dim;
	bool operator()(const _BVHPrimitiveInfo &a,
		const _BVHPrimitiveInfo &b) const {
			return a._centroid[dim] < b._centroid[dim];
	}
};


uint32_t RayPrimitiveBVHEngineBuilder::_FindSplit(vector<_BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end)
{
	Range3f cBox = Range3f::INVALID;
	for (uint32_t i = start; i < end; i++)
	{
		cBox.Grow(buildData[i]._centroid);
	}
	uint32_t dim = _MaxDimension(cBox);

	uint32_t mid = -1;
	switch(_splitMethod)
	{
	case SPLIT_MIDDLE:
		{
			float pmid = cBox.GetCenter()[dim];
			_BVHPrimitiveInfo *midPtr = std::partition(&buildData[start],
				&buildData[end-1]+1,
				CompareToMid(dim, pmid));
			mid = midPtr - &buildData[0];
			break;
		}
	case SPLIT_EQUAL_COUNTS:
		{
			mid = (start + end) / 2;
			std::nth_element(&buildData[start], &buildData[mid],
				&buildData[end-1]+1, ComparePoints(dim));
			break;
		}
	}
	assert(mid != -1);
	return mid;
}