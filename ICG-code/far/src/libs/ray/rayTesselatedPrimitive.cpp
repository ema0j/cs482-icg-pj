#include "rayTesselatedPrimitive.h"

template<class T>
inline void RayTesselatedTrianglePrimitive::_ResolveVertexAttrib(T& v0, T& v1, T& v2, const carray<T>& v) {
    v0 = v[triangles->triangles[idx][0]];
    v1 = v[triangles->triangles[idx][1]];
    v2 = v[triangles->triangles[idx][2]];
}

template<class T>
inline T RayTesselatedTrianglePrimitive::_InterpolateVertexAttrib(float b1, float b2, const carray<T>& a) {
    T a0, a1, a2; _ResolveVertexAttrib(a0,a1,a2,a);
    return a0 * (1-b1-b2) + a1 * b1 + a2 * b2;
}

Range3f RayTesselatedTrianglePrimitive::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    Vec3f v0, v1, v2; _ResolveVertexAttrib(v0,v1,v2,triangles->pos);
    return ElementOperations::TriangleBoundingBox(v0,v1,v2);
}

float RayTesselatedTrianglePrimitive::ComputeAverageArea(const Intervalf& time, int approximationSamples) {
    Vec3f v0, v1, v2; _ResolveVertexAttrib(v0,v1,v2,triangles->pos);
    return ElementOperations::TriangleArea(v0,v1,v2);
}

bool RayTesselatedTrianglePrimitive::Intersect(const Ray& ray, Intersection* intersection) {
    Vec3f v0, v1, v2; _ResolveVertexAttrib(v0,v1,v2,triangles->pos);
    float t, b1, b2, rayEpsilon;
    bool hit = IntersectTriangle(v0,v1,v2,ray, &t, &b1, &b2, &rayEpsilon);

    if(hit) {
        // geometry
        intersection->t = t;
        intersection->rayEpsilon = rayEpsilon;
        intersection->dp.P = ray.Eval(t);
        intersection->dp.uv = Vec2f(b1,b2);
        if(triangles->normal.empty()) {
            intersection->dp.Ng = triangles->faceNormal[idx];
            intersection->dp.N = triangles->faceNormal[idx];
        } else {
            intersection->dp.Ng = ElementOperations::TriangleNormal(v0,v1,v2);
            intersection->dp.N = _InterpolateVertexAttrib(b1,b2,triangles->normal).GetNormalized();
        }
        intersection->dp.GenerateTuTv();
        
        // shading
        if(triangles->uv.empty()) {
            intersection->dp.st = intersection->dp.uv;
        } else {
            intersection->dp.st = _InterpolateVertexAttrib(b1,b2,triangles->uv);
        }

        // material
        intersection->m = material;
    }
    
    return hit;
}

bool RayTesselatedTrianglePrimitive::Intersect(const Ray& ray) {
    Vec3f v0, v1, v2; _ResolveVertexAttrib(v0,v1,v2,triangles->pos);
    float t, b1, b2, rayEpsilon;
    return IntersectTriangle(v0,v1,v2,ray, &t, &b1, &b2, &rayEpsilon);
}

void RayTesselatedTrianglePrimitive::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Ray", "RayTesselatedTrianglePrimitive");
    stat->Increment();
}

template<class T>
inline void RayTesselatedTimeSampledTrianglePrimitive::_ResolveVertexAttrib(T& v0, T& v1, T& v2, int timeIdx, const tarray<T>& v) {
    v0 = v.at(triangles->triangles[idx][0], timeIdx);
    v1 = v.at(triangles->triangles[idx][1], timeIdx);
    v2 = v.at(triangles->triangles[idx][2], timeIdx);
}

template<class T>
inline void RayTesselatedTimeSampledTrianglePrimitive::_InterpolateVertexAttrib(T& v0, T& v1, T& v2, float time, const tarray<T>& v) {
    v0 = v.interpolate(triangles->triangles[idx][0], time);
    v1 = v.interpolate(triangles->triangles[idx][1], time);
    v2 = v.interpolate(triangles->triangles[idx][2], time);
}

template<class T>
inline T RayTesselatedTimeSampledTrianglePrimitive::_InterpolateFaceAttrib(float time, const tarray<T>& f) {
    return f.interpolate(idx, time);
}

