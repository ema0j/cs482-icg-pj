#include "rayTesselatedKdTreeEngine.h"
#include <scene/intersectionMethods.h>
#include <misc/stats.h>
using std::sort;

enum _KdSplitType { _KD_X = 0, _KD_Y = 1, _KD_Z = 2, _KD_LEAF = 3 };
struct _TesselatedKdTreeSplit { _KdSplitType type; float split; };

struct _TesselatedKdTreeNode {
    _KdSplitType type;
    virtual ~_TesselatedKdTreeNode() { }
    bool IsLeaf() { return type == _KD_LEAF; }
	virtual void Print(FILE* f, int depth) = 0;
    virtual void CollectStats(StatsManager& stats, int depth) = 0;
};

struct _TesselatedKdTreeInternalNode : public _TesselatedKdTreeNode {
    _TesselatedKdTreeNode* minChild;
    _TesselatedKdTreeNode* maxChild;
    float split;

    _TesselatedKdTreeInternalNode(_KdSplitType t, float s, _TesselatedKdTreeNode* m, _TesselatedKdTreeNode* M, int depth) {
        type = t;
        split = s;
        minChild = m;
        maxChild = M;
    }

    virtual ~_TesselatedKdTreeInternalNode() { if(minChild) delete minChild; if(maxChild) delete maxChild; }

	virtual void Print(FILE* f, int depth) {
		for(int i = 0; i < depth*2; i ++) fputc(' ', f);
		fprintf(f, "internal %d %f\n", type, split);
		minChild->Print(f, depth+1 );
		maxChild->Print(f, depth+1 );
	}

    virtual void CollectStats(StatsManager& stats, int depth) {
        StatsCounterVariable* internalNodes = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Kd Internal Nodes");
        StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Kd Memory");
        internalNodes->Increment();
        memory->Increment(sizeof(*this));
        if(minChild) minChild->CollectStats(stats, depth+1);
        if(maxChild) maxChild->CollectStats(stats, depth+1);
    }
};

struct _TesselatedKdTreeLeafNode : public _TesselatedKdTreeNode {
    carray<RayPrimitive*> prims;

    _TesselatedKdTreeLeafNode(const vector<_TesselatedKdTreeBoxedPrimitive*>& p, int depth) {
        type = _KD_LEAF;
        prims.resize((uint32_t)p.size());
        for(uint32_t i = 0; i < prims.size(); i ++) {
            prims[i] = p[i]->prim;
        }
    }

    _TesselatedKdTreeLeafNode(const vector<_TesselatedKdTreeBoxedAreaPrimitive*>& p, int depth) {
        type = _KD_LEAF;
        prims.resize((uint32_t)p.size());
        for(uint32_t i = 0; i < prims.size(); i ++) {
            prims[i] = p[i]->prim;
        }
    }

    _TesselatedKdTreeLeafNode(const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& p, int depth) {
        type = _KD_LEAF;
        prims.resize((uint32_t)p.size());
        for(uint32_t i = 0; i < prims.size(); i ++) {
            prims[i] = p[i]->prim;
        }
    }

    void Print(FILE* f, int depth) {
		for(int i = 0; i < depth*2; i ++) fputc(' ', f);
		fprintf(f, "leaf with %d\n", prims.size());
	}

    virtual void CollectStats(StatsManager& stats, int depth) {
        StatsCounterVariable* leafNodes = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Kd Leaf Nodes");
        StatsDistributionVariable* leafPrims = stats.GetVariable<StatsDistributionVariable>("Ray Engine", "Kd Leaf Prims");
        StatsDistributionVariable* leafDepth = stats.GetVariable<StatsDistributionVariable>("Ray Engine", "Kd Leaf Depth");
        StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Ray Engine", "Kd Memory");
        leafNodes->Increment();
        leafPrims->Value(prims.size());
        leafDepth->Value(depth);
        memory->Increment(static_cast<double>(sizeof(*this) + prims.size()*sizeof(RayPrimitive*)));
    }
};

class _KdTodo {
public:
    float tMin;
    float tMax;
    _TesselatedKdTreeNode* node;
};

RayTesselatedKdTreeEngine::RayTesselatedKdTreeEngine() {
    _root = 0;
#if (FAR_MULTITHREAD == 0)
    _currentMailboxId = 0;
#endif
    _cachedBBox = Range3f();
}

inline float _GetCoord(const Vec3f& v, const _KdSplitType& split) {
    if(split == _KD_X) return v[0];
    if(split == _KD_Y) return v[1];
    if(split == _KD_Z) return v[2];
    return 0;
}

void RayTesselatedKdTreeEngine::Print(FILE* f) {
	_root->Print(f, 0);
}

void RayTesselatedKdTreeEngine::CollectStats(StatsManager& stats) {
    RayTesselatedEngine::CollectStats(stats);
    _root->CollectStats(stats, 0);
}


