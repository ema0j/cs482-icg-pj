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

class RayShapePrimitive : public RayPrimitive {
public:
    RayShapePrimitive(Shape* s, Material* m) {
        shape = s;
        material = m;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16) { return shape->ComputeMaxBoundingBox(time, approximationSamples); }
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16) { return shape->ComputeAverageArea(time, approximationSamples); }

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection) { 
        float t; DifferentialGeometry dp;
        bool hit = shape->Intersect(ray, t, dp); 
        if(hit) {
            intersection->t = t;
            intersection->dp = dp;
            intersection->m = material;
        }
        return hit;
    }
    virtual bool Intersect(const Ray& ray) { return shape->Intersect(ray); }

    virtual void CollectStats(StatsManager& stats) {
        StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "RayShapePrimitive");
        stat->Increment();
    }

protected:
    Material* material;
    Shape* shape;
};

class RayShapeElementPrimitive : public RayPrimitive {
public:
    RayShapeElementPrimitive(ElementShape* s, Material* m, int eid) {
        shape = s;
        material = m;
        elementId = eid;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16) { return shape->ComputeMaxBoundingBoxElement(time, elementId, approximationSamples); }
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16) { return shape->ComputeAverageAreaElement(time, elementId, approximationSamples); }

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection) { 
        float t; DifferentialGeometry dp;
        bool hit = shape->IntersectElement(ray, t, dp, elementId); 
        if(hit) {
            intersection->t = t;
            intersection->dp = dp;
            intersection->m = material;
        }
        return hit;
    }
    virtual bool Intersect(const Ray& ray) { return shape->IntersectElement(ray, elementId); }

    virtual void CollectStats(StatsManager& stats) {
        StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "RayShapeElementPrimitive");
        stat->Increment();
    }

protected:
    Material* material;
    ElementShape* shape;
    int elementId;
};

class RaySurfacePrimitive : public RayPrimitive {
public:
    RaySurfacePrimitive(shared_ptr<RayEngine> e, Surface* s) {
        shapeEngine = e;
        surface = s;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16) { return surface->ComputeMaxBoundingBox(time, approximationSamples); }
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16) { return surface->ComputeAverageArea(time, approximationSamples); }

    virtual bool Intersect(const Ray& ray, Intersection* intersection) {
        // xform the ray back
        Ray xformRay = ray;
        Xform* xform = surface->GetXform().get();
        xformRay.Transform(xform->GetInverseTransform(ray.time));
        bool hit = shapeEngine->Intersect(xformRay, intersection);
        // if intersection, xform intersection data
        if(hit) {
            intersection->Transform(xform->GetTransform(ray.time));
            intersection->m = surface->GetMaterial().get();
        }
        return hit;
    }

    virtual bool Intersect(const Ray& ray) {
        // xform the ray back
        Ray xformRay = ray;
        Xform* xform = surface->GetXform().get();
        xformRay.Transform(xform->GetInverseTransform(ray.time));
        // intersect instance
        return shapeEngine->IntersectAny(xformRay);
    }

    virtual void CollectStats(StatsManager& stats) {
        StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "RaySurfacePrimitive");
        stat->Increment();
        shapeEngine->CollectStats(stats);
    }

protected:
    Surface* surface;
    shared_ptr<RayEngine> shapeEngine;
};

#endif
