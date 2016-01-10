#ifndef _DEFORMEDMESH_H_
#define _DEFORMEDMESH_H_

#include "shape.h"
#include "shape_mesh.h"
#include "xform.h"
#include <misc/arrays.h>

#define __BBOX_TIMESAMPLES 16

// mesh with fixed topology, but varying geometry
class DeformedMeshShape : public Shape {
public:
    virtual void CollectStats(StatsManager& stats);

    virtual bool IsStatic() { return false; }
    virtual Intervalf ComputeAnimationInterval() = 0;

    virtual uint64_t NFaces() = 0;
    virtual uint64_t NVertices() = 0;

    virtual bool HasNormal() = 0;
    virtual bool HasUv() = 0;

    virtual Vec3f Pos(int face, int vertex, float time) = 0;
    virtual Vec3f Normal(int face, int vertex, float time) = 0;
    virtual Vec2f Uv(int face, int vertex, float time) = 0;
    Vec3f TriangleNormal(int face, float time) {
        return ( (Pos(face,1,time) - Pos(face,0,time)) ^ (Pos(face,2,time) - Pos(face,0,time))).GetNormalized();
    }

    virtual float ComputeArea(float time);
    virtual Range3f ComputeBoundingBox(float time);

    virtual bool ApplyXform(shared_ptr<Xform> xform) { return false; }

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time) = 0;
    virtual void Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples);

protected:
    virtual uint64_t GetMemoryAllocatedStats();
};

class SkinnedMeshShape : public DeformedMeshShape {
public:
	SkinnedMeshShape() { }

    SkinnedMeshShape(shared_ptr<MeshShape> refPose, shared_ptr<carray< shared_ptr<carray<float> > > > boneWeights,
        shared_ptr<carray< shared_ptr< carray<int> > > > boneIndex, shared_ptr<carray<shared_ptr<Xform> > > bonesAnim);

    virtual Intervalf ComputeAnimationInterval();

    // shared_ptr<MeshShape> GetPoseMesh() { return refPoseMesh; }

    virtual uint64_t NFaces() { return refPoseMesh->NFaces(); }
    virtual uint64_t NVertices() { return refPoseMesh->NVertices(); }

    virtual bool HasNormal() { return refPoseMesh->HasNormal(); }
    virtual bool HasUv() { return refPoseMesh->HasUv(); }

    virtual Vec3f Pos(int face, int vertex, float time);
    virtual Vec3f Normal(int face, int vertex, float time);
    virtual Vec2f Uv(int face, int vertex, float time);

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time);

	static string serialize_typename();
	virtual void serialize(Archive* a);
	
protected:
    shared_ptr<MeshShape> refPoseMesh;

    carray<shared_ptr<Xform> > boneXforms;
    carray2<float> vertexBoneWeights;
    carray2<int> vertexBoneIndex;

protected:
    Matrix4d _ComputeBoneXform(int bId, float time);
    Matrix4d _ComputeVertexXform(int face, int vertex, float time);
    Matrix4d _ComputeVertexXformFromIdx(int vertexIdx, float time);
};

class BlendedMeshShape : public DeformedMeshShape {
public:
	BlendedMeshShape() { }

    BlendedMeshShape(shared_ptr<carray<shared_ptr<MeshShape> > > poses, 
                     shared_ptr<carray<shared_ptr<Spline<float, float> > > > weights);

    virtual Intervalf ComputeAnimationInterval();

    virtual uint64_t NFaces() { return poseMeshes[0]->NFaces(); }
    virtual uint64_t NVertices() { return poseMeshes[0]->NVertices(); }

    virtual bool HasNormal() { return poseMeshes[0]->HasNormal(); }
    virtual bool HasUv() { return poseMeshes[0]->HasUv(); }

    virtual Vec3f Pos(int face, int vertex, float time);
    virtual Vec3f Normal(int face, int vertex, float time);
    virtual Vec2f Uv(int face, int vertex, float time);

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time);

	static string serialize_typename(); 
	virtual void serialize(Archive* a);

protected:
    carray<shared_ptr<MeshShape> > poseMeshes;
    carray<Spline<float, float> > poseWeights;
};

// linear interpolation for now
// also if times->size == 1, then use the time as interpolation value
// (useful for debugging!)
class KeyframedMeshShape : public DeformedMeshShape {
public:
    KeyframedMeshShape() { }

    KeyframedMeshShape(shared_ptr<carray<shared_ptr<MeshShape> > > poses, shared_ptr<carray<float> > times);
	
    virtual Intervalf ComputeAnimationInterval();

    uint64_t GetNPoses() { return poseMeshes.size(); }

    carray<shared_ptr<MeshShape> >* GetPoseMeshes() { return &poseMeshes; }
    carray<float>* GetPoseTimes() { return &poseTimes; }
    shared_ptr<MeshShape> GetPoseMesh(int i) { return poseMeshes[i]; }

    virtual uint64_t NFaces() { return poseMeshes[0]->NFaces(); }
    virtual uint64_t NVertices() { return poseMeshes[0]->NVertices(); }

    virtual bool HasNormal() { return poseMeshes[0]->HasNormal(); }
    virtual bool HasUv() { return poseMeshes[0]->HasUv(); }

    virtual Vec3f Pos(int face, int vertex, float time);
    virtual Vec3f Normal(int face, int vertex, float time);
    virtual Vec2f Uv(int face, int vertex, float time);

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time);

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    carray<shared_ptr<MeshShape> > poseMeshes;
    carray<float> poseTimes;

    void _Map(float time, float& t, int& k0, int& k1);
};

#endif
