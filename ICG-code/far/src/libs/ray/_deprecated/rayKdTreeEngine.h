#ifndef _RAYKDTREEENGINE_H_
#define _RAYKDTREEENGINE_H_

#include "rayPrimitiveEngine.h"

#define _KDTREE_MAX_DEPTH 64
#define _KDTREE_MAX_MAILBOXES 1024

struct _KdTreeNode; struct _KdTreeSplit;
class RayKdTreeEngine : public RayPrimitiveEngine {
public:
    RayKdTreeEngine();

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool IntersectAny(const Ray& ray);

	void Print(FILE* f);

    virtual void CollectStats(StatsManager& stats);

    friend class RayKdTreeEngineBuilder;
    friend class RayKdTreeFastEngineBuilder;
    friend class RayKdTreeSAHEngineBuilder;

protected:
    _KdTreeNode* _root;
    Range3f _cachedBBox;

#if (FAR_MULTITHREAD == 0)
    int _currentMailboxId;
#endif
};

struct _KdTreeBoxedPrimitive {
    RayPrimitive* prim;
    Range3f bbox;
    // float area;
};

class RayKdTreeEngineBuilder : public RayPrimitiveEngineBuilder {
public:
    RayKdTreeEngineBuilder();

protected:
    virtual shared_ptr<RayPrimitiveEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayPrimitiveEngine>(new RayKdTreeEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayPrimitiveEngine> engine, const Intervalf& time);

protected:
    int _maxPrims;
    int _maxDepth;
    
    _KdTreeNode* _BuildNode(const Range3f& bbox, const vector<_KdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
    _KdTreeNode* _BuildInternalNode(const Range3f& bbox, const vector<_KdTreeBoxedPrimitive*>& prims, const _KdTreeSplit& split, const Intervalf& time, int maxDepth, int depth);
    _KdTreeNode* _BuildLeafNode(const vector<_KdTreeBoxedPrimitive*>& prims, int depth);
    _KdTreeSplit _FindSplit(const Range3f& bbox, const vector<_KdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
};

template<class T>
struct _PreallocatedArray {
public:
	_PreallocatedArray(int cc) { c = cc; d = new T[c]; s = 0; }	
	~_PreallocatedArray() { delete d; }
	
	void clear() { s = 0; }
	void resize(int ss) { s = ss; }
	
	int size() const { return s; }
	
	void add(const T& v) { d[s] = v; s ++; }
	void push_back(const T& v) { add(v); }
	T& operator[](int i) const { return d[i]; }
	
protected:
	int s, c;
	T* d;
};

class RayKdTreeFastEngineBuilder : public RayPrimitiveEngineBuilder {
public:
    RayKdTreeFastEngineBuilder();

protected:
    virtual shared_ptr<RayPrimitiveEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayPrimitiveEngine>(new RayKdTreeEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayPrimitiveEngine> engine, const Intervalf& time);

protected:
    int _maxPrims;
    int _maxDepth;
    
    _PreallocatedArray<_KdTreeBoxedPrimitive*>* __minPrims[_KDTREE_MAX_DEPTH];
    _PreallocatedArray<_KdTreeBoxedPrimitive*>* __maxPrims[_KDTREE_MAX_DEPTH];
    
    _KdTreeNode* _BuildNode(const Range3f& bbox, const _PreallocatedArray<_KdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
    _KdTreeNode* _BuildInternalNode(const Range3f& bbox, const _PreallocatedArray<_KdTreeBoxedPrimitive*>& prims, const _KdTreeSplit& split, const Intervalf& time, int maxDepth, int depth);
    _KdTreeNode* _BuildLeafNode(const _PreallocatedArray<_KdTreeBoxedPrimitive*>& prims, int depth);
    _KdTreeSplit _FindSplit(const Range3f& bbox, const _PreallocatedArray<_KdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
};

struct _KdTreeBoxedAreaPrimitive {
    RayPrimitive* prim;
    Range3f bbox;
    float area;
};

class RayKdTreeSAHEngineBuilder : public RayPrimitiveEngineBuilder {
public:
    RayKdTreeSAHEngineBuilder();

protected:
    virtual shared_ptr<RayPrimitiveEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayPrimitiveEngine>(new RayKdTreeEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayPrimitiveEngine> engine, const Intervalf& time);

protected:
    int _maxPrims;
    int _maxDepth;

    float _intersectionCostEstimate;
    float _emptyBonusEstimate;
    float _traversalCostEstimate;

    _KdTreeNode* _BuildNode(const Range3f& bbox, float area, const vector<_KdTreeBoxedAreaPrimitive*>& prims, const Intervalf& time, int depth, int badRefines = 0);
    static float _ComputeAreaEstimate(const vector<shared_ptr<RayPrimitive> >& primitives, vector<float>& areas, const Intervalf& time) {
        float area = 0;
        for(size_t i = 0; i < primitives.size(); i ++) {
            areas[i] = primitives[i]->ComputeAverageArea(time);
            area += areas[i];
        }
        return area;
    }
};

#endif
