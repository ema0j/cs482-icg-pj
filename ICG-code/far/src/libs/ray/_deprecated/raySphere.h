#ifndef _RAYSPHERE_H_
#define _RAYSPHERE_H_

#include "rayPrimitive.h"
#include <scene/intersectionMethods.h>
#include <scene/sphere.h>

class RaySpherePrimitive : public RayShapeElementPrimitive {
public:
    RaySpherePrimitive(SphereShape* s, Material* m) : RayShapeElementPrimitive(s,m) { 
        StatsCounterVariable* spheres = StatsManager::GetVariable<StatsCounterVariable>("Ray Primitive", "Spheres");
        spheres->Increment();
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time) {
        SphereShape* sphere = (SphereShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return sphere->ComputeBBox(0);
#else
        Range3f ret;
        ret.SetMin(sphere->GetCenter() - Vec3f(sphere->GetRadius()));
        ret.SetMax(sphere->GetCenter() + Vec3f(sphere->GetRadius()));
        return ret;
#endif
    }

    virtual float ComputeAreaEstimate(const Intervalf& time) {
        SphereShape* sphere = (SphereShape*)shape;
#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return sphere->ComputeArea(0);
#else
        return 4 * PIf * sphere->GetRadius();
#endif
    }

    virtual bool Intersect(const Ray& ray, Intersection* intersection) {
        SphereShape* sphere = (SphereShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        float t;
        bool hit = sphere->Intersect(ray, t, intersection->dp);

        if(hit) {
            intersection->t = t;
            intersection->m = material;
        }
#else
        float t;
        bool hit = IntersectSphere(sphere->GetCenter(), sphere->GetRadius(), ray, &t);

        if(hit) {
            intersection->t = t;
            intersection->dp.P = ray.Eval(t);
            intersection->dp.Ng = (intersection->dp.P - sphere->GetCenter()).GetNormalized();
            intersection->dp.N = (intersection->dp.P - sphere->GetCenter()).GetNormalized();
            intersection->dp.uv[0] = (atan2f(intersection->dp.N[0], intersection->dp.N[1]) + (float)PI) / (float)(2*PI);
            intersection->dp.uv[1] = acosf(intersection->dp.N[2]) / (float)PI;
            intersection->dp.GenerateTuTv();
            intersection->dp.st = intersection->dp.uv;
            intersection->m = material;
        }
#endif

        return hit;
    }

    virtual bool Intersect(const Ray& ray) {
        SphereShape* sphere = (SphereShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return sphere->Intersect(ray);
#else
        float t;
        return IntersectSphere(sphere->GetCenter(), sphere->GetRadius(), ray, &t);
#endif
    }

protected:
};

#endif
