#include "shape_deformedmesh.h"
#include "intersectionMethods.h"

void DeformedMeshShape::CollectStats(StatsManager& stats) {
    StatsCounterVariable* meshes = stats.GetVariable<StatsCounterVariable>("Shapes", "Deformed Meshes");
    StatsCounterVariable* faces = stats.GetVariable<StatsCounterVariable>("Shapes", "Deformed Mesh Faces");
    StatsCounterVariable* vertices = stats.GetVariable<StatsCounterVariable>("Shapes", "Deformed Mesh Triangle Vertices");
    StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Shapes", "Memory");

    meshes->Increment();
    faces->Increment(NFaces());
    vertices->Increment(NVertices());
    memory->Increment(GetMemoryAllocatedStats());
}

void DeformedMeshShape::Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples) {
    if(time.IsEmpty()) { Tesselate(cache, time.Middle()); return; }

    if(!cache->SupportTimeSampledTriangles()) { cerr << "cannot sample primitive" << endl; return; }

    // sample geometry
    carray<shared_ptr<TriangleTesselationCache> > timeCaches(timeSamples);
    for(int t = 0; t < timeSamples; t ++) {
        timeCaches[t] = shared_ptr<TriangleTesselationCache>(new TriangleTesselationCache());
        float timeInstant = time.Get((float)t / (timeSamples-1));
        Tesselate(timeCaches[t], timeInstant);
    }
    
    // pack geometry
    tarray<Vec3f> posArray(timeCaches[0]->Pos().size(), timeSamples, time);
    tarray<Vec3f> normalArray(timeCaches[0]->Normal().size(), timeSamples, time);
    tarray<Vec2f> uvArray(timeCaches[0]->Uv().size(), timeSamples, time);
    for(int t = 0; t < timeSamples; t ++) {
        posArray.setAtTime(t, timeCaches[t]->Pos());
        normalArray.setAtTime(t, timeCaches[t]->Normal());
        uvArray.setAtTime(t, timeCaches[t]->Uv());
    }

    // send them out
    cache->AddTimeSampledTriangles(posArray, normalArray, uvArray, timeCaches[0]->Triangles());
}

float DeformedMeshShape::ComputeArea(float time) {
    float area = 0;
    for(int i = 0; i < NFaces(); i ++) area += ElementOperations::TriangleArea(Pos(i,0,time),Pos(i,1,time),Pos(i,2,time));
    return area;
}

Range3f DeformedMeshShape::ComputeBoundingBox(float time) {
    Range3f ret;
    for(uint32_t i = 0; i < NFaces(); i ++) ret.Grow(ElementOperations::TriangleBoundingBox(Pos(i,0,time),Pos(i,1,time),Pos(i,2,time)));
    return ret;
}

uint64_t DeformedMeshShape::GetMemoryAllocatedStats() { printf("Memory stats for deformed meshes not done yet\n"); return 0; }

template<class T>
inline void _reccopy(carray2<T>& dst, shared_ptr<carray< shared_ptr<carray<T> > > > src) {
	dst.resize(src->at(0)->size(), src->size());
	for(uint32_t i = 0; i < dst.width(); i ++) {
		for(uint32_t j = 0; j < dst.height(); j ++) {
			dst.at(i,j) = src->at(j)->at(i);
		}
	}
}

SkinnedMeshShape::SkinnedMeshShape(shared_ptr<MeshShape> refPose, shared_ptr<carray< shared_ptr<carray<float> > > > boneWeights,
    shared_ptr<carray< shared_ptr< carray<int> > > > boneIndex, shared_ptr<carray<shared_ptr<Xform> > > bonesAnim) {
    refPoseMesh = refPose;
    _reccopy(vertexBoneWeights, boneWeights);
    _reccopy(vertexBoneIndex, boneIndex);
    boneXforms = *bonesAnim;
}

Intervalf SkinnedMeshShape::ComputeAnimationInterval() {
    Intervalf ret = Intervalf::Invalid();
    for(uint64_t b = 0; b < boneXforms.size(); b ++) {
        ret.Include(boneXforms[b]->ComputeAnimationInterval());
    }
    return ret;
}

