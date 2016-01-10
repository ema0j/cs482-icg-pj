#ifndef _RAYTESSELATEDPRIMITIVE_H_
#define _RAYTESSELATEDPRIMITIVE_H_

#include "rayPrimitive.h"
#include "rayTesselationCache.h"
#include <scene/intersectionMethods.h>

class RayTesselatedTrianglePrimitive : public RayPrimitive {
public:
    RayTesselatedTrianglePrimitive(TriangleList* t, Material* m, int i) {
        this->triangles = t;
        this->material = m;
        this->idx = i;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16);
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16);

    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool Intersect(const Ray& ray);

    virtual void CollectStats(StatsManager& stats);

protected:
    TriangleList*   triangles;
    Material*       material;
    int             idx;

    template<class T> void _ResolveVertexAttrib(T& v0, T& v1, T& v2, const carray<T>& v);
    template<class T> T _InterpolateVertexAttrib(float b1, float b2, const carray<T>& a);
};

class RayTesselatedTimeSampledTrianglePrimitive : public RayPrimitive {
public:
    RayTesselatedTimeSampledTrianglePrimitive(TimeSampledTriangleList* t, Material* m, int i) {
        this->triangles = t;
        this->material = m;
        this->idx = i;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16);
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16);

    virtual bool Intersect(const Ray& ray, Intersection* intersection) ;
    virtual bool Intersect(const Ray& ray);

    virtual void CollectStats(StatsManager& stats);

protected:
    TimeSampledTriangleList*    triangles;
    Material*                   material;
    int                         idx;

    template<class T> void _ResolveVertexAttrib(T& v0, T& v1, T& v2, int timeIdx, const tarray<T>& v);
    template<class T> void _InterpolateVertexAttrib(T& v0, T& v1, T& v2, float time, const tarray<T>& v);
    template<class T> T _InterpolateFaceAttrib(float time, const tarray<T>& f);
    template<class T> T _InterpolateVertexAttrib(float b1, float b2, float time, const tarray<T>& a);
};

class RayTesselatedSpherePrimitive : public RayPrimitive {
public:
    RayTesselatedSpherePrimitive(SphereList* s, Material* m, int i) {
        this->spheres = s;
        this->material = m;
        this->idx = i;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16);
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16);

    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool Intersect(const Ray& ray);

    virtual void CollectStats(StatsManager& stats) ;

protected:
    SphereList*   spheres;
    Material*       material;
    int             idx;

    void _ResolveSphereAttrib(Vec3f& p, float& r, Matrix4d& m, Matrix4d& mi);
};

class RayTesselatedSegmentPrimitive : public RayPrimitive {
public:
    RayTesselatedSegmentPrimitive(SegmentList* s, Material* m, int i) {
        this->segments = s;
        this->material = m;
        this->idx = i;
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16);
    virtual float ComputeAverageArea(const Intervalf& time, int approximationSamples = 16);

    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool Intersect(const Ray& ray);

    virtual void CollectStats(StatsManager& stats);

protected:
    SegmentList*    segments;
    Material*       material;
    int             idx;

    template<class T> void _ResolveVertexAttrib(T& v0, T& v1, const carray<T>& v);
    template<class T> T _InterpolateVertexAttrib(float u, const carray<T>& a);
};

#endif