bool RayTesselatedKdTreeEngine::Intersect(const Ray& ray, Intersection* intersection) {
#if (FAR_MULTITHREAD == 0)
    static StatsCounterVariable* lookups = StatsManager::GetVariable<StatsCounterVariable>("Ray", "Kd Lookups");
    lookups->Increment();
#endif

    // find the ray distances at the intersection with the box
    Ray r = ray;
    if(!IntersectBoundingBox(_cachedBBox, r, r.tMin, r.tMax)) {
        return false;
    }

    float tMin = r.tMin;
    float tMax = r.tMax;

    // init ray marching params
    Vec3f traversalRayOrigin = ray.E;
    Vec3f traversalRayDirection = ray.D;

    // create a node stack
    _KdTodo traversalStack[_KDTREE_MAX_DEPTH];
    int todoPos = 0;
#if (FAR_MULTITHREAD == 0)
    _currentMailboxId++;
#else
	RayPrimitive* mailboxes[_KDTREE_MAX_MAILBOXES];
	for(int i = 0; i < _KDTREE_MAX_MAILBOXES; i ++) { mailboxes[i] = 0; }
#endif

    // start walking the nodes
    _TesselatedKdTreeNode* node = _root;
    RayPrimitive* hitPrimitive = 0;
    while(node && r.tMax >= tMin) {
        if(node->IsLeaf()) {
            // grab primitives
            int nprims = (int32_t)((_TesselatedKdTreeLeafNode*)node)->prims.size();
            RayPrimitive** prims = ((_TesselatedKdTreeLeafNode*)node)->prims.data();

            // process leaf
            for(int i = 0; i < nprims; i++) {
#if (FAR_MULTITHREAD == 0)
                if(prims[i]->lastRayId != _currentMailboxId) {
                    prims[i]->lastRayId = _currentMailboxId;
#else
				intptr_t primMailboxId = (intptr_t)(prims[i]) % _KDTREE_MAX_MAILBOXES;
                if(mailboxes[primMailboxId] != prims[i]) {
                    mailboxes[primMailboxId] = prims[i];
#endif
                    if(prims[i]->Intersect(r, intersection)) {
                        hitPrimitive = prims[i];
                        r.tMax = intersection->t;
                    }
                }
            }
            if(todoPos > 0) {
                todoPos--;
                node = traversalStack[todoPos].node;
                tMin = traversalStack[todoPos].tMin;
                tMax = traversalStack[todoPos].tMax;
            } else {
                node = 0;
            }
        } else {
            _TesselatedKdTreeInternalNode* intNode = (_TesselatedKdTreeInternalNode*)node;
            float rayOriginAxis = traversalRayOrigin[(int)intNode->type];
            float rayDirectionAxis = traversalRayDirection[(int)intNode->type];
            float startPosAxis = rayOriginAxis + tMin * rayDirectionAxis;
            if(startPosAxis < intNode->split) {
                node = intNode->minChild;
                //if(rayDirectionAxis >= 0) {  //causes a divide by zero error where (tPlane <= tMax) returns true
                if(rayDirectionAxis > 0) {
                    float tPlane = (intNode->split - rayOriginAxis) / rayDirectionAxis;
                    if(tPlane <= tMax) {
                        traversalStack[todoPos].node = intNode->maxChild;
                        traversalStack[todoPos].tMin = tPlane;
                        traversalStack[todoPos].tMax = tMax;
                        todoPos++;
                        tMax = tPlane;
                    }
                }
            } else {
                node = intNode->maxChild;
                //if(rayDirectionAxis <= 0) {  //causes a divide by zero error where (tPlane <= tMax) returns true
                if(rayDirectionAxis < 0) {
                    float tPlane = (intNode->split - rayOriginAxis) / rayDirectionAxis;
                    if(tPlane <= tMax) {
                        traversalStack[todoPos].node = intNode->minChild;
                        traversalStack[todoPos].tMin = tPlane;
                        traversalStack[todoPos].tMax = tMax;
                        todoPos++;
                        tMax = tPlane;
                    }
                }
            }
        }
        assert(todoPos < _KDTREE_MAX_DEPTH);
    }
    return hitPrimitive != 0;
}

bool RayTesselatedKdTreeEngine::IntersectAny(const Ray& ray) {
#if (FAR_MULTITHREAD == 0)
    static StatsCounterVariable* lookups = StatsManager::GetVariable<StatsCounterVariable>("Ray", "Kd Shadow Lookups");
    lookups->Increment();
#endif

    // find the ray distances at the intersection with the box
    Ray r = ray;
    if(!IntersectBoundingBox(_cachedBBox, r, r.tMin, r.tMax)) {
        return false;
    }
    float tMin = r.tMin;
    float tMax = r.tMax;

    // init ray marching params
    Vec3f traversalRayOrigin = ray.E;
    Vec3f traversalRayDirection = ray.D;

    // create a node stack
    _KdTodo traversalStack[_KDTREE_MAX_DEPTH];
    int todoPos = 0;
#if (FAR_MULTITHREAD == 0)
    _currentMailboxId++;
#else
	RayPrimitive* mailboxes[_KDTREE_MAX_MAILBOXES];
	for(int i = 0; i < _KDTREE_MAX_MAILBOXES; i ++) { mailboxes[i] = 0; }
#endif

    // start walking the nodes
    _TesselatedKdTreeNode* node = _root;
    while(node && r.tMax >= tMin) {
        if(node->IsLeaf()) {
            // grab primitives
            int nprims = (int)((_TesselatedKdTreeLeafNode*)node)->prims.size();
            RayPrimitive** prims = ((_TesselatedKdTreeLeafNode*)node)->prims.data();

            // process leaf
            for(int i = 0; i < nprims; i++) {
#if (FAR_MULTITHREAD == 0)
                if(prims[i]->lastRayId != _currentMailboxId) {
                    prims[i]->lastRayId = _currentMailboxId;
#else
				intptr_t primMailboxId = (intptr_t)(prims[i]) % _KDTREE_MAX_MAILBOXES;
                if(mailboxes[primMailboxId] != prims[i]) {
                    mailboxes[primMailboxId] = prims[i];
#endif
                    if(prims[i]->Intersect(r)) {
                        return true;
                    }
                }
            }
            if(todoPos > 0) {
                todoPos--;
                node = traversalStack[todoPos].node;
                tMin = traversalStack[todoPos].tMin;
                tMax = traversalStack[todoPos].tMax;
            } else {
                node = 0;
            }
        } else {
            _TesselatedKdTreeInternalNode* intNode = (_TesselatedKdTreeInternalNode*)node;
            float rayOriginAxis = traversalRayOrigin[(int)intNode->type];
            float rayDirectionAxis = traversalRayDirection[(int)intNode->type];
            float startPosAxis = rayOriginAxis + tMin * rayDirectionAxis;
            if(startPosAxis < intNode->split) {
                node = intNode->minChild;
                //if(rayDirectionAxis >= 0) {  //causes a divide by zero error where (tPlane <= tMax) returns true
                if(rayDirectionAxis > 0) {
                    float tPlane = (intNode->split - rayOriginAxis) / rayDirectionAxis;
                    if(tPlane <= tMax) {
                        traversalStack[todoPos].node = intNode->maxChild;
                        traversalStack[todoPos].tMin = tPlane;
                        traversalStack[todoPos].tMax = tMax;
                        todoPos++;
                        tMax = tPlane;
                    }
                }
            } else {
                node = intNode->maxChild;
                //if(rayDirectionAxis <= 0) {  //causes a divide by zero error where (tPlane <= tMax) returns true
                if(rayDirectionAxis < 0) {
                    float tPlane = (intNode->split - rayOriginAxis) / rayDirectionAxis;
                    if(tPlane <= tMax) {
                        traversalStack[todoPos].node = intNode->minChild;
                        traversalStack[todoPos].tMin = tPlane;
                        traversalStack[todoPos].tMax = tMax;
                        todoPos++;
                        tMax = tPlane;
                    }
                }
            }
        }
    }
    return false;
}

RayTesselatedKdTreeEngineBuilder::RayTesselatedKdTreeEngineBuilder() {
    _maxPrims = 5;
    _maxDepth = -1;
}

void RayTesselatedKdTreeEngineBuilder::_InitAcceleration(shared_ptr<RayTesselatedEngine> e, const Intervalf& time) {
    shared_ptr<RayTesselatedKdTreeEngine> kde = dynamic_pointer_cast<RayTesselatedKdTreeEngine>(e);

    vector<Range3f> bboxes(kde->_primitives.size()); bboxes.resize(kde->_primitives.size());
    Range3f bbox = _ComputeBoundingBox(kde->_primitives, bboxes, time);
    vector<_TesselatedKdTreeBoxedPrimitive*> boxedPrims = vector<_TesselatedKdTreeBoxedPrimitive*>(kde->_primitives.size());
    boxedPrims.resize(kde->_primitives.size());
    for(int i = 0; i < (int)kde->_primitives.size(); i ++) {
        _TesselatedKdTreeBoxedPrimitive* prim = new _TesselatedKdTreeBoxedPrimitive();
        prim->prim = kde->_primitives[i].get();
        prim->bbox = bboxes[i];
        boxedPrims[i] = prim;
    }

    if(_maxDepth <= 0) {
        _maxDepth = (int)(8 + 1.3f * log((float)boxedPrims.size()));
        // _maxDepth = (int)(8 + 2 * log((float)boxedPrims.size()));
        // printf("estimated maxdepth: %d\n", _maxDepth);
    }
    
    kde->_root = _BuildNode(bbox, boxedPrims, time, _maxDepth, 0);
    kde->_cachedBBox = bbox;
    
    for(int i = 0; i < (int)kde->_primitives.size(); i ++) {
        delete boxedPrims[i];
    }
}

_TesselatedKdTreeNode* RayTesselatedKdTreeEngineBuilder::_BuildNode(const Range3f& bbox, const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth) {
    // check termination criteria
    if(maxDepth == 0 || (int)prims.size() < _maxPrims) { // check max depth and primitive count
        return _BuildLeafNode(prims, depth);
    } else {
        _TesselatedKdTreeSplit split = _FindSplit(bbox, prims, time, maxDepth, depth);
        return _BuildInternalNode(bbox, prims, split, time, maxDepth, depth);
    }
}

_TesselatedKdTreeSplit RayTesselatedKdTreeEngineBuilder::_FindSplit(const Range3f& bbox, const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth) {
    _TesselatedKdTreeSplit split;
    // find largest axis
    Vec3f size = bbox.GetSize();
    if(size[0] > size[1] && size[0] > size[2]) {
        split.type = _KD_X;
        split.split = bbox.GetCenter()[0];
    } else if(size[1] > size[2]) {
        split.type = _KD_Y;
        split.split = bbox.GetCenter()[1];
    } else {
        split.type = _KD_Z;
        split.split = bbox.GetCenter()[2];
    }
    return split;
}

inline void _SplitBox(const Range3f& bbox, const _TesselatedKdTreeSplit& split, Range3f& minBox, Range3f& maxBox) {
    minBox = bbox;
    maxBox = bbox;
    switch(split.type) {
        case _KD_X:
            minBox.GetMaxRef()[0] = split.split;
            maxBox.GetMinRef()[0] = split.split;
            break;
        case _KD_Y:
            minBox.GetMaxRef()[1] = split.split;
            maxBox.GetMinRef()[1] = split.split;
            break;
        case _KD_Z:
            minBox.GetMaxRef()[2] = split.split;
            maxBox.GetMinRef()[2] = split.split;
            break;
        case _KD_LEAF:
        	break;
    }
}

_TesselatedKdTreeNode* RayTesselatedKdTreeEngineBuilder::_BuildInternalNode(const Range3f& bbox, const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, const _TesselatedKdTreeSplit& split, const Intervalf& time, int maxDepth, int depth) {
    // split box
    Range3f minBox, maxBox;
    _SplitBox(bbox, split, minBox, maxBox);

    // split primitives into groups
    vector<_TesselatedKdTreeBoxedPrimitive*> minList, maxList;
    minList.reserve(prims.size()); maxList.reserve(prims.size());
    for(int i = 0; i < (int)prims.size(); i++) {
        _TesselatedKdTreeBoxedPrimitive* prim = prims[i];
        if(Range3f::Overlap(prim->bbox, minBox)) {
            minList.push_back(prim);
        }
        if(Range3f::Overlap(prim->bbox, maxBox)) {
            maxList.push_back(prim);
        }
    }

    _TesselatedKdTreeNode* node;
    if(minList.size() == prims.size() && maxList.size() == prims.size()) {
        node = new _TesselatedKdTreeLeafNode(prims,depth);
    } else {
        _TesselatedKdTreeNode *minChild, *maxChild;
        if(minList.size() > 0) {
            minChild = _BuildNode(minBox, minList, time, maxDepth - 1,depth+1);
        } else {
            minChild = _BuildLeafNode(minList,depth+1);
        }
        if(maxList.size() > 0) {
            maxChild = _BuildNode(maxBox, maxList, time, maxDepth - 1,depth+1);
        } else {
            maxChild = _BuildLeafNode(maxList,depth+1);
        }
        _TesselatedKdTreeInternalNode* splitnode = new _TesselatedKdTreeInternalNode(split.type, split.split, minChild, maxChild,depth);
        node = splitnode;
    }

    return node;
}

_TesselatedKdTreeNode* RayTesselatedKdTreeEngineBuilder::_BuildLeafNode(const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, int depth) {
    return new _TesselatedKdTreeLeafNode(prims, depth);
}

RayTesselatedKdTreeFastEngineBuilder::RayTesselatedKdTreeFastEngineBuilder() {
    _maxPrims = 5;
    _maxDepth = -1;
}

void RayTesselatedKdTreeFastEngineBuilder::_InitAcceleration(shared_ptr<RayTesselatedEngine> e, const Intervalf& time) {
    shared_ptr<RayTesselatedKdTreeEngine> kde = dynamic_pointer_cast<RayTesselatedKdTreeEngine>(e);

    vector<Range3f> bboxes(kde->_primitives.size()); bboxes.resize(kde->_primitives.size());
    Range3f bbox = _ComputeBoundingBox(kde->_primitives, bboxes, time);
    _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*> boxedPrims = _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>((int)kde->_primitives.size());
    boxedPrims.resize((int)kde->_primitives.size());
    for(int i = 0; i < (int)kde->_primitives.size(); i ++) {
        _TesselatedKdTreeBoxedPrimitive* prim = new _TesselatedKdTreeBoxedPrimitive();
        prim->prim = kde->_primitives[i].get();
        prim->bbox = bboxes[i];
        boxedPrims[i] = prim;
    }

    if(_maxDepth <= 0) {
        _maxDepth = (int)(8 + 1.3f * log((float)boxedPrims.size()));
        // _maxDepth = (int)(8 + 2 * log((float)boxedPrims.size()));
        // printf("estimated maxdepth: %d\n", _maxDepth);
    }
    
    for(int i = 0; i < _maxDepth; i ++) {
		__minPrims[i] = new _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>(boxedPrims.size());
		__maxPrims[i] = new _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>(boxedPrims.size());
    }
    
    kde->_root = _BuildNode(bbox, boxedPrims, time, _maxDepth, 0);
    kde->_cachedBBox = bbox;
    
    for(int i = 0; i < _maxDepth; i ++) {
		delete __minPrims[i];
		delete __maxPrims[i];
    }
    
    for(int i = 0; i < (int)kde->_primitives.size(); i ++) {
        delete boxedPrims[i];
    }
}

_TesselatedKdTreeNode* RayTesselatedKdTreeFastEngineBuilder::_BuildNode(const Range3f& bbox, const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth) {
    // check termination criteria
    if(maxDepth == 0 || (int)prims.size() < _maxPrims) { // check max depth and primitive count
        return _BuildLeafNode(prims, depth);
    } else {
        _TesselatedKdTreeSplit split = _FindSplit(bbox, prims, time, maxDepth, depth);
        return _BuildInternalNode(bbox, prims, split, time, maxDepth, depth);
    }
}

_TesselatedKdTreeSplit RayTesselatedKdTreeFastEngineBuilder::_FindSplit(const Range3f& bbox, const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth) {
    _TesselatedKdTreeSplit split;
    // find largest axis
    Vec3f size = bbox.GetSize();
    if(size[0] > size[1] && size[0] > size[2]) {
        split.type = _KD_X;
        split.split = bbox.GetCenter()[0];
    } else if(size[1] > size[2]) {
        split.type = _KD_Y;
        split.split = bbox.GetCenter()[1];
    } else {
        split.type = _KD_Z;
        split.split = bbox.GetCenter()[2];
    }
    return split;
}

_TesselatedKdTreeNode* RayTesselatedKdTreeFastEngineBuilder::_BuildInternalNode(const Range3f& bbox, const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, const _TesselatedKdTreeSplit& split, const Intervalf& time, int maxDepth, int depth) {
    // split box
    Range3f minBox, maxBox;
    _SplitBox(bbox, split, minBox, maxBox);

    // split primitives into groups
    // _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*> minList(prims.size()), maxList(prims.size());
    _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& minList = *__minPrims[depth];
    _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& maxList = *__maxPrims[depth];
    minList.clear(); maxList.clear();    
    for(int i = 0; i < (int)prims.size(); i++) {
        _TesselatedKdTreeBoxedPrimitive* prim = prims[i];
        if(Range3f::Overlap(prim->bbox, minBox)) {
            minList.push_back(prim);
        }
        if(Range3f::Overlap(prim->bbox, maxBox)) {
            maxList.push_back(prim);
        }
    }

    _TesselatedKdTreeNode* node;
    if(minList.size() == prims.size() && maxList.size() == prims.size()) {
        node = new _TesselatedKdTreeLeafNode(prims,depth);
    } else {
        _TesselatedKdTreeNode *minChild, *maxChild;
        if(minList.size() > 0) {
            minChild = _BuildNode(minBox, minList, time, maxDepth - 1,depth+1);
        } else {
            minChild = _BuildLeafNode(minList,depth+1);
        }
        if(maxList.size() > 0) {
            maxChild = _BuildNode(maxBox, maxList, time, maxDepth - 1,depth+1);
        } else {
            maxChild = _BuildLeafNode(maxList,depth+1);
        }
        _TesselatedKdTreeInternalNode* splitnode = new _TesselatedKdTreeInternalNode(split.type, split.split, minChild, maxChild,depth);
        node = splitnode;
    }

    return node;
}

_TesselatedKdTreeNode* RayTesselatedKdTreeFastEngineBuilder::_BuildLeafNode(const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, int depth) {
    return new _TesselatedKdTreeLeafNode(prims, depth);
}

// from pbrt
struct _KdBBoxEdge {
	// BoundEdge Public Methods
	_KdBBoxEdge() { }
	_KdBBoxEdge(float edgePos, _TesselatedKdTreeBoxedAreaPrimitive* p, bool starting) {
		pos = edgePos;
		prim = p;
		type = starting ? BBOXEDGETYPE_MIN : BBOXEDGETYPE_MAX;
	}
	bool operator<(const _KdBBoxEdge &e) const {
		if (pos == e.pos)
			return (int)type < (int)e.type;
		else return pos < e.pos;
	}
	float pos;
	_TesselatedKdTreeBoxedAreaPrimitive* prim;
	enum { BBOXEDGETYPE_MIN = 0, BBOXEDGETYPE_MAX = 1 } type;
};

RayTesselatedKdTreeSAHEngineBuilder::RayTesselatedKdTreeSAHEngineBuilder() {
    _maxPrims = 5;
    _maxDepth = -1;

    _intersectionCostEstimate = 20;
    _traversalCostEstimate = 15;
    _emptyBonusEstimate = 0.2f;

    // _intersectionCostEstimate = 80;
    // _traversalCostEstimate = 1;
    // _emptyBonusEstimate = 0.5f;
}

void RayTesselatedKdTreeSAHEngineBuilder::_InitAcceleration(shared_ptr<RayTesselatedEngine> e, const Intervalf& time) {
    shared_ptr<RayTesselatedKdTreeEngine> kde = dynamic_pointer_cast<RayTesselatedKdTreeEngine>(e);

    vector<Range3f> bboxes(kde->_primitives.size()); bboxes.resize(kde->_primitives.size());
    vector<float> areas(kde->_primitives.size()); areas.resize(kde->_primitives.size());
    Range3f bbox = _ComputeBoundingBox(kde->_primitives, bboxes, time);
    float area = _ComputeAreaEstimate(kde->_primitives, areas, time);
    vector<_TesselatedKdTreeBoxedAreaPrimitive*> boxedPrims = vector<_TesselatedKdTreeBoxedAreaPrimitive*>(kde->_primitives.size());
    boxedPrims.resize(kde->_primitives.size());
    for(int i = 0; i < (int)kde->_primitives.size(); i ++) {
        _TesselatedKdTreeBoxedAreaPrimitive* prim = new _TesselatedKdTreeBoxedAreaPrimitive();
        prim->prim = kde->_primitives[i].get();
        prim->bbox = bboxes[i];
        prim->area = areas[i];
        boxedPrims[i] = prim;
    }

    if(_maxDepth <= 0) {
        // _maxDepth = (int)(8 + 1.3f * log((float)boxedPrims.size()));
        _maxDepth = (int)(8 + 2 * log((float)boxedPrims.size()));
        printf("estimated maxdepth: %d\n", _maxDepth);
    }

    kde->_root = _BuildNode(bbox, area, boxedPrims, time, 0);
    kde->_cachedBBox = bbox;

    for(int i = 0; i < (int)kde->_primitives.size(); i ++) {
        delete boxedPrims[i];
    }
}

// #define _KDSAH_USEAREAESTIMATE

_TesselatedKdTreeNode* RayTesselatedKdTreeSAHEngineBuilder::_BuildNode(const Range3f& bbox, float areaEstimate, const vector<_TesselatedKdTreeBoxedAreaPrimitive*>& prims, const Intervalf& time, int depth, int badRefines) {
    // check termination criteria
    if(depth >= _maxDepth || (int)prims.size() <= _maxPrims) { // check max depth and primitive count
        return new _TesselatedKdTreeLeafNode(prims, depth);
    }

    // allocate locals
	_KdBBoxEdge *edges[3];
	for (int i = 0; i < 3; ++i)
		edges[i] = new _KdBBoxEdge[2*prims.size()];

    // build a new node
    _TesselatedKdTreeNode* node = 0;

    // estimate the total surface area
	Vec3f size = bbox.GetSize();
#ifdef _KDSAH_USEAREAESTIMATE
	float totalSA = areaEstimate;
#else
	float totalSA = (2.f * (size[0]*size[1] + size[0]*size[2] + size[1]*size[2]));
#endif

	// Find the split axis and position
    int bestAxis = -1; int bestOffset = -1; float bestCost = FLT_MAX;
    float bestMinArea, bestMaxArea;
    for(int axis = 0; axis < 3; axis ++) {

	    // Initialize edges
        for (int i = 0; i < (int) prims.size(); ++i) {
            _TesselatedKdTreeBoxedAreaPrimitive* prim = prims[i];
		    edges[axis][2*i] = _KdBBoxEdge(prim->bbox.GetMin()[axis], prim, true);
		    edges[axis][2*i+1] = _KdBBoxEdge(prim->bbox.GetMax()[axis], prim, false);
	    }

        // sort edges
        sort(&edges[axis][0], &edges[axis][2*prims.size()]);

	    // Sweep across the edges to compute the cost of all splits
	    int nMin = 0, nMax = (int)prims.size();
#ifdef _KDSAH_USEAREAESTIMATE
        float minSA = 0; float maxSA = totalSA;
#endif
	    for (int i = 0; i < 2*(int)prims.size(); ++i) {
            // passed a MAX edge
#ifdef _KDSAH_USEAREAESTIMATE
            if (edges[axis][i].type == _KdBBoxEdge::BBOXEDGETYPE_MAX) {
                nMax--;
                maxSA -= edges[axis][i].prim->area;
            }
#else
            if (edges[axis][i].type == _KdBBoxEdge::BBOXEDGETYPE_MAX) nMax--;
#endif
		    float edgePos = edges[axis][i].pos;
		    if (edgePos > bbox.GetMin()[axis] &&
			    edgePos < bbox.GetMax()[axis]) {
                // compute SA for min and max lists
#ifndef _KDSAH_USEAREAESTIMATE
			    int otherAxis[3][2] = { {1,2}, {0,2}, {0,1} };
			    int otherAxis0 = otherAxis[axis][0];
			    int otherAxis1 = otherAxis[axis][1];
			    float minSA = 2 * (size[otherAxis0] * size[otherAxis1] +
			             		    (edgePos - bbox.GetMin()[axis]) *
				                    (size[otherAxis0] + size[otherAxis1]));
			    float maxSA = 2 * (size[otherAxis0] * size[otherAxis1] +
								    (bbox.GetMax()[axis] - edgePos) *
								    (size[otherAxis0] + size[otherAxis1]));
#endif
			    // Compute cost for split at _i_th edge
			    float emptyCellBias = (nMax == 0 || nMin == 0) ? (1 - _emptyBonusEstimate) : 1.f;
			    float cost = _traversalCostEstimate + _intersectionCostEstimate *
				    (minSA * nMin / totalSA + maxSA * nMax / totalSA) * emptyCellBias;
			    // Update best split if this is lowest cost so far
			    if (cost < bestCost)  {
				    bestCost = cost;
				    bestAxis = axis;
				    bestOffset = i;
                    bestMinArea = minSA;
                    bestMaxArea = maxSA;
			    }
		    }
#ifdef _KDSAH_USEAREAESTIMATE
            if (edges[axis][i].type == _KdBBoxEdge::BBOXEDGETYPE_MIN) {
                nMin++;
                minSA += edges[axis][i].prim->area;
            }
#else
            if (edges[axis][i].type == _KdBBoxEdge::BBOXEDGETYPE_MIN) nMin++;
#endif
	    }
	    assert(nMin == (int)prims.size() && nMax == 0); // NOBOOK
    }

    // termination criteria
    float totalIntersectionCost = _intersectionCostEstimate * prims.size();
	if (bestCost > totalIntersectionCost) badRefines ++;
	//if ((bestCost > 4.f * totalIntersectionCost) && prims.size() < 16) || badRefines == 3) {
	if (bestCost > totalIntersectionCost || badRefines == 3) {
        node = new _TesselatedKdTreeLeafNode(prims, depth);
    } else {
	    // Classify primitives with respect to split
        vector<_TesselatedKdTreeBoxedAreaPrimitive*> minList, maxList;
	    for (int i = 0; i < bestOffset; ++i)
		    if (edges[bestAxis][i].type == _KdBBoxEdge::BBOXEDGETYPE_MIN)
                minList.push_back(edges[bestAxis][i].prim);
	    for (int i = bestOffset+1; i < 2*(int)prims.size(); ++i)
		    if (edges[bestAxis][i].type == _KdBBoxEdge::BBOXEDGETYPE_MAX)
                maxList.push_back(edges[bestAxis][i].prim);
	    // Recursively initialize children nodes
	    float split = edges[bestAxis][bestOffset].pos;
	    Range3f minBox = bbox, maxBox = bbox;
	    minBox.GetMaxRef()[bestAxis] = maxBox.GetMinRef()[bestAxis] = split;
        _TesselatedKdTreeNode *minChild = _BuildNode(minBox, bestMinArea, minList, time, depth+1, badRefines);
        _TesselatedKdTreeNode *maxChild = _BuildNode(maxBox, bestMaxArea, maxList, time, depth+1, badRefines);
        node = new _TesselatedKdTreeInternalNode((_KdSplitType)bestAxis, split, minChild, maxChild, depth);
    }

	for (int i = 0; i < 3; ++i) delete edges[i];

    return node;
}

/*
_TesselatedKdTreeNode* RayTesselatedKdTreeSAHEngineBuilder::_BuildNode(const Range3f& nodeBounds, vector<_TesselatedKdTreeBoxedPrimitive*> *prims, const Intervalf& time, int depth, int badRefines) {
    // check termination criteria
    if(depth >= _maxDepth || prims.size() <= _maxPrims) { // check max depth and primitive count
        return new _TesselatedKdTreeLeafNode(prims, depth);
    }

    _TesselatedKdTreeNode* node = 0;

	_KdBoundEdge *edges[3];
	for (int i = 0; i < 3; ++i)
		edges[i] = new _KdBoundEdge[2*prims.size()];

	// Choose split axis position for interior node
	int bestAxis = -1, bestOffset = -1;
	float bestCost = FLT_MAX;
    float oldCost = _intersectionCostEstimate * float(prims.size());
	Vec3f d = nodeBounds.GetMax() - nodeBounds.GetMin();
	float totalSA = (2.f * (d[0]*d[1] + d[0]*d[2] + d[1]*d[2]));
	float invTotalSA = 1.f / totalSA;
	// Choose which axis to split along
	int axis;
	if (d[0] > d[1] && d[0] > d[2]) axis = 0;
	else axis = (d[1] > d[2]) ? 1 : 2;
	int retries = 0;
	retrySplit:
	// Initialize edges for _axis_
    for (int i = 0; i < prims.size(); ++i) {
        _TesselatedKdTreeBoxedPrimitive* prim = prims[i];
		edges[axis][2*i] =
		    _KdBoundEdge(prim->bbox.GetMin()[axis], prim, true);
		edges[axis][2*i+1] =
			_KdBoundEdge(prim->bbox.GetMax()[axis], prim, false);
	}
    sort(&edges[axis][0], &edges[axis][2*prims.size()]);
	// Compute cost of all splits for _axis_ to find best
	int nBelow = 0, nAbove = prims.size();
	for (int i = 0; i < 2*prims.size(); ++i) {
        if (edges[axis][i].type == _KdBoundEdge::END) {
            --nAbove;
        }
		float edget = edges[axis][i].t;
		if (edget > nodeBounds.GetMin()[axis] &&
			edget < nodeBounds.GetMax()[axis]) {
			// Compute cost for split at _i_th edge
			int otherAxis[3][2] = { {1,2}, {0,2}, {0,1} };
			int otherAxis0 = otherAxis[axis][0];
			int otherAxis1 = otherAxis[axis][1];
			float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
			             		(edget - nodeBounds.GetMin()[axis]) *
				                (d[otherAxis0] + d[otherAxis1]));
			float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
								(nodeBounds.GetMax()[axis] - edget) *
								(d[otherAxis0] + d[otherAxis1]));
			float pBelow = belowSA * invTotalSA;
			float pAbove = aboveSA * invTotalSA;
			float eb = (nAbove == 0 || nBelow == 0) ? _emptyBonusEstimate : 0.f;
			float cost = _traversalCostEstimate + _intersectionCostEstimate * (1.f - eb) *
				(pBelow * nBelow + pAbove * nAbove);
			// Update best split if this is lowest cost so far
			if (cost < bestCost)  {
				bestCost = cost;
				bestAxis = axis;
				bestOffset = i;
			}
		}
        if (edges[axis][i].type == _KdBoundEdge::START) {
            ++nBelow;
        }
	}
	assert(nBelow == prims.size() && nAbove == 0); // NOBOOK
	// Create leaf if no good splits were found
	if (bestAxis == -1 && retries < 2) {
		++retries;
		axis = (axis+1) % 3;
		goto retrySplit;
	}
	if (bestCost > oldCost) ++badRefines;
	if ((bestCost > 4.f * oldCost && prims.size() < 16) ||
		bestAxis == -1 || badRefines == 3) {
        node = new _TesselatedKdTreeLeafNode(prims, depth);
    } else {
	    // Classify primitives with respect to split
        vector<_TesselatedKdTreeBoxedPrimitive*> minList, maxList;
	    for (int i = 0; i < bestOffset; ++i)
		    if (edges[bestAxis][i].type == _KdBoundEdge::START)
                minList.push_back(edges[bestAxis][i].prim);
	    for (int i = bestOffset+1; i < 2*prims.size(); ++i)
		    if (edges[bestAxis][i].type == _KdBoundEdge::END)
                maxList.push_back(edges[bestAxis][i].prim);
	    // Recursively initialize children nodes
	    float tsplit = edges[bestAxis][bestOffset].t;
        _TesselatedKdTreeSplit split;
        split.type = (_KdSplitType)bestAxis;
        split.split = tsplit;
	    Range3f minBox = nodeBounds, maxBox = nodeBounds;
	    minBox.GetMaxRef()[bestAxis] = maxBox.GetMinRef()[bestAxis] = tsplit;
        _TesselatedKdTreeNode *minChild = _BuildNode(minBox, &minList, time, depth+1, badRefines);
        _TesselatedKdTreeNode *maxChild = _BuildNode(maxBox, &maxList, time, depth+1, badRefines);
        node = new _TesselatedKdTreeInternalNode(split.type, split.split, minChild, maxChild, depth);
    }

	for (int i = 0; i < 3; ++i) delete edges[i];

    return node;
}
*/