Vec3f SkinnedMeshShape::Pos(int face, int vertex, float time) {
    Matrix4d xform = _ComputeVertexXform(face, vertex, time);
    return xform.TransformPoint(refPoseMesh->Pos(face, vertex));
}
Vec3f SkinnedMeshShape::Normal(int face, int vertex, float time) {
    Matrix4d xform = _ComputeVertexXform(face, vertex, time);
    return xform.TransformNormalNoAdjoint(refPoseMesh->Normal(face, vertex));
}

Vec2f SkinnedMeshShape::Uv(int face, int vertex, float time) { return refPoseMesh->Uv(face, vertex); }

void SkinnedMeshShape::Tesselate(shared_ptr<TesselationCache> cache, float time) {
    carray<Vec3f> posArray(refPoseMesh->NVertices());
    carray<Vec3f> normalArray(refPoseMesh->NVertices());

    for(uint32_t i = 0; i < posArray.size(); i ++) {
        Matrix4d xform = _ComputeVertexXformFromIdx(i, time);
        posArray[i] = xform.TransformPoint(refPoseMesh->PosArray()[i]);
        normalArray[i] = xform.TransformNormalNoAdjoint(refPoseMesh->NormalArray()[i]);
    }

    cache->AddTriangles(posArray, normalArray, refPoseMesh->UvArray(), refPoseMesh->FaceArray());
}

string SkinnedMeshShape::serialize_typename() { return "SkinnedMeshShape"; } 

void SkinnedMeshShape::serialize(Archive* a) {
	a->member("refPose", refPoseMesh);
	a->member("boneXforms", boneXforms);
	a->member("boneWeights", vertexBoneWeights);
	a->member("boneIdx", vertexBoneIndex);
}
	
Matrix4d SkinnedMeshShape::_ComputeBoneXform(int bId, float time) {
    return boneXforms[bId]->GetTransform(time);
}

Matrix4d SkinnedMeshShape::_ComputeVertexXform(int face, int vertex, float time) {
    return _ComputeVertexXformFromIdx(refPoseMesh->VertexIndex(face, vertex), time);
}

Matrix4d SkinnedMeshShape::_ComputeVertexXformFromIdx(int vertexIdx, float time) {
    Matrix4d xf = Matrix4d::Zero();
    for(uint32_t vb = 0; vb < vertexBoneIndex.width(); vb ++) {
        int refBId = vertexBoneIndex.at(vb, vertexIdx);
        xf +=  _ComputeBoneXform(refBId, time) *
            vertexBoneWeights.at(vb, vertexIdx);
    }
    return xf;
}

BlendedMeshShape::BlendedMeshShape(shared_ptr<carray<shared_ptr<MeshShape> > > poses, shared_ptr<carray<shared_ptr<Spline<float, float> > > > weights) {
    poseMeshes = *poses;

	poseWeights.resize(weights->size());
    for(uint32_t i = 0; i < weights->size(); i ++) {
		poseWeights[i] = *weights->at(i);
	}
}

Intervalf BlendedMeshShape::ComputeAnimationInterval() {
    Intervalf ret = Intervalf::Invalid();
    for(uint32_t b = 0; b < poseWeights.size(); b ++) {
        ret.Include(poseWeights[b].GetTimesInterval());
    }
    return ret;
}

Vec3f BlendedMeshShape::Pos(int face, int vertex, float time) {
    Vec3f ret = Vec3f::Zero();
    for(uint32_t i = 0; i < poseMeshes.size(); i ++) {
        float w = poseWeights[i].Eval(time);
        Vec3f v = poseMeshes[i]->Pos(face, vertex);
        ret += v * w;
    }
    return ret;
}
Vec3f BlendedMeshShape::Normal(int face, int vertex, float time) {
    Vec3f ret = Vec3f::Zero();
    for(uint32_t i = 0; i < poseMeshes.size(); i ++) {
        float w = poseWeights[i].Eval(time);
        Vec3f n = poseMeshes[i]->Normal(face, vertex);
        ret += n * w;
    }
    ret.Normalize();
    return ret;
}
Vec2f BlendedMeshShape::Uv(int face, int vertex, float time) {
    Vec2f ret = Vec2f::Zero();
    for(uint32_t i = 0; i < poseMeshes.size(); i ++) {
        float w = poseWeights[i].Eval(time);
        Vec2f uv = poseMeshes[i]->Uv(face, vertex);
        ret += uv * w;
    }
    return ret;
}

