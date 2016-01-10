#ifndef _MESH_H_
#define _MESH_H_

#include "shape.h"
#include "tesselation.h"

class MeshShape : public Shape {
public:
    MeshShape() { }

	MeshShape(shared_ptr<carray<Vec3f> > pos, shared_ptr<carray<Vec3f> > normal, shared_ptr<carray<Vec2f> > texcoord, shared_ptr<carray<int> > idxs);
	MeshShape(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& texcoord, const carray<Vec3i>& idxs);

    virtual void CollectStats(StatsManager& stats);

    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

    uint64_t NFaces() { return faceArray.size(); }
    uint64_t NVertices() { return posArray.size(); }
    virtual uint64_t NElements() { return NFaces(); }

    bool HasNormal() { return !normalArray.empty(); }
    bool HasUv() { return !uvArray.empty(); }

    carray<Vec3f>& PosArray() { return posArray; }
    carray<Vec3f>& NormalArray() { return normalArray; }
    carray<Vec2f>& UvArray() { return uvArray; }
    carray<Vec3i>& FaceArray() { return faceArray; }

    Vec3f Pos(int face, int vertex) { return posArray[faceArray[face][vertex]]; }
    Vec3f Normal(int face, int vertex) { return normalArray[faceArray[face][vertex]]; }
    Vec2f Uv(int face, int vertex) { return uvArray[faceArray[face][vertex]]; }
    int VertexIndex(int face, int vertex) { return faceArray[face][vertex]; }
    Vec3f TriangleNormal(int face) { return ((Pos(face, 1) - Pos(face, 0)) ^ (Pos(face, 2) - Pos(face, 0))).GetNormalized(); }

    virtual float ComputeArea(float time);
    virtual Range3f ComputeBoundingBox(float time);

    virtual bool ApplyXform(shared_ptr<Xform> xform);

    virtual void Tesselate(shared_ptr<TesselationCache> context, float time);
    virtual void Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples) { Tesselate(cache,time.Middle()); }

    virtual bool Intersect(const Ray& ray);
    virtual bool Intersect(const Ray& ray, float& t, DifferentialGeometry& dp);

	static string serialize_typename();
	virtual void serialize(Archive* a) ;

protected:
    carray<Vec3f> posArray;
    carray<Vec3f> normalArray;
    carray<Vec2f> uvArray;
    carray<Vec3i> faceArray;
};

#endif
