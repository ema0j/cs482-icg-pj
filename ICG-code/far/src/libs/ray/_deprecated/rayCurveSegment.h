#ifndef _RAYCURVESEGMENT_H_
#define _RAYCURVESEGMENT_H_

#include "rayPrimitive.h"
#include <scene/intersectionMethods.h>
#include <scene/curve.h>

class RayCurveSegmentPrimitive : public RayShapeElementPrimitive {
public:
    RayCurveSegmentPrimitive(CurveShape* s, Material* m, int id) : RayShapeElementPrimitive(s,m) { 
        static StatsCounterVariable* curvesegments = StatsManager::GetVariable<StatsCounterVariable>("Ray Primitive", "Curve Segments");
        curvesegments->Increment();
        firstId = id;
    }

    // THIS COULD BE MADE QUITE A BIT BETTER !!!
    virtual Range3f ComputeBoundingBox(const Intervalf& time) {
        CurveShape* curve = (CurveShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return curve->ComputeBBoxElement(0, firstId/2);
#else
        Range3f ret;
        ret.Grow(curve->GetVertexPos(firstId+0));
        ret.Grow(curve->GetVertexPos(firstId+1));
        ret.Grow(curve->GetVertexPos(firstId+0) + Vec3f(curve->GetRadius()));
        ret.Grow(curve->GetVertexPos(firstId+1) + Vec3f(curve->GetRadius()));
        ret.Grow(curve->GetVertexPos(firstId+0) - Vec3f(curve->GetRadius()));
        ret.Grow(curve->GetVertexPos(firstId+1) - Vec3f(curve->GetRadius()));
        return ret;
#endif
    }

    virtual float ComputeAreaEstimate(const Intervalf& time) {
        CurveShape* curve = (CurveShape*)shape;
#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return curve->ComputeAreaElement(0, firstId/2);
#else
        return 2 * PIf * curve->GetRadius() * 
            (curve->GetVertexPos(firstId+1) - curve->GetVertexPos(firstId+0)).GetLength();
#endif
    }

    virtual bool Intersect(const Ray& ray, Intersection* intersection) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "CurveSegment Intersect Queries");
        rays->Increment();
#endif

        CurveShape* curve = (CurveShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        float t;
        bool hit = curve->IntersectElement(ray, t, intersection->dp, firstId/2);

        if(hit) {
            intersection->t = t;
            intersection->m = material;
        }
#else
        float t,u;
        bool hit = IntersectHairSegment(curve->GetVertexPos(firstId+0),
                                        curve->GetVertexPos(firstId+1),
                                        curve->GetRadius(),ray,&t,&u);

        if(hit) {
            // intesection
            intersection->t = t;
            intersection->dp.P = ray.Eval(t);
            intersection->dp.Ng = (curve->GetVertexPos(firstId+1) - curve->GetVertexPos(firstId+0)).GetNormalized();
            intersection->dp.uv = Vec2f(u,0.5f);
            if(curve->HasVertexTangent()) {
                intersection->dp.N = (curve->GetVertexTangent(firstId+0) * (1-u) + curve->GetVertexTangent(firstId+1) * u).GetNormalized(); // cheat that agrees with curve material
            } else {
                intersection->dp.N = intersection->dp.Ng; // cheat that agrees with curve material
            }
            intersection->dp.GenerateTuTv();
            if(curve->HasVertexTexCoord()) {
                intersection->dp.st = (curve->GetVertexTexCoord(firstId+0) * (1-u) + curve->GetVertexTexCoord(firstId+1)*u);
            } else {
                intersection->dp.st = intersection->dp.uv;
            }
            intersection->m = material;
        }
#endif

        return hit;
    }

    virtual bool Intersect(const Ray& ray) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "CurveSegment IntersectAny Queries");
        rays->Increment();
#endif

        CurveShape* curve = (CurveShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return curve->IntersectElement(ray, firstId/2);
#else
        float t, u;
        return IntersectHairSegment(
            curve->GetVertexPos(firstId+0), 
            curve->GetVertexPos(firstId+1), 
            curve->GetRadius(), 
            ray, &t, &u);
#endif
    }

protected:
    int                  firstId;
};

#endif
