#include "rayTesselationCache.h"

void TriangleList::Transform(const Matrix4d& m, const Matrix4d& mi) {
    m.TransformPointArray(pos);
    m.TransformNormalArrayNoAdjoint(normal);
    m.TransformNormalArrayNoAdjoint(faceNormal);
}

void SphereList::Transform(const Matrix4d& m, const Matrix4d& mi) {
    for(int i = 0; i < this->m.size(); i ++) {
        this->m[i] = this->m[i] % m;
        this->mi[i] = mi % this->mi[i];
    }
}

void SegmentList::Transform(const Matrix4d& m, const Matrix4d& mi) {
    m.TransformPointArray(pos);
    m.TransformNormalizedVectorArray(tangent);
    m.TransformNormalizedVectorArray(segmentTangent);
}

void TimeSampledTriangleList::Transform(const Matrix4d& m, const Matrix4d& mi) {
    m.TransformPointArray<float>(pos.data(), pos.size());
    m.TransformNormalArrayNoAdjoint<float>(normal.data(), normal.size());
    m.TransformNormalArrayNoAdjoint<float>(faceNormal.data(), faceNormal.size());
}

void RayTesselationCache::Transform(const Matrix4d& m, const Matrix4d& mi) {
    triangles.Transform(m,mi);
    spheres.Transform(m,mi);
    segments.Transform(m,mi);
    timeSampledTriangles.Transform(m,mi);
}

// if normal is empty, use face normals | if uv is empty, use 0.5
void RayTesselationCache::AddTriangles(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec3i>& face) {
    if(!normal.empty()) {
        triangles.pos = pos;
        triangles.normal = normal;
        triangles.uv = uv;
        triangles.triangles = face;
    } else {
        triangles.pos = pos;
        triangles.faceNormal = ElementOperations::TriangleNormals(pos, face);
        triangles.uv = uv;
        triangles.triangles = face;
    }
}
// if normal is empty, use face normals | if uv is empty, use 0.
void RayTesselationCache::AddQuads(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec4i>& face) {
    if(!normal.empty()) {
        triangles.pos = pos;
        triangles.normal = normal;
        triangles.uv = uv;
        triangles.triangles = ElementOperations::QuadsToTriangles(face);
    } else {
        triangles.pos = pos;
        carray<Vec3f> quadNormal = ElementOperations::QuadNormals(pos, face);
        triangles.faceNormal.resize(quadNormal.size()*2);
        for(int f = 0; f < quadNormal.size(); f ++) {
            triangles.faceNormal[f*2+0] = quadNormal[f];
            triangles.faceNormal[f*2+1] = quadNormal[f];
        }
        triangles.uv = uv;
        triangles.triangles = ElementOperations::QuadsToTriangles(face);
    }
}

// only call if support is provided, might be removed later on
void RayTesselationCache::AddSpheres(const carray<Vec3f>& pos, const carray<float>& radius) {
    spheres.pos = pos;
    spheres.radius = radius;
    spheres.m.resize(pos.size()); spheres.m.set(Matrix4d::Identity());
    spheres.mi.resize(pos.size()); spheres.mi.set(Matrix4d::Identity());
}

// if tangent is empty, use segment tangents | if us is empty, use 0.5
void RayTesselationCache::AddSegments(const carray<Vec3f>& pos, const carray<Vec3f>& tangent, const carray<Vec2f>& uv, float radius, const carray<Vec2i>& segment) {
    if(!tangent.empty()) {
        segments.pos = pos;
        segments.tangent = tangent;
        segments.uv = uv;
        segments.radius = radius;
        segments.segments = segment;
    } else {
        segments.pos = pos;
        segments.uv = uv;
        segments.radius = radius;
        segments.segmentTangent = ElementOperations::SegmentTangents(pos,segment);
        segments.segments = segment;
    }
}

// if normal is empty, use face normals | if uv is empty, use 0.5
void RayTesselationCache::AddTimeSampledTriangles(const tarray<Vec3f>& pos, const tarray<Vec3f>& normal, const tarray<Vec2f>& uv, const carray<Vec3i>& face) {
    if(!normal.empty()) {
        timeSampledTriangles.pos = pos;
        timeSampledTriangles.normal = normal;
        timeSampledTriangles.uv = uv;
        timeSampledTriangles.triangles = face;
    } else {
        timeSampledTriangles.pos = pos;
        timeSampledTriangles.faceNormal.resize(face.size(), pos.times(), pos.interval());
        for(int time = 0; time < pos.times(); time ++) {
            timeSampledTriangles.faceNormal.setAtTime(time, ElementOperations::TriangleNormals(pos.atTime(time), face));
        }
        timeSampledTriangles.uv = uv;
        timeSampledTriangles.triangles = face;
    }
}
