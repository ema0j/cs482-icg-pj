#ifndef _RAYPRIMITIVE_H_
#define _RAYPRIMITIVE_H_

#include <misc/stats.h>
#include <scene/surface.h>

#include "compilerOptions.h"
#include "intersection.h"
#include "rayEngine.h"

class RayPrimitive {
public:
    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16) = 0;
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16) = 0; // does not have to be correct!

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection) = 0;
    virtual bool Intersect(const Ray& ray) = 0;

    virtual void CollectStats(StatsManager& stats) = 0;
};

inline float _RayEstimateTransformAreaHack(float area, const Matrix4d& m) {
    Range3f bbox(-area/2,area/2);
    return m.TransformBBox(bbox).GetSize().GetLength();
}

class RayGroupPrimitive : public RayPrimitive {
public:
    RayGroupPrimitive(shared_ptr<RayEngine> e, Xform* xf, Material* m) {
        shapeEngine = e;
        xform = xf;
        material = m;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16) ;
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16);

    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool Intersect(const Ray& ray);

    virtual void CollectStats(StatsManager& stats);

protected:
    Xform* xform;
    Material* material;
    shared_ptr<RayEngine> shapeEngine;
};

#endif