template<class T>
inline T RayTesselatedTimeSampledTrianglePrimitive::_InterpolateVertexAttrib(float b1, float b2, float time, const tarray<T>& a) {
    T a0, a1, a2; _InterpolateVertexAttrib(a0,a1,a2,time,a);
    return a0 * (1-b1-b2) + a1 * b1 + a2 * b2;
}

Range3f RayTesselatedTimeSampledTrianglePrimitive::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    Range3f ret;
    for(int i = 0; i < triangles->pos.times(); i ++) {
        Vec3f v0, v1, v2; _ResolveVertexAttrib(v0,v1,v2,i,triangles->pos);
        ret.Grow(ElementOperations::TriangleBoundingBox(v0,v1,v2));
    }
    return ret;
}

float RayTesselatedTimeSampledTrianglePrimitive::ComputeAverageArea(const Intervalf& time, int approximationSamples) {
    float ret = 0;
    for(int i = 0; i < triangles->pos.times(); i ++) {
        Vec3f v0, v1, v2; _ResolveVertexAttrib(v0,v1,v2,i,triangles->pos);
        ret += ElementOperations::TriangleArea(v0,v1,v2);
    }
    return ret / triangles->pos.times();
}

bool RayTesselatedTimeSampledTrianglePrimitive::Intersect(const Ray& ray, Intersection* intersection) {
    Vec3f v0, v1, v2; _InterpolateVertexAttrib(v0,v1,v2,ray.time,triangles->pos);
    float t, b1, b2, rayEpsilon;
    bool hit = IntersectTriangle(v0,v1,v2,ray, &t, &b1, &b2, &rayEpsilon);

    if(hit) {
        // geometry
        intersection->t = t;
        intersection->dp.P = ray.Eval(t);
        intersection->dp.uv = Vec2f(b1,b2);
        if(triangles->normal.empty()) {
            intersection->dp.Ng = ElementOperations::TriangleNormal(v0,v1,v2);
            intersection->dp.N = _InterpolateFaceAttrib(ray.time,triangles->faceNormal);
        } else {
            intersection->dp.Ng = ElementOperations::TriangleNormal(v0,v1,v2);
            intersection->dp.N = _InterpolateVertexAttrib(b1,b2,ray.time,triangles->normal).GetNormalized();
        }
        intersection->dp.GenerateTuTv();
        
        // shading
        if(triangles->uv.empty()) {
            intersection->dp.st = intersection->dp.uv;
        } else {
            intersection->dp.st = _InterpolateVertexAttrib(b1,b2,ray.time,triangles->uv);
        }

        // material
        intersection->m = material;
    }
    
    return hit;
}

bool RayTesselatedTimeSampledTrianglePrimitive::Intersect(const Ray& ray) {
    Vec3f v0, v1, v2; _InterpolateVertexAttrib(v0,v1,v2,ray.time,triangles->pos);
    float t, b1, b2, rayEpsilon;
    return IntersectTriangle(v0,v1,v2,ray, &t, &b1, &b2, &rayEpsilon);
}

void RayTesselatedTimeSampledTrianglePrimitive::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Ray", "RayTesselatedTrianglePrimitive");
    stat->Increment();
}

inline void RayTesselatedSpherePrimitive::_ResolveSphereAttrib(Vec3f& p, float& r, Matrix4d& m, Matrix4d& mi) {
    p = spheres->pos[idx];
    r = spheres->radius[idx];
    m = spheres->m[idx];
    mi = spheres->mi[idx];
}

Range3f RayTesselatedSpherePrimitive::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    Vec3f p; float r; Matrix4d m; Matrix4d mi; _ResolveSphereAttrib(p,r,m,mi);
    return m.TransformBBox(ElementOperations::SphereBoundingBox(p,r));
}

float RayTesselatedSpherePrimitive::ComputeAverageArea(const Intervalf& time, int approximationSamples) {
    Vec3f p; float r; Matrix4d m; Matrix4d mi; _ResolveSphereAttrib(p,r,m,mi);
    float area = ElementOperations::SphereArea(r);
    Range3f bbox(-area/2,area/2);
    return m.TransformBBox(bbox).GetSize().GetLength();
}

