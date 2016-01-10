#ifndef _SHAPE_H_
#define _SHAPE_H_

#include "sceneObject.h"
#include <scene/smath.h>
#include <misc/stats.h>
#include "tesselation.h"

class Shape : public SceneObject {
public:
    virtual bool IsStatic() = 0;
    virtual Intervalf ComputeAnimationInterval() = 0;

    virtual float ComputeArea(float time) = 0;
    virtual Range3f ComputeBoundingBox(float time) = 0;

    virtual bool ApplyXform(shared_ptr<Xform> xform) = 0;

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time) = 0;
    virtual void Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples) = 0;

    virtual bool Intersect(const Ray& ray) { return false; }
    virtual bool Intersect(const Ray& ray, float& t, DifferentialGeometry& dp) { return false; }

    virtual void SamplePoint(const Vec2f& s, Vec3f* P, Vec3f *Ns) {};
};

#endif
