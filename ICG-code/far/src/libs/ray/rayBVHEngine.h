#ifndef _RAY_BVH_ENGINE_H_
#define _RAY_BVH_ENGINE_H_

#include "rayEngine.h"
#include <vector>
#include <scene/xform.h>
#include <scene/shape_mesh.h>
#include <scene/intersectionMethods.h>
#include "rayBVHEngineData.h"

using std::vector;
class RayBVHEngineBuilder;

inline bool IntersectBVHBoundingBox(const Range3f &bounds, const Ray &ray,
    const Vec3f &invDir, const uint32_t dirIsNeg[3]) {
        // Check for ray intersection against $x$ and $y$ slabs
        float tmin =  (bounds[  dirIsNeg[0]].x - ray.E.x) * invDir.x;
        float tmax =  (bounds[1-dirIsNeg[0]].x - ray.E.x) * invDir.x;
        float tymin = (bounds[  dirIsNeg[1]].y - ray.E.y) * invDir.y;
        float tymax = (bounds[1-dirIsNeg[1]].y - ray.E.y) * invDir.y;
        if ((tmin > tymax) || (tymin > tmax))
            return false;
        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        // Check for ray intersection against $z$ slab
        float tzmin = (bounds[  dirIsNeg[2]].z - ray.E.z) * invDir.z;
        float tzmax = (bounds[1-dirIsNeg[2]].z - ray.E.z) * invDir.z;
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;
        return (tmin < ray.tMax) && (tmax > ray.tMin);
}


class BvhEngineGroupNode
{
    friend class RayBVHEngineBuilder;
public:
    BvhEngineGroupNode(shared_ptr<GroupBvh> data) : _ref(data), _data(data.get()) {}
    Intervalf               ValidInterval();
    bool                    Intersect(const Ray& ray, Intersection* intersection);
    bool                    IntersectAny(const Ray& ray);
    void                    CollectStats(StatsManager& stats);
    Range3f                 ComputeBoundingBox();
    float                   ComputeAverageArea();
    GroupBvh                *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<GroupBvh>    _ref;   //Hold the obj ref to avoid deletion
};

