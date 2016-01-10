#include "shape_sphere.h"
#include "intersectionMethods.h"

void SphereShape::CollectStats(StatsManager& stats) {
    StatsCounterVariable* spheres = stats.GetVariable<StatsCounterVariable>("Shapes", "Spheres");
    StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Shapes", "Memory");

    spheres->Increment();
    memory->Increment(sizeof(*this));
}

Vec3f SphereShape::InterpolatePos(const Vec2f& uv) {
    float cP = cosf(2*PIf*uv[0]);
    float sP = sinf(2*PIf*uv[0]);
    float cT = cosf(PIf*uv[1]);
    float sT = sinf(PIf*uv[1]);
    return Vec3f(cP*sT,sP*sT,cT) * radius + pos;
}

void SphereShape::InterpolatePosNormal(const Vec2f& uv, Vec3f* P, Vec3f* N) {
    float cP = cosf(2*PIf*uv[0]);
    float sP = sinf(2*PIf*uv[0]);
    float cT = cosf(PIf*uv[1]);
    float sT = sinf(PIf*uv[1]);
    *P = Vec3f(cP*sT,sP*sT,cT) * radius + pos;
    *N = Vec3f(cP*sT,sP*sT,cT).GetNormalized();
}

bool SphereShape::Intersect(const Ray& ray) {
    float t, rayEpsilon;
    return IntersectSphere(pos, radius, ray, &t, &rayEpsilon);
}

bool SphereShape::Intersect(const Ray& ray, float& t, float &rayEpsilon, DifferentialGeometry& dp) {
    bool hit = IntersectSphere(pos, radius, ray, &t, &rayEpsilon);

    if(hit) {
        dp.P = ray.Eval(t);
        dp.Ng = (dp.P - pos).GetNormalized();
        dp.N = (dp.P - pos).GetNormalized();
        dp.uv[0] = (atan2f(dp.N[0], dp.N[1]) + (float)PI) / (float)(2*PI);
        dp.uv[1] = acosf(dp.N[2]) / (float)PI;
        dp.GenerateTuTv();
        dp.st = dp.uv;
    }

    return hit;
}

float SphereShape::ComputeArea(float time) {
    return ElementOperations::SphereArea(radius);
}

Range3f SphereShape::ComputeBoundingBox(float time) {
    return ElementOperations::SphereBoundingBox(pos,radius);
}

bool ApplyXform(shared_ptr<Xform> xform) { return false; }

void SphereShape::Tesselate(shared_ptr<TesselationCache> cache, float time) {
    if(cache->SupportSpheres()) {
        cache->AddSpheres(carray<Vec3f>(1, pos), carray<float>(1, radius));
    } else {
        int level = 3;
        int nPhi = pow2(level+2);
        int nTheta = pow2(level+1);
        int nVertex = (nPhi+1)*(nTheta+1);
        int nTriangles = nPhi*nTheta*2;

        carray<Vec3f> pos(nVertex);
        carray<Vec3f> normal(nVertex);
        carray<Vec2f> uv(nVertex);
        carray<Vec3i> idx(nTriangles);

        for(int t = 0; t <= nTheta; t ++) {
            for(int p = 0; p <= nPhi; p ++) {
                int vIdx = p+t*(nPhi+1);
                float u = float(p) / float(nPhi);
                float v = float(t) / float(nTheta);
                float phi = u * 2 * PIf;
                float theta = v * PIf;
                Vec3f r = Vec3f(sin(phi)*sin(theta), cos(phi)*sin(theta), cos(theta));
                pos.at(vIdx) = this->pos + r * this->radius;
                normal.at(vIdx) = r;
                uv.at(vIdx) = Vec2f(u,v);
            }
        }

        int count = 0;
        for(int t = 0; t < nTheta; t ++) {
            for(int p = 0; p < nPhi; p ++) {
                idx.at(count)[0] = (p+0)+(t+0)*(nPhi+1); 
                idx.at(count)[1] = (p+1)+(t+0)*(nPhi+1); 
                idx.at(count)[2] = (p+1)+(t+1)*(nPhi+1); count ++;
                idx.at(count)[0] = (p+0)+(t+0)*(nPhi+1); 
                idx.at(count)[1] = (p+1)+(t+1)*(nPhi+1); 
                idx.at(count)[2] = (p+0)+(t+1)*(nPhi+1); count ++;
            }
        }

        cache->AddTriangles(pos,normal,uv,idx);
    }
}

string SphereShape::serialize_typename() { return "SphereShape"; } 

void SphereShape::serialize(Archive* a) {
	a->member("pos", pos);
	a->member("radius", radius);
}