bool RayTesselatedSpherePrimitive::Intersect(const Ray& ray, Intersection* intersection) {
    Vec3f p; float r; float rayEpsilon; Matrix4d m; Matrix4d mi; _ResolveSphereAttrib(p,r,m,mi);

    // xform the ray
    Ray xformRay = ray;
    xformRay.Transform(mi);

    float t;
    bool hit = IntersectSphere(p, r, xformRay, &t, &rayEpsilon);

    if(hit) {
        intersection->rayEpsilon = rayEpsilon;
        intersection->t = t;
        intersection->dp.P = xformRay.Eval(t);
        intersection->dp.Ng = (intersection->dp.P - p).GetNormalized();
        intersection->dp.N = intersection->dp.Ng;
        intersection->dp.uv[0] = (atan2f(intersection->dp.N[0], intersection->dp.N[1]) + (float)PI) / (float)(2*PI);
        intersection->dp.uv[1] = acosf(intersection->dp.N[2]) / (float)PI;
        intersection->dp.GenerateTuTv();
        intersection->dp.st = intersection->dp.uv;
        intersection->m = material;
    }

    // xform back
    if(hit) {
        intersection->Transform(m);
    }

    return hit;
}

bool RayTesselatedSpherePrimitive::Intersect(const Ray& ray) {
    Vec3f p; float r; Matrix4d m; Matrix4d mi; _ResolveSphereAttrib(p,r,m,mi);
    Ray xformRay = ray;
    xformRay.Transform(mi);
    float t, rayEpsilon;
    return IntersectSphere(p, r, xformRay, &t, &rayEpsilon);
}

void RayTesselatedSpherePrimitive::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Ray", "RayTesselatedSpherePrimitive");
    stat->Increment();
}

template<class T>
inline void RayTesselatedSegmentPrimitive::_ResolveVertexAttrib(T& v0, T& v1, const carray<T>& v) {
    v0 = v[segments->segments[idx][0]];
    v1 = v[segments->segments[idx][1]];
}

template<class T>
inline T RayTesselatedSegmentPrimitive::_InterpolateVertexAttrib(float u, const carray<T>& a) {
    T a0, a1; _ResolveVertexAttrib(a0,a1,a);
    return a0 * (1-u) + a1 * u;
}

Range3f RayTesselatedSegmentPrimitive::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    Vec3f v0, v1; _ResolveVertexAttrib(v0,v1,segments->pos);
    return ElementOperations::CylinderBoundingBox(v0,v1,segments->radius);
}

float RayTesselatedSegmentPrimitive::ComputeAverageArea(const Intervalf& time, int approximationSamples) {
    Vec3f v0, v1; _ResolveVertexAttrib(v0,v1,segments->pos);
    return ElementOperations::CylinderArea(v0,v1,segments->radius);
}

bool RayTesselatedSegmentPrimitive::Intersect(const Ray& ray, Intersection* intersection) {
    Vec3f v0, v1; _ResolveVertexAttrib(v0,v1,segments->pos);
    float t,u, rayEpsilon;
    bool hit = IntersectHairSegment(v0,v1,segments->radius,ray,&t,&u,&rayEpsilon);

    if(hit) {
        // intesection
        intersection->rayEpsilon = rayEpsilon;
        intersection->t = t;
        intersection->dp.P = ray.Eval(t);
        intersection->dp.uv = Vec2f(u,0);
        if(segments->tangent.empty()) {
            intersection->dp.Ng = segments->segmentTangent[idx];
            intersection->dp.N = segments->segmentTangent[idx];
        } else {
            intersection->dp.Ng = ElementOperations::SegmentTangent(v0,v1);
            intersection->dp.N = _InterpolateVertexAttrib(u,segments->tangent).GetNormalized();
        }
        intersection->dp.GenerateTuTv();
        if(segments->uv.empty()) {
            intersection->dp.st = intersection->dp.uv;
        } else {
            intersection->dp.st = _InterpolateVertexAttrib(u,segments->uv);
        }
        intersection->m = material;
    }

    return hit;
}

bool RayTesselatedSegmentPrimitive::Intersect(const Ray& ray) {
    Vec3f v0, v1; _ResolveVertexAttrib(v0,v1,segments->pos);
    float t, u, rayEpsilon;
    return IntersectHairSegment(v0, v1, segments->radius, ray, &t, &u, &rayEpsilon);
}

void RayTesselatedSegmentPrimitive::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Ray", "RayTesselatedSegmentPrimitive");
    stat->Increment();
}