template<typename NormOp, typename UvOp>
struct BvhEngineMeshNode
{
    friend class RayBVHEngineBuilder;
public:
    BvhEngineMeshNode() : _xform(NULL), _material(NULL) {}
    BvhEngineMeshNode(Xform* xform, Material* material, shared_ptr<TriangleBvh> data) : _xform(xform), _material(material), _ref(data), _data(data.get()) {}
    Intervalf ValidInterval() { return Intervalf::Invalid(); }
    bool Intersect(const Ray& r, Intersection* isect)
    {
        if (!_data->bvhNodes.size()) 
            return false;

        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        carray<Vec3i> &faces = _data->faces;
        carray<Vec3f> &pos = _data->positions;
        const vector<uint32_t> &ordered = _data->ordered;

        float t = ray.tMax;
        float rayEpsilon = ray.tMin;

        bool hit = false;
        Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
        uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
        // Follow ray through BVH nodes to find primitive intersections
        uint32_t todoOffset = 0, nodeNum = 0;
        uint32_t todo[64];
        while (true) {
            const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
            // Check ray against BVH node
            if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
            {
                if (node->nPrimitives > 0) 
                {
                    float b1, b2;
                    DifferentialGeometry &dp = isect->dp;
                    // Intersect ray with primitives in leaf BVH node
                    for (uint32_t i = 0; i < node->nPrimitives; ++i)
                    {
                        uint32_t f = ordered[node->primitivesOffset+i];
                        Vec3i &face = faces[f];
                        Vec3f &v0 = pos[face[0]]; 
                        Vec3f &v1 = pos[face[1]]; 
                        Vec3f &v2 = pos[face[2]]; 
                        if(IntersectTriangle(v0, v1, v2, ray, &t, &b1, &b2, &rayEpsilon))
                        {
                            if (_material->CheckAlpha(Vec2f(b1, b2), 0.5f))
                            {
                                hit = true;
                                dp.P = ray.Eval(t);
                                dp.uv = Vec2f(b1, b2);
                                dp.Ng = ElementOperations::TriangleNormal(v0,v1,v2);
                                dp.N = NormOp::ComputeNormal(_data, dp, b1, b2, face);
                                dp.GenerateTuTv();
                                dp.st = UvOp::ComputeTexcoord(_data, dp, b1, b2, face);
                                ray.tMax = t;
                                isect->rayEpsilon = rayEpsilon;
                                isect->t = t;
                                isect->m = _material;
                                isect->Transform(_xform->GetTransform(ray.time));
                            }
                        }
                    }
                    if (todoOffset == 0) break;
                    nodeNum = todo[--todoOffset];
                }
                else {
                    // Put far BVH node on _todo_ stack, advance to near node
                    if (dirIsNeg[node->axis]) {
                        todo[todoOffset++] = nodeNum + 1;
                        nodeNum = node->secondChildOffset;
                    }
                    else {
                        todo[todoOffset++] = node->secondChildOffset;
                        nodeNum = nodeNum + 1;
                    }
                }
            }
            else {
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
        }
        return hit;
    }

    bool IntersectAny(const Ray& r)
    {
        if (!_data->bvhNodes.size()) 
            return false;

        Matrix4d mi = _xform->GetInverseTransform(r.time);
        Ray ray = r;
        ray.Transform(mi);

        float t  = r.tMax;
        float rayEpsilon = r.tMin;
        float b1, b2;
        carray<Vec3i> &faces = _data->faces;
        carray<Vec3f> &pos = _data->positions;
        const vector<uint32_t> &ordered = _data->ordered;

        Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
        uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
        uint32_t todo[64];
        uint32_t todoOffset = 0, nodeNum = 0;
        while (true) {
            const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
            if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
            {
                // Process BVH node _node_ for traversal
                if (node->nPrimitives > 0) {
                    for (uint32_t i = 0; i < node->nPrimitives; ++i) 
                    {
                        uint32_t f = ordered[node->primitivesOffset+i];
                        if(IntersectTriangle(pos[faces[f][0]], pos[faces[f][1]], pos[faces[f][2]], ray, &t, &b1, &b2, &rayEpsilon))
                        {
                            if (_material->IntersectOption() & IOPT_IGNORE_SHADOW)
                                continue;

                            if(_material->CheckAlpha(Vec2f(b1, b2), 0.5f))
                                return true;
                        }
                    }
                    if (todoOffset == 0) break;
                    nodeNum = todo[--todoOffset];
                }
                else {
                    if (dirIsNeg[node->axis]) {
                        /// second child first
                        todo[todoOffset++] = nodeNum + 1;
                        nodeNum = node->secondChildOffset;
                    }
                    else {
                        todo[todoOffset++] = node->secondChildOffset;
                        nodeNum = nodeNum + 1;
                    }
                }
            }
            else {
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
        }
        return false;
    }

    void CollectStats(StatsManager& stats)
    {
        StatsCounterVariable* primitives = stats.GetVariable<StatsCounterVariable>("Ray", "Primitives");
        primitives->Increment(_data->faces.size());
    }

    Range3f ComputeBoundingBox()
    {
        return _data->bvhNodes.size() ? _xform->GetTransform(0.0f).TransformBBox(_data->bvhNodes[0].bounds) : Range3f();
    }

    float ComputeAverageArea() {  throw std::exception(); }
private:
    Xform                       *_xform;
    Material                    *_material;
    TriangleBvh                 *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<TriangleBvh>      _ref;   //Hold the obj ref to avoid deletion
};

struct BvhEngineSphereNode
{
    friend class RayBVHEngineBuilder;
public:
    BvhEngineSphereNode() : _xform(NULL), _material(NULL) {}
    BvhEngineSphereNode(Xform* xform, Material* material, shared_ptr<SphereBvh> data) : _xform(xform), _material(material), _ref(data), _data(data.get()) {}
    Intervalf ValidInterval() { return Intervalf::Invalid(); }
    bool Intersect(const Ray& r, Intersection* isect)
    {
        if (!_data->bvhNodes.size()) 
            return false;

        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        carray<float> &radiuss = _data->radius;
        carray<Vec3f> &centers = _data->centers;
        const vector<uint32_t> &ordered = _data->ordered;


        float t = ray.tMax;
        float rayEpsilon = ray.tMin;
        DifferentialGeometry &dp = isect->dp;
        bool hit = false;

        Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
        uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
        // Follow ray through BVH nodes to find primitive intersections
        uint32_t todoOffset = 0, nodeNum = 0;
        uint32_t todo[64];
        while (true) {
            const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
            // Check ray against BVH node
            if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
            {
                if (node->nPrimitives > 0) 
                {
                    // Intersect ray with primitives in leaf BVH node
                    for (uint32_t i = 0; i < node->nPrimitives; ++i)
                    {
                        uint32_t s = ordered[node->primitivesOffset+i];
                        Vec3f &center = centers[s];
                        float &radius = radiuss[s];
                        if(IntersectSphere(center, radius, ray, &t, &rayEpsilon))
                        {
                            Vec2f uv;
                            Vec3f P = ray.Eval(t);
                            Vec3f N = (P - center).GetNormalized();
                            uv[0] = (atan2f(N[0], N[1]) + (float)PI) / (float)(2*PI);
                            uv[1] = acosf(N[2]) / (float)PI;
                            if (_material->CheckAlpha(uv, 0.5f))
                            {
                                hit = true;
                                dp.P = P;
                                dp.N = dp.Ng = N;
                                dp.uv = uv;
                                dp.GenerateTuTv();
                                dp.st = dp.uv;
                                ray.tMax = t;
                            }
                        }
                    }
                    if (todoOffset == 0) break;
                    nodeNum = todo[--todoOffset];
                }
                else {
                    // Put far BVH node on _todo_ stack, advance to near node
                    if (dirIsNeg[node->axis]) {
                        todo[todoOffset++] = nodeNum + 1;
                        nodeNum = node->secondChildOffset;
                    }
                    else {
                        todo[todoOffset++] = node->secondChildOffset;
                        nodeNum = nodeNum + 1;
                    }
                }
            }
            else {
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
        }
        if(hit)
        {
            isect->rayEpsilon = rayEpsilon;
            isect->t = t;
            isect->m = _material;
            isect->Transform(_xform->GetTransform(ray.time));
        }
        return hit;
    }

    bool IntersectAny(const Ray& r)
    {
        if (!_data->bvhNodes.size()) 
            return false;

        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        float t  = r.tMax;
        float rayEpsilon = r.tMin;
        carray<float> &radiuss = _data->radius;
        carray<Vec3f> &centers = _data->centers;
        const vector<uint32_t> &ordered = _data->ordered;

        Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
        uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
        uint32_t todo[64];
        uint32_t todoOffset = 0, nodeNum = 0;
        while (true) {
            const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
            if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
            {
                // Process BVH node _node_ for traversal
                if (node->nPrimitives > 0) {
                    for (uint32_t i = 0; i < node->nPrimitives; ++i) 
                    {
                        uint32_t s = ordered[node->primitivesOffset+i];
                        if(IntersectSphere(centers[s], radiuss[s], ray, &t, &rayEpsilon))
                        {
                            if(_material->IntersectOption() & IOPT_IGNORE_SHADOW)
                                continue;

                            if (_material->HasAlphaMap())
                            {
                                Vec3f P = ray.Eval(t);
                                Vec3f N = (P - centers[s]).GetNormalized();
                                Vec2f uv;
                                uv[0] = (atan2f(N[0], N[1]) + (float)PI) / (float)(2*PI);
                                uv[1] = acosf(N[2]) / (float)PI;

                                if (_material->CheckAlpha(uv, 0.5f))
                                    return true;
                            }
                            else
                                return true;
                        }
                    }
                    if (todoOffset == 0) break;
                    nodeNum = todo[--todoOffset];
                }
                else {
                    if (dirIsNeg[node->axis]) {
                        /// second child first
                        todo[todoOffset++] = nodeNum + 1;
                        nodeNum = node->secondChildOffset;
                    }
                    else {
                        todo[todoOffset++] = node->secondChildOffset;
                        nodeNum = nodeNum + 1;
                    }
                }
            }
            else {
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
        }
        return false;
    }

    void CollectStats(StatsManager& stats)
    {
        StatsCounterVariable* primitives = stats.GetVariable<StatsCounterVariable>("Ray", "Primitives");
        primitives->Increment(_data->centers.size());
    }

    Range3f ComputeBoundingBox()
    {
        return _data->bvhNodes.size() ? _xform->GetTransform(0.0f).TransformBBox(_data->bvhNodes[0].bounds) : Range3f();
    }

    float ComputeAverageArea() {  throw std::exception(); }
private:
    Xform                       *_xform;
    Material                    *_material;
    SphereBvh                   *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<SphereBvh>        _ref;   //Hold the obj ref to avoid deletion
};

template<typename TangOp, typename UvOp, typename RadiusOp>
struct BvhEngineSegmentNode
{
    friend class RayBVHEngineBuilder;
public:
    BvhEngineSegmentNode() : _xform(NULL), _material(NULL) {}
    BvhEngineSegmentNode(Xform* xform, Material* material, shared_ptr<SegmentBvh> data) : _xform(xform), _material(material), _ref(data), _data(data.get()) {}
    Intervalf ValidInterval() { return Intervalf::Invalid(); }
    bool Intersect(const Ray& r, Intersection* isect)
    {
        if (!_data->bvhNodes.size()) 
            return false;

        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        carray<Vec3f> &pos = _data->positions;
        carray<Vec2i> &segs = _data->segments;
        const vector<uint32_t> &ordered = _data->ordered;

        float t = ray.tMax;
        float rayEpsilon = ray.tMin;
        DifferentialGeometry &dp = isect->dp;
        bool hit = false;
        Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
        uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
        // Follow ray through BVH nodes to find primitive intersections
        uint32_t todoOffset = 0, nodeNum = 0;
        uint32_t todo[64];
        while (true) {
            const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
            // Check ray against BVH node
            if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
            {
                if (node->nPrimitives > 0) 
                {
                    float u;
                    // Intersect ray with primitives in leaf BVH node
                    for (uint32_t i = 0; i < node->nPrimitives; ++i)
                    {
                        uint32_t s = ordered[node->primitivesOffset+i];
                        Vec2i &seg = segs[s];
                        float radius = RadiusOp::ComputeRadius(_data->radius, _data, seg);
                        Vec3f &v0 = pos[seg[0]];
                        Vec3f &v1 = pos[seg[1]];
                        if(IntersectHairSegment(v0, v1, _data->radius, ray, &t, &u, &rayEpsilon))
                        {
                            if (_material->CheckAlpha(Vec2f(u, 0), 0.5f))
                            {
                                hit = true;
                                dp.P = ray.Eval(t);
                                dp.uv = Vec2f(u, 0);
                                dp.Ng = ElementOperations::SegmentTangent(v0, v1);
                                dp.N = TangOp::ComputeTangent(_data, dp, u, seg);
                                dp.GenerateTuTv();
                                dp.st = UvOp::ComputeTexcoord(_data, dp, u, seg);
                                ray.tMax = t;
                            }
                        }
                    }
                    if (todoOffset == 0) break;
                    nodeNum = todo[--todoOffset];
                }
                else {
                    // Put far BVH node on _todo_ stack, advance to near node
                    if (dirIsNeg[node->axis]) {
                        todo[todoOffset++] = nodeNum + 1;
                        nodeNum = node->secondChildOffset;
                    }
                    else {
                        todo[todoOffset++] = node->secondChildOffset;
                        nodeNum = nodeNum + 1;
                    }
                }
            }
            else {
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
        }
        if(hit)
        {
            isect->rayEpsilon = rayEpsilon;
            isect->t = t;
            isect->m = _material;
            isect->Transform(_xform->GetTransform(ray.time));
        }
        return hit;
    }

    bool IntersectAny(const Ray& r)
    {
        if (!_data->bvhNodes.size()) 
            return false;

        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        float t  = r.tMax;
        float rayEpsilon = r.tMin;
        carray<Vec3f> &pos = _data->positions;
        carray<Vec2i> &segs = _data->segments;
        float u;
        const vector<uint32_t> &ordered = _data->ordered;

        Vec3f invDir(1.f / ray.D.x, 1.f / ray.D.y, 1.f / ray.D.z);
        uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
        uint32_t todo[64];
        uint32_t todoOffset = 0, nodeNum = 0;
        while (true) {
            const LinearBVHNode *node = &_data->bvhNodes[nodeNum];
            if (IntersectBVHBoundingBox(node->bounds, ray, invDir, dirIsNeg)) 
            {
                // Process BVH node _node_ for traversal
                if (node->nPrimitives > 0) {
                    for (uint32_t i = 0; i < node->nPrimitives; ++i) 
                    {
                        uint32_t s = ordered[node->primitivesOffset+i];
                        Vec2i &seg = segs[s];
                        if(IntersectHairSegment(pos[seg[0]], pos[seg[1]], _data->radius, ray, &t, &u, &rayEpsilon))
                        {
                            if(_material->IntersectOption() & IOPT_IGNORE_SHADOW)
                                continue;
                            if (_material->CheckAlpha(Vec2f(u, 0), 0.5f))
                                return true;
                        }
                    }
                    if (todoOffset == 0) break;
                    nodeNum = todo[--todoOffset];
                }
                else {
                    if (dirIsNeg[node->axis]) {
                        /// second child first
                        todo[todoOffset++] = nodeNum + 1;
                        nodeNum = node->secondChildOffset;
                    }
                    else {
                        todo[todoOffset++] = node->secondChildOffset;
                        nodeNum = nodeNum + 1;
                    }
                }
            }
            else {
                if (todoOffset == 0) break;
                nodeNum = todo[--todoOffset];
            }
        }
        return false;
    }

    void CollectStats(StatsManager& stats)
    {
        StatsCounterVariable* primitives = stats.GetVariable<StatsCounterVariable>("Ray", "Primitives");
        primitives->Increment(_data->segments.size());
    }

    Range3f ComputeBoundingBox()
    {
        return _data->bvhNodes.size() ? _xform->GetTransform(0.0f).TransformBBox(_data->bvhNodes[0].bounds) : Range3f();
    }

    float ComputeAverageArea() {  throw std::exception(); }
private:
    Xform                       *_xform;
    Material                    *_material;
    SegmentBvh                  *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<SegmentBvh>       _ref;   //Hold the obj ref to avoid deletion
};


#endif // _RAY_BVH_ENGINE_H_

