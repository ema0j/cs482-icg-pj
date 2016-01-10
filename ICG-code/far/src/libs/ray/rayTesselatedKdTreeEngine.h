#ifndef _RAYTESSELATEDKDTREEENGINE_H_
#define _RAYTESSELATEDKDTREEENGINE_H_

#include "rayTesselatedEngine.h"

#define _KDTREE_MAX_DEPTH 64
#define _KDTREE_MAX_MAILBOXES 1024

struct _TesselatedKdTreeNode; struct _TesselatedKdTreeSplit;
class RayTesselatedKdTreeEngine : public RayTesselatedEngine {
public:
    RayTesselatedKdTreeEngine();

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool IntersectAny(const Ray& ray);

	void Print(FILE* f);

    virtual void CollectStats(StatsManager& stats);

    friend class RayTesselatedKdTreeEngineBuilder;
    friend class RayTesselatedKdTreeFastEngineBuilder;
    friend class RayTesselatedKdTreeSAHEngineBuilder;

protected:
	_TesselatedKdTreeNode* _root;
	Range3f _cachedBBox;

#if (FAR_MULTITHREAD == 0)
    int _currentMailboxId;
#endif
};

struct _TesselatedKdTreeBoxedPrimitive {
    RayPrimitive* prim;
    Range3f bbox;
    // float area;
};

class RayTesselatedKdTreeEngineBuilder : public RayTesselatedEngineBuilder {
public:
    RayTesselatedKdTreeEngineBuilder();

protected:
    virtual shared_ptr<RayTesselatedEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayTesselatedEngine>(new RayTesselatedKdTreeEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayTesselatedEngine> engine, const Intervalf& time);

protected:
    int _maxPrims;
    int _maxDepth;
    
    _TesselatedKdTreeNode* _BuildNode(const Range3f& bbox, const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
    _TesselatedKdTreeNode* _BuildInternalNode(const Range3f& bbox, const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, const _TesselatedKdTreeSplit& split, const Intervalf& time, int maxDepth, int depth);
    _TesselatedKdTreeNode* _BuildLeafNode(const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, int depth);
    _TesselatedKdTreeSplit _FindSplit(const Range3f& bbox, const vector<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
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

class RayTesselatedKdTreeFastEngineBuilder : public RayTesselatedEngineBuilder {
public:
    RayTesselatedKdTreeFastEngineBuilder();

protected:
    virtual shared_ptr<RayTesselatedEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayTesselatedEngine>(new RayTesselatedKdTreeEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayTesselatedEngine> engine, const Intervalf& time);

protected:
    int _maxPrims;
    int _maxDepth;
    
    _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>* __minPrims[_KDTREE_MAX_DEPTH];
    _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>* __maxPrims[_KDTREE_MAX_DEPTH];
    
    _TesselatedKdTreeNode* _BuildNode(const Range3f& bbox, const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
    _TesselatedKdTreeNode* _BuildInternalNode(const Range3f& bbox, const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, const _TesselatedKdTreeSplit& split, const Intervalf& time, int maxDepth, int depth);
    _TesselatedKdTreeNode* _BuildLeafNode(const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, int depth);
    _TesselatedKdTreeSplit _FindSplit(const Range3f& bbox, const _PreallocatedArray<_TesselatedKdTreeBoxedPrimitive*>& prims, const Intervalf& time, int maxDepth, int depth);
};

struct _TesselatedKdTreeBoxedAreaPrimitive {
    RayPrimitive* prim;
    Range3f bbox;
    float area;
};

class RayTesselatedKdTreeSAHEngineBuilder : public RayTesselatedEngineBuilder {
public:
    RayTesselatedKdTreeSAHEngineBuilder();

protected:
    virtual shared_ptr<RayTesselatedEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayTesselatedEngine>(new RayTesselatedKdTreeEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayTesselatedEngine> engine, const Intervalf& time);

protected:
    int _maxPrims;
    int _maxDepth;

    float _intersectionCostEstimate;
    float _emptyBonusEstimate;
    float _traversalCostEstimate;

    _TesselatedKdTreeNode* _BuildNode(const Range3f& bbox, float area, const vector<_TesselatedKdTreeBoxedAreaPrimitive*>& prims, const Intervalf& time, int depth, int badRefines = 0);
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
