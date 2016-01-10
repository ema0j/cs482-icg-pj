#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "surface.h"
#include "intersectionMethods.h"
#include <scene/smath.h>
#include "tesselation.h"
#include <vmath/functions.h>

class SphereShape : public Shape {
public:
	SphereShape() { pos = Vec3f::Zero(); radius = 1; }
    SphereShape(const Vec3f& p, float r) { pos = p; radius = r; }

    virtual void CollectStats(StatsManager& stats);

    float GetRadius() { return radius; }
    void SetRadius(float r) { radius = r; }
    Vec3f GetCenter() { return pos; }
    void SetCenter(const Vec3f& c) { pos = c; }

    Vec3f InterpolatePos(const Vec2f& uv);
    void InterpolatePosNormal(const Vec2f& uv, Vec3f* P, Vec3f* N);

    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

    virtual float ComputeArea(float time);
    virtual Range3f ComputeBoundingBox(float time);

    virtual bool Intersect(const Ray& ray);
    virtual bool Intersect(const Ray& ray, float& t, float &rayEpsilon, DifferentialGeometry& dp);

    virtual bool ApplyXform(shared_ptr<Xform> xform) { return false; }

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time);
    virtual void Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples) { Tesselate(cache,time.Middle()); }

	static string serialize_typename(); 
	virtual void serialize(Archive* a);

protected:
    Vec3f      pos;
    float       radius;
};

#endif
