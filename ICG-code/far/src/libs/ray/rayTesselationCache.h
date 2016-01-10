#ifndef _RAYTESSELATIONCACHE_H_
#define _RAYTESSELATIONCACHE_H_

#include <scene/tesselation.h>

struct TriangleList {
    carray<Vec3f> pos;
    carray<Vec3f> normal;
    carray<Vec2f> uv;
    carray<Vec3i> triangles;
    carray<Vec3f> faceNormal; // one per face

    void Transform(const Matrix4d& m, const Matrix4d& mi);
};

struct SphereList {
    carray<Vec3f> pos;
    carray<float> radius;
    carray<Matrix4d> m;
    carray<Matrix4d> mi;

    void Transform(const Matrix4d& m, const Matrix4d& mi);
};

struct SegmentList {
    float radius;
    carray<Vec3f> pos;
    carray<Vec3f> tangent;
    carray<Vec2f> uv;
    carray<Vec2i> segments;
    carray<Vec3f> segmentTangent; // one per segment

    void Transform(const Matrix4d& m, const Matrix4d& mi);
};

struct TimeSampledTriangleList {
    tarray<Vec3f> pos;
    tarray<Vec3f> normal;
    tarray<Vec2f> uv;
    carray<Vec3i> triangles;
    tarray<Vec3f> faceNormal; // one per face

    void Transform(const Matrix4d& m, const Matrix4d& mi);
};

class RayTesselationCache : public TesselationCache {
public:
    virtual bool SupportSpheres() { return true; }
    virtual bool SupportSegments() { return true; }
    virtual bool SupportTimeSampledTriangles() { return true; }

    TriangleList& Triangles() { return triangles; }
    SphereList& Spheres() { return spheres; }
    SegmentList& Segments() { return segments; }
    TimeSampledTriangleList& TimeSampledTriangles() { return timeSampledTriangles; }

    void Transform(const Matrix4d& m, const Matrix4d& mi);

    // if normal is empty, use face normals | if uv is empty, use 0.5
    virtual void AddTriangles(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec3i>& face);
    // if normal is empty, use face normals | if uv is empty, use 0.
    virtual void AddQuads(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec4i>& face);
    // only call if support is provided, might be removed later on
    virtual void AddSpheres(const carray<Vec3f>& pos, const carray<float>& radius);
    // if tangent is empty, use segment tangents | if us is empty, use 0.5
    virtual void AddSegments(const carray<Vec3f>& pos, const carray<Vec3f>& tangent, const carray<Vec2f>& uv, float radius, const carray<Vec2i>& segment);
    // if normal is empty, use face normals | if uv is empty, use 0.5
    virtual void AddTimeSampledTriangles(const tarray<Vec3f>& pos, const tarray<Vec3f>& normal, const tarray<Vec2f>& uv, const carray<Vec3i>& face);
protected:
    TriangleList triangles;
    SegmentList segments;
    SphereList spheres;
    TimeSampledTriangleList timeSampledTriangles;
};

#endif
