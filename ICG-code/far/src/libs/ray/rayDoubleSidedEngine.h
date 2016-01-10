
#ifndef _RAY_DOUBLDE_SIDED_ENDING_H_
#define _RAY_DOUBLDE_SIDED_ENDING_H_

#include "rayEngine.h"
class rayDoubleSidedEngine : public RayEngine
{
public:
    rayDoubleSidedEngine(shared_ptr<RayEngine> engine) : _engine(engine) {}
    virtual ~rayDoubleSidedEngine(void) {}

    virtual Intervalf ValidInterval() { return _engine->ValidInterval(); }

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool IntersectAny(const Ray& ray) { return _engine->IntersectAny(ray); }

    virtual void CollectStats(StatsManager& stats) { _engine->CollectStats(stats); }

    // these are very slow and should be cached by the caller
    // assume to be valid within the valid interval
    // average area as average over the interval
    virtual Range3f ComputeBoundingBox() { return _engine->ComputeBoundingBox(); }
    virtual float ComputeAverageArea() { return _engine->ComputeAverageArea(); }
private:
    shared_ptr<RayEngine> _engine;
};
#endif // _RAY_DOUBLDE_SIDED_ENDING_H_

