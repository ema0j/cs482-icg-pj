#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include "smath.h"
#include <misc/arrays.h>

// all counter clock-wise operations
class ElementOperations {
public:
    static carray<Vec3i> QuadsToTriangles(const carray<Vec4i>& quads);
    static carray<Vec3f> TriangleNormals(const carray<Vec3f>& pos, const carray<Vec3i>& triangles);
    static carray<Vec3f> QuadNormals(const carray<Vec3f>& pos, const carray<Vec4i>& quads);
    static carray<Vec3f> SegmentTangents(const carray<Vec3f>& pos, const carray<Vec2i>& segments);
    static carray<Vec3f> SmoothTriangleNormals(const carray<Vec3f> pos, const carray<Vec3i>& triangles);

    // this is useful to push face data into vertices
    template<class ElementType>
    static void UnshareForFaceNormals(
        const carray<Vec3f>& pos, const carray<Vec2f>& uv, 
        const carray<Vec3f>& elementNormal, const carray<ElementType>& elements,
        carray<Vec3f>& outPos, carray<Vec3f>& outNormal, 
        carray<Vec2f>& outUv, carray<ElementType>& outElements);

    // starts from face data with an index for each P,N,uv
    // create unique vertices with P,N,uv and a face array
    // the array in VertexArray has x,y,z corresponing to P,N,uv
    static void ShareTriangleVertices(const carray<Vec3f>& pos, const carray<Vec3f>& normal, 
                                      const carray<Vec2f>& uv, const carray<Vec3i>& vertices,
                                      carray<Vec3f>& trianglePos, carray<Vec3f>& triangleNormal, 
                                      carray<Vec2f>& triangleUv, carray<Vec3i>& triangles);

	static void ShareTriangleVertices(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2i>& vertices,
								      carray<Vec3f>& trianglePos, carray<Vec3f>& triangleNormal, carray<Vec3i>& triangles);

    static Vec3f TriangleNormal(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2) {
        return ((v1-v0)^(v2-v0)).GetNormalized();
    }
    static Vec3f QuadNormal(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Vec3f& v3) {
        return (TriangleNormal(v0,v1,v2)+TriangleNormal(v1,v2,v3)+
                TriangleNormal(v2,v3,v0)+TriangleNormal(v3,v0,v1)).GetNormalized();
    }
    static Vec3f SegmentTangent(const Vec3f& v0, const Vec3f& v1) {
        return (v1-v0).GetNormalized();
    }

    static Range3f TriangleBoundingBox(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2) {
        Range3f ret = Range3f::Empty(); ret.Grow(v0); ret.Grow(v1); ret.Grow(v2); return ret;
    }
    static Range3f SphereBoundingBox(const Vec3f& p, float r) {
        Range3f ret = Range3f::Empty(); ret.SetMin(p - r); ret.SetMax(p + r); return ret;
    }
    // TODO: very conservative!!!
    static Range3f CylinderBoundingBox(const Vec3f& v0, const Vec3f& v1, float r) {
        Range3f ret = Range3f::Empty();  ret.Grow(v0); ret.Grow(v1);  ret.Grow(v0 + r); ret.Grow(v0 - r); ret.Grow(v1 + r); ret.Grow(v1 - r); return ret;
    }
    static Range3f VerticesBoundingBox(const carray<Vec3f>& vertices) {
        Range3f ret = Range3f::Empty();
        for(uint32_t i = 0; i < vertices.size(); i ++) {
            ret.Grow(vertices[i]);
        }
        return ret;
    }
    
    static float TriangleArea(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2) {
        return ((v1-v0) ^ (v2-v0)).GetLength() / 2;
    }
    static float SphereArea(float r) {
        return 4 * PIf * r;
    }
    static float CylinderArea(const Vec3f& v0, const Vec3f& v1, float r) {
        return 2 * PIf * r * (v1 - v0).GetLength();
    }
    
    static carray<Vec3f> NormalizeVerticesToUnitCube(const carray<Vec3f>& pos); // translate/uniform-scale into [-1, 1]^3
    static carray<Vec3f> NormalizeVerticesToBox(const carray<Vec3f>& pos, const Range3f& box); // translate/uniform-scale to fit into box
    static carray<Vec3f> NormalizeVerticesToBoxFromRef(const carray<Vec3f>& pos, const Range3f& box, const Range3f& ref); // translate/uniform-scale to fit into box
};

template<class ElementType>
inline void ElementOperations::UnshareForFaceNormals(
    const carray<Vec3f>& pos, const carray<Vec2f>& uv, 
    const carray<Vec3f>& elementNormal, const carray<ElementType>& elements,
    carray<Vec3f>& outPos, carray<Vec3f>& outNormal, 
    carray<Vec2f>& outUv, carray<ElementType>& outElements) {
    int elementSize = ElementType::NumComponents();
    // duplicate all vertices to preserve hard shape
    int nf = static_cast<int>(elements.size());
    outPos.resize(nf*elementSize);
    outNormal.resize(nf*elementSize);
    outUv.resize(nf*elementSize);
    outElements.resize(nf);

    // copy all vertex data
    for(int f = 0; f < nf; f ++) {
        for(int v = 0; v < elementSize; v ++) {
            int idx = f*elementSize+v;
            outPos[idx] = pos[elements[f][v]];
            outNormal[idx] = elementNormal[f];
            outUv[idx] = uv[elements[f][v]];
            outElements[f][v] = idx;
        }
    }
}

#endif

