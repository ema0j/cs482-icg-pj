#ifndef _RAY_LIST_ENGINE_H_
#define _RAY_LIST_ENGINE_H_

#include "rayEngine.h"
#include "rayIntersectionOp.h"
#include <vector>
#include <scene/xform.h>
#include <scene/shape_mesh.h>
#include <scene/intersectionMethods.h>
#include "rayListEngineData.h"

using std::vector;
class RayListEngineBuilder;

class ListEngineGroupNode
{
    friend class RayListEngineBuilder;
public:
    Intervalf   ValidInterval();
    bool        Intersect(const Ray& ray, Intersection* intersection);
    bool        IntersectAny(const Ray& ray);
    void        CollectStats(StatsManager& stats);
    Range3f     ComputeBoundingBox();
    float       ComputeAverageArea();
private:
    vector<RayEngine*>  _groups;
    Intervalf           _interval;
};


template<typename NormOp, typename UvOp>
struct ListEngineMeshNode
{
    friend class RayListEngineBuilder;
public:
    ListEngineMeshNode(Xform* xform, Material* material, shared_ptr<TriangleArray> data) : _xform(xform), _material(material), _ref(data), _data(data.get()) {}
    Intervalf ValidInterval() { return Intervalf::Invalid(); }
    bool RayThrough() { return false; }
    bool Intersect(const Ray& r, Intersection* isect)
    {
        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        float b1, b2;
        float t;
        float rayEpsilon;
        carray<Vec3i> &faces = _data->faces;
        carray<Vec3f> &pos = _data->positions;
        bool hit = false;
        DifferentialGeometry &dp = isect->dp;
        for(uint64_t f = 0; f < faces.size(); f ++) 
        {
            Vec3i &face = faces[f];
            Vec3f &v0 = pos[face[0]]; 
            Vec3f &v1 = pos[face[1]]; 
            Vec3f &v2 = pos[face[2]]; 
            if(IntersectTriangle(v0, v1, v2, ray, &t, &b1, &b2, &rayEpsilon))
            {
                if (!RayThrough())
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
        return hit;
    }
    bool IntersectAny(const Ray& r)
    {
        Matrix4d mi = _xform->GetInverseTransform(r.time);
        Ray ray = r;
        ray.Transform(mi);

        float t;
        float rayEpsilon;
        float b1, b2;
        carray<Vec3i> &faces = _data->faces;
        carray<Vec3f> &pos = _data->positions;
        for(uint32_t f = 0; f < faces.size(); f++)
        {
            if(IntersectTriangle(pos[faces[f][0]], pos[faces[f][1]], pos[faces[f][2]], ray, &t, &b1, &b2, &rayEpsilon))
            {
                if (!RayThrough())
                    return true;
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
        carray<Vec3i> &faces = _data->faces;
        carray<Vec3f> &pos = _data->positions;
        Range3f ret;
        for(uint32_t f = 0; f < faces.size(); f ++) 
            ret.Grow(ElementOperations::TriangleBoundingBox(pos[faces[f][0]], pos[faces[f][1]], pos[faces[f][2]]));

        return _xform->GetTransform(0.0f).TransformBBox(ret);
    }
    float ComputeAverageArea()
    {
        carray<Vec3i> &faces = _data->faces;
        carray<Vec3f> &pos = _data->positions;
        float area = 0;
        for(uint32_t f = 0; f < faces.size(); f ++) 
            area += ElementOperations::TriangleArea(pos[faces[f][0]], pos[faces[f][1]], pos[faces[f][2]]);
        return area;
    }
private:
    Xform                       *_xform;
    Material                    *_material;
    TriangleArray            *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<TriangleArray> _ref;   //Hold the obj ref to avoid deletion
};

struct ListEngineSphereNode
{
    friend class RayListEngineBuilder;
public:
    ListEngineSphereNode(Xform* xform, Material* material, shared_ptr<SphereArray> data) : _xform(xform), _material(material), _ref(data), _data(data.get()) {}
    Intervalf ValidInterval() { return Intervalf::Invalid(); }
    bool RayThrough() { return false; }
    bool Intersect(const Ray& r, Intersection* isect)
    {
        Matrix4d mi = _xform->GetInverseTransform(r.time);
        Ray ray = r;
        ray.Transform(mi);

        carray<float> &radiuss = _data->radius;
        carray<Vec3f> &centers = _data->centers;

        bool hit = false;
        float t;
        float rayEpsilon;
        DifferentialGeometry &dp = isect->dp;
        for (uint32_t s = 0; s < centers.size(); s++)
        {
            Vec3f &center = centers[s];
            float &radius = radiuss[s];
            if(IntersectSphere(center, radius, ray, &t, &rayEpsilon))
            {
                if (!RayThrough())
                {
                    hit = true;
                    dp.P = ray.Eval(t);
                    dp.N = dp.Ng = (dp.P - center).GetNormalized();
                    dp.uv[0] = (atan2f(dp.N[0], dp.N[1]) + (float)PI) / (float)(2*PI);
                    dp.uv[1] = acosf(dp.N[2]) / (float)PI;
                    dp.GenerateTuTv();
                    dp.st = dp.uv;
                    ray.tMax = t;
                }
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
        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        carray<float> &radius = _data->radius;
        carray<Vec3f> &centers = _data->centers;
        float t;
        float rayEpsilon;
        for (uint32_t s = 0; s < centers.size(); s++)
        {
            if(IntersectSphere(centers[s], radius[s], ray, &t, &rayEpsilon))
            {
                if (!RayThrough())
                    return true;
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
        carray<float> &radiuss = _data->radius;
        carray<Vec3f> &centers = _data->centers;
        Range3f ret = Range3f::Empty();
        for(uint32_t f = 0; f < centers.size(); f ++)
        {
            ret.Grow(ElementOperations::SphereBoundingBox(centers[f], radiuss[f]));
        }
        return _xform->GetTransform(0.0f).TransformBBox(ret);
    }

    float ComputeAverageArea()
    {
        carray<float> &radiuss = _data->radius;
        float area = 0;
        for(uint32_t f = 0; f < radiuss.size(); f ++)
        {
            area += ElementOperations::SphereArea(radiuss[f]);
        }
        return area;
    }
private:
    Xform                       *_xform;
    Material                    *_material;
    SphereArray              *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<SphereArray>   _ref;   //Hold the obj ref to avoid deletion
};

template<typename TangOp, typename UvOp, typename RadiusOp>
struct ListEngineSegmentNode
{
    friend class RayListEngineBuilder;
public:
    ListEngineSegmentNode(Xform* xform, Material* material, shared_ptr<SegmentArray> data) 
        : _xform(xform), _material(material), _ref(data), _data(data.get()) {}
    Intervalf ValidInterval() { return Intervalf::Invalid(); }
    bool RayThrough() { return false; }
    bool Intersect(const Ray& r, Intersection* isect)
    {
        Matrix4d mi = _xform->GetInverseTransform(r.time);
        Ray ray = r;
        ray.Transform(mi);

        bool hit = false;
        float t;
        float rayEpsilon;
        float u;
        DifferentialGeometry &dp = isect->dp;

        carray<Vec3f> &pos = _data->positions;
        carray<Vec2i> &segs = _data->segments;
        for (uint64_t s = 0; s < segs.size(); s++)
        {
            Vec2i &seg = segs[s];
            float radius = RadiusOp::ComputeRadius(_data->radius, _data, seg);
            Vec3f &v0 = pos[seg[0]];
            Vec3f &v1 = pos[seg[1]];

            if(IntersectHairSegment(v0, v1, _data->radius, ray, &t, &u, &rayEpsilon))
            {
                if (!RayThrough())
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
        Ray ray = r;
        ray.Transform(_xform->GetInverseTransform(r.time));

        float radius = _data->radius;
        carray<Vec3f> &pos = _data->positions;
        carray<Vec2i> &segs = _data->segments;
        float t;
        float rayEpsilon;
        float u;

        for (uint64_t s = 0; s < segs.size(); s++)
        {
            Vec2i &seg = segs[s];
            if(IntersectHairSegment(pos[seg[0]], pos[seg[1]], _data->radius, ray, &t, &u, &rayEpsilon))
            {
                if (!RayThrough())
                    return true;
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
        carray<Vec2i> &segs = _data->segments;
        carray<Vec3f> &pos = _data->positions;
        Range3f ret = Range3f::Empty();
        for(uint64_t s = 0; s < segs.size(); s++)
        {
            Vec2i seg = segs[s];
            ret.Grow(ElementOperations::CylinderBoundingBox(pos[seg[0]], pos[seg[1]], _data->radius));
        }
        return _xform->GetTransform(0.0f).TransformBBox(ret);
    }

    float ComputeAverageArea()
    {
        carray<Vec2i> &segs = _data->segments;
        carray<Vec3f> &pos = _data->positions;
        float area = 0;
        for(uint64_t s = 0; s < segs.size(); s++)
        {
            Vec2i seg = segs[s];
            area += ElementOperations::CylinderArea(pos[seg[0]], pos[seg[1]], _data->radius);
        }
        return area;
    }
private:
    Xform                       *_xform;
    Material                    *_material;
    SegmentArray             *_data;  //This cache the pointer to avoid evaluate share_ptr every time.
    shared_ptr<SegmentArray>  _ref;   //Hold the obj ref to avoid deletion
};

#endif // _RAY_LIST_ENGINE_H_

