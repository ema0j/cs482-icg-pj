#include "rayPrimitive.h"

/*
inline float _RayEstimateTransformAreaHack(float area, const Matrix4d& m) {
    Range3f bbox(-area/2,area/2);
    return m.TransformBBox(bbox).GetSize().GetLength();
}
*/

Range3f RayGroupPrimitive::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    Range3f engineBBox = shapeEngine->ComputeBoundingBox();
    if(xform->IsStatic()) return xform->GetTransform(time.Middle()).TransformBBox(engineBBox);
    else {
        Range3f bbox;
        for(int i = 0; i < approximationSamples; i ++) {
            float t = (float)i / (approximationSamples-1);
            bbox.Grow(xform->GetTransform(time.Get(t)).TransformBBox(engineBBox));
        }
        return bbox;
    }
}

float RayGroupPrimitive::ComputeAverageArea(const Intervalf& time, int approximationSamples) {
    float engineArea = shapeEngine->ComputeAverageArea();
    if(xform->IsStatic()) return _RayEstimateTransformAreaHack(engineArea, xform->GetTransform(time.Middle()));
    else {
        float area = 0;
        for(int i = 0; i < approximationSamples; i ++) {
            float t = (float)i / (approximationSamples-1);
            area += _RayEstimateTransformAreaHack(engineArea, xform->GetTransform(time.Get(t)));
        }
        return area / approximationSamples;
    }
}

bool RayGroupPrimitive::Intersect(const Ray& ray, Intersection* intersection) {
    // xform the ray back
    Ray xformRay = ray;
    xformRay.Transform(xform->GetInverseTransform(ray.time));
    bool hit = shapeEngine->Intersect(xformRay, intersection);
    // if intersection, xform intersection data
    if(hit) {
        intersection->Transform(xform->GetTransform(ray.time));
        intersection->m = material;
    }
    return hit;
}

bool RayGroupPrimitive::Intersect(const Ray& ray) {
    // xform the ray back
    Ray xformRay = ray;
    xformRay.Transform(xform->GetInverseTransform(ray.time));
    // intersect instance
    return shapeEngine->IntersectAny(xformRay);
}

void RayGroupPrimitive::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Ray", "RayGroupPrimitive");
    stat->Increment();
}
