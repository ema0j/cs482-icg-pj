#include "tesselation.h"

void TriangleTesselationCache::AddTriangles(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec3i>& triangles) {
    // check if they have normals
    if(!normal.empty()) {
        this->pos = pos;
        this->normal = normal;
        if(!uv.empty()) this->uv = uv;
        else { this->uv.resize(pos.size()); this->uv.set(0.5); }
        this->triangles = triangles;
    } else {
        ElementOperations::UnshareForFaceNormals(pos, 
            (!uv.empty())?uv:carray<Vec2f>(pos.size(),Vec2f(0,0)), 
            ElementOperations::TriangleNormals(pos, triangles),
            triangles,
            this->pos, this->normal, this->uv, this->triangles);
    }

    _UpdateBBox();
}

void TriangleTesselationCache::AddQuads(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec4i>& quads) {
    // check if they have normals
    if(!normal.empty()) {
        AddTriangles(pos, normal, uv, ElementOperations::QuadsToTriangles(quads));
    } else {
        carray<Vec4i> newQuads;
        ElementOperations::UnshareForFaceNormals(pos, 
            (!uv.empty())?uv:carray<Vec2f>(pos.size(),Vec2f(0,0)), 
            ElementOperations::QuadNormals(pos, quads),
            quads,
            this->pos, this->normal, this->uv, newQuads);
        this->triangles = ElementOperations::QuadsToTriangles(newQuads);
    }

    _UpdateBBox();
}

void TriangleTesselationCache::_UpdateBBox() {
    bbox = Range3f();
    for(int i = 0; i < triangles.size(); i ++) {
        bbox.Grow(ElementOperations::TriangleBoundingBox(
            pos[triangles[i][0]],pos[triangles[i][1]],pos[triangles[i][2]]));
    }
}