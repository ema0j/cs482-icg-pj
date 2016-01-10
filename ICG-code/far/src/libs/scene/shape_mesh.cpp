#include "shape_mesh.h"
#include "intersectionMethods.h"
#include "xform.h"

MeshShape::MeshShape(shared_ptr<carray<Vec3f> > pos, shared_ptr<carray<Vec3f> > normal, shared_ptr<carray<Vec2f> > texcoord, shared_ptr<carray<int> > idxs) {
    posArray = *pos;
    faceArray.resize(idxs->size()/3);
    for(int i = 0; i < faceArray.size(); i ++) { faceArray[i].x = (*idxs)[i*3+0]; faceArray[i].y = (*idxs)[i*3+1]; faceArray[i].z = (*idxs)[i*3+2]; }

    if(normal) normalArray = *normal;
    if(texcoord) uvArray = *texcoord;
}

MeshShape::MeshShape(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& texcoord, const carray<Vec3i>& idxs) {
    posArray = pos;
    faceArray = idxs;
    normalArray = normal;
    uvArray = texcoord;
}

void MeshShape::CollectStats(StatsManager& stats) {
    StatsCounterVariable* meshes = stats.GetVariable<StatsCounterVariable>("Shapes", "Meshes");
    StatsCounterVariable* faces = stats.GetVariable<StatsCounterVariable>("Shapes", "Mesh Triangles");
    StatsCounterVariable* vertices = stats.GetVariable<StatsCounterVariable>("Shapes", "Mesh Triangle Vertices");
    StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Shapes", "Memory");

    meshes->Increment();
    faces->Increment(NFaces());
    vertices->Increment(NVertices());
    memory->Increment(sizeof(*this) +
        posArray.getMemoryAllocated() +
        faceArray.getMemoryAllocated() +
        ((HasNormal())?normalArray.getMemoryAllocated():0) +
        ((HasUv())?uvArray.getMemoryAllocated():0));
}

bool MeshShape::ApplyXform(shared_ptr<Xform> xform) {
    if(!xform->IsStatic()) return false;
    if(xform->IsIdentity()) return false;

    Matrix4d xformMat = xform->GetTransform(0);
    for(uint32_t i = 0; i < posArray.size(); i ++) {
        posArray[i] = xformMat.TransformPoint(posArray[i]);
        if(HasNormal()) xformMat.TransformNormalArrayNoAdjoint(normalArray);
    }

    return true;
}

void MeshShape::Tesselate(shared_ptr<TesselationCache> context, float time) {
    context->AddTriangles(posArray, normalArray, uvArray, faceArray);
}

float MeshShape::ComputeArea(float time) {
    float area = 0;
    for(uint32_t i = 0; i < NFaces(); i ++) area += ElementOperations::TriangleArea(Pos(i,0),Pos(i,1),Pos(i,2));
    return area;
}

Range3f MeshShape::ComputeBoundingBox(float time) {
    Range3f ret;
    for(uint32_t i = 0; i < NFaces(); i ++) ret.Grow(ElementOperations::TriangleBoundingBox(Pos(i,0),Pos(i,1),Pos(i,2)));
    return ret;
}

string MeshShape::serialize_typename() { return "MeshShape"; } 

void MeshShape::serialize(Archive* a) {
    a->member("pos", posArray);
    a->optional("normal", normalArray, carray<Vec3f>());
    a->optional("uv", uvArray, carray<Vec2f>());
    a->member("triangles", faceArray);
}

bool MeshShape::Intersect(const Ray& ray) {
    float t;
    float b1, b2, rayEpsilon;
    for(uint32_t i = 0; i < NFaces(); i ++) 
    {
        if(IntersectTriangle(Pos(i,0),Pos(i,1),Pos(i,2), ray, &t, &b1, &b2, &rayEpsilon))
            return true;
    }
    return false;
}

bool MeshShape::Intersect(const Ray& r, float& t, DifferentialGeometry& dp) {
    Ray ray = r;
    float b1, b2, rayEpsilon;
    for(int i = 0; i < NFaces(); i ++) 
    {
        const Vec3f &v0 = Pos(i, 0);
        const Vec3f &v1 = Pos(i, 1);
        const Vec3f &v2 = Pos(i, 2);
        if(IntersectTriangle(v0, v1, v2, ray, &t, &b1, &b2, &rayEpsilon))
        {
            dp.P = ray.Eval(t);
            dp.uv = Vec2f(b1,b2);
            if(normalArray.empty()) {
                dp.N = dp.Ng = ElementOperations::TriangleNormal(v0,v1,v2);
            } else {
                const Vec3f &n0 = Normal(i, 0);
                const Vec3f &n1 = Normal(i, 1);
                const Vec3f &n2 = Normal(i, 2);

                dp.Ng = ElementOperations::TriangleNormal(v0,v1,v2);
                dp.N = (n0 * (1-b1-b2) + n1 * b1 + n2 * b2).GetNormalized();
            }
            dp.GenerateTuTv();

            // shading
            if(uvArray.empty()) {
                dp.st = dp.uv;
            } else {
                const Vec2f &uv0 = Uv(i, 0);
                const Vec2f &uv1 = Uv(i, 1);
                const Vec2f &uv2 = Uv(i, 2);
                dp.st = (uv0 * (1-b1-b2) + uv1 * b1 + uv2 * b2);
            }
            ray.tMax = t;
        }
    }
    return false;
}

