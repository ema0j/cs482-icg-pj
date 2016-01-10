#ifndef rayPrimitiveBVHEngine_h__
#define rayPrimitiveBVHEngine_h__

#include <vmath/range3.h>
#include <ray/rayPrimitiveEngine.h>
#include <ray/rayBvhNode.h>

class RayPrimitiveBVHEngine : public RayPrimitiveEngine 
{
	friend class RayPrimitiveBVHEngineBuilder;
public:
	RayPrimitiveBVHEngine(void);
	~RayPrimitiveBVHEngine(void);
	void Print(FILE* f);
	virtual void CollectStats(StatsManager& stats);
protected:
	virtual bool Intersect(const Ray& ray, Intersection* intersection);
	virtual bool IntersectAny(const Ray& ray);
private:
	_BvhNode* _root;
	Range3f _cachedBBox;
};

struct _BVHPrimitiveInfo;
class RayPrimitiveBVHEngineBuilder : public RayPrimitiveEngineBuilder {
public:
    enum BvhSplitMethod { SPLIT_MIDDLE, SPLIT_EQUAL_COUNTS };
	RayPrimitiveBVHEngineBuilder(BvhSplitMethod splitMethod = SPLIT_MIDDLE);

protected:
	virtual shared_ptr<RayPrimitiveEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayPrimitiveEngine>(new RayPrimitiveBVHEngine()); }
	virtual void _InitAcceleration(shared_ptr<RayPrimitiveEngine> engine, const Intervalf& time);
private:
	uint32_t	_MaxDimension(const Range3f &box);
	_BvhNode*		_BuildNode(vector<shared_ptr<RayPrimitive> >& prims, 
						 vector<_BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end, Range3f& bbox, int depth);
	_BvhNode*		_MakeLeaf(vector<shared_ptr<RayPrimitive> >& prims, 
						vector<_BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end, Range3f &bbox);
	uint32_t	_FindSplit(vector<_BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end);

	
	BvhSplitMethod		    _splitMethod;
	const uint32_t		_maxPrims;
	const uint32_t		_maxDepth;
	vector<shared_ptr<RayPrimitive> > orderedPrims;
};

#endif // rayPrimitiveBVHEngine_h__

