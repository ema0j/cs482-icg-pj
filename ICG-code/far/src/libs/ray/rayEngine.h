#ifndef _RAYENGINE_H_
#define _RAYENGINE_H_

#include <scene/surface.h>
#include <scene/instance.h>
#include <ray/intersection.h>
#include <misc/stats.h>

class RayEngine {
public:
    // valid interval for time vaiations
    virtual Intervalf ValidInterval() = 0;

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection) = 0;
    virtual bool IntersectAny(const Ray& ray) = 0;

    virtual void CollectStats(StatsManager& stats) = 0;

    // these are very slow and should be cached by the caller
    // assume to be valid within the valid interval
    // average area as average over the interval
    virtual Range3f ComputeBoundingBox() = 0;
    virtual float ComputeAverageArea() = 0;

    static shared_ptr<RayEngine> BuildDefault(
        const vector<shared_ptr<Surface> >& surfaces, 
        const vector<shared_ptr<InstanceGroup> >& instances,
        const Intervalf& time, int timeSamples);
};


template<typename T>
class RayEngineX : public RayEngine
{
public:
    RayEngineX(const T &tt) : t(tt) {}
    // valid interval for time vaiations
    virtual Intervalf   ValidInterval() { return t.ValidInterval(); }
    // contract: intersection is modified iff the ray hits
    virtual bool        Intersect(const Ray& ray, Intersection* intersection) { return t.Intersect(ray, intersection); }
    virtual bool        IntersectAny(const Ray& ray) { return t.IntersectAny(ray); }
    virtual void        CollectStats(StatsManager& stats) { t.CollectStats(stats); }
    virtual Range3f     ComputeBoundingBox() { return t.ComputeBoundingBox(); }
    virtual float       ComputeAverageArea() { return t.ComputeAverageArea(); }
    T t;
private:
    explicit RayEngineX(const RayEngineX &e) {}
};

#endif