void BlendedMeshShape::Tesselate(shared_ptr<TesselationCache> cache, float time) {
    carray<Vec3f> posArray(poseMeshes[0]->NVertices());
    carray<Vec3f> normalArray(poseMeshes[0]->NVertices());
    carray<Vec2f> uvArray(poseMeshes[0]->NVertices());

    posArray.set(0);
    normalArray.set(0);
    uvArray.set(0);
    for(uint32_t i = 0; i < poseMeshes.size(); i ++) {
        float w = poseWeights[i].Eval(time);
        for(uint32_t j = 0; j < posArray.size(); j ++) {
            posArray[j] += poseMeshes[i]->PosArray()[j] * w;
            normalArray[j] += poseMeshes[i]->NormalArray()[j] * w;
            uvArray[j] += poseMeshes[i]->UvArray()[j] * w;
        }
    }
    for(uint32_t j = 0; j < normalArray.size(); j ++) {
        normalArray[j].Normalize();
    }

    cache->AddTriangles(posArray, normalArray, uvArray, poseMeshes[0]->FaceArray());
}

string BlendedMeshShape::serialize_typename() { return "BlendedMeshShape"; } 

void BlendedMeshShape::serialize(Archive* a) {
	a->member("poses", poseMeshes);
	a->member("weights", poseWeights);
}

KeyframedMeshShape::KeyframedMeshShape(shared_ptr<carray<shared_ptr<MeshShape> > > poses, shared_ptr<carray<float> > times) {
    poseMeshes = *poses;
    poseTimes = *times;
} 

Intervalf KeyframedMeshShape::ComputeAnimationInterval() {
    return Intervalf(poseTimes[0], poseTimes[poseTimes.size()-1]);
}

Vec3f KeyframedMeshShape::Pos(int face, int vertex, float time) {
    float t; int k0,k1; _Map(time, t, k0, k1);
    return poseMeshes[k0]->Pos(face, vertex)*(1-t) +
        poseMeshes[k1]->Pos(face, vertex)*t;
}
Vec3f KeyframedMeshShape::Normal(int face, int vertex, float time) {
    float t; int k0,k1; _Map(time, t, k0, k1);
    return (poseMeshes[k0]->Normal(face, vertex)*(1-t) +
        poseMeshes[k1]->Normal(face, vertex)*t).GetNormalized();
}
Vec2f KeyframedMeshShape::Uv(int face, int vertex, float time) {
    float t; int k0,k1; _Map(time, t, k0, k1);
    return poseMeshes[k0]->Uv(face, vertex)*(1-t) +
        poseMeshes[k1]->Uv(face, vertex)*t;
}

void KeyframedMeshShape::Tesselate(shared_ptr<TesselationCache> cache, float time) {
    carray<Vec3f> posArray(poseMeshes[0]->PosArray().size());
    carray<Vec3f> normalArray(poseMeshes[0]->NormalArray().size());
    carray<Vec2f> uvArray(poseMeshes[0]->UvArray().size());

    float t; int k0,k1; _Map(time, t, k0, k1);
    for(int i = 0; i < posArray.size(); i ++) {
        posArray[i] = poseMeshes[k0]->PosArray()[i]*(1-t) + poseMeshes[k1]->PosArray()[i]*t;
        if(!normalArray.empty()) normalArray[i] = (poseMeshes[k0]->NormalArray()[i]*(1-t) + poseMeshes[k1]->NormalArray()[i]*t).GetNormalized();
        if(!uvArray.empty()) uvArray[i] = poseMeshes[k0]->UvArray()[i]*(1-t) + poseMeshes[k1]->UvArray()[i]*t;
    }

    cache->AddTriangles(posArray, normalArray, uvArray, poseMeshes[0]->FaceArray());
}

string KeyframedMeshShape::serialize_typename() { return "KeyframedMeshShape"; } 

void KeyframedMeshShape::serialize(Archive* a) {
	a->member("poses", poseMeshes);
	a->member("times", poseTimes);
}

void KeyframedMeshShape::_Map(float time, float& t, int& k0, int& k1) {
    if(poseTimes.size() == 1) { t = poseTimes[0]; k0 = 0; k1 = 1; }
    else SplineEvalInterval(poseTimes,time,t,k0,k1);
}
