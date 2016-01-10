#ifndef _RAYSKINNEDMESHTRIANGLE_H_
#define _RAYSKINNEDMESHTRIANGLE_H_

#include "rayPrimitive.h"
#include <scene/intersectionMethods.h>
#include <scene/deformedmesh.h>

class RayDeformedMeshTrianglePrimitive : public RayShapeElementPrimitive {
public:
    RayDeformedMeshTrianglePrimitive(DeformedMeshShape* s, Material* m, int id) : RayShapeElementPrimitive(s,m) { 
        static StatsCounterVariable* deftriangles = StatsManager::GetVariable<StatsCounterVariable>("Ray Primitive", "Deformed Triangles");
        deftriangles->Increment();
        firstId = id; 
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time) {
        DeformedMeshShape* mesh = (DeformedMeshShape*)shape;

        Range3f ret;
        for(int i = 0; i < _DEFORMTIMESAMPLES; i ++) {
#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
            ret.Grow(mesh->ComputeBoundingBoxElement(i / (_DEFORMTIMESAMPLES-1.0f), firstId/3));
#else
            ret.Grow(mesh->GetVertexPos(firstId+0,time.Get(i / (_DEFORMTIMESAMPLES-1.0f))));
            ret.Grow(mesh->GetVertexPos(firstId+1,time.Get(i / (_DEFORMTIMESAMPLES-1.0f))));
            ret.Grow(mesh->GetVertexPos(firstId+2,time.Get(i / (_DEFORMTIMESAMPLES-1.0f))));
#endif
        }
        return ret;
    }

    virtual float ComputeAreaEstimate(const Intervalf& time) {
        DeformedMeshShape* mesh = (DeformedMeshShape*)shape;

        float ret = -1;
        for(int i = 0; i < _DEFORMTIMESAMPLES; i ++) {
            float t = time.Get(i / (_DEFORMTIMESAMPLES-1.0f));
#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
            ret = max(ret, mesh->ComputeAreaElement(t, i));
#else
            ret = max(ret, ((mesh->GetVertexPos(firstId+1,t)-mesh->GetVertexPos(firstId+0,t)) ^ 
                (mesh->GetVertexPos(firstId+2,t)-mesh->GetVertexPos(firstId+0,t))).GetLength() / 2);
#endif
        }
        return ret;
    }

    virtual bool Intersect(const Ray& ray, Intersection* intersection) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "DeformedMeshTriangle Intersect Queries");
        rays->Increment();
#endif

        DeformedMeshShape* mesh = (DeformedMeshShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        float t;
        bool hit = mesh->IntersectElement(ray, t, intersection->dp, firstId/3);

        if(hit) {
            intersection->t = t;
            intersection->m = material;
        }
#else
        float t, b1, b2;
        bool hit = IntesectTriangle(mesh->GetVertexPos(firstId+0,ray.time), 
                                    mesh->GetVertexPos(firstId+1,ray.time), 
                                    mesh->GetVertexPos(firstId+2,ray.time), ray, &t, &b1, &b2);

        if(hit) {
            // geometry
            intersection->t = t;
            intersection->dp.P = ray.Eval(t);
            intersection->dp.Ng = ((mesh->GetVertexPos(firstId+1,ray.time)-mesh->GetVertexPos(firstId+0,ray.time)) ^ 
                                   (mesh->GetVertexPos(firstId+2,ray.time)-mesh->GetVertexPos(firstId+0,ray.time))).GetNormalized();
            intersection->dp.uv = Vec2f(b1,b2);
            
            // shading
            if(mesh->HasVertexNormal()) {
                intersection->dp.N = mesh->GetVertexNormal(firstId+0,ray.time) * (1-b1-b2) + 
                                     mesh->GetVertexNormal(firstId+1,ray.time) * b1 + 
                                     mesh->GetVertexNormal(firstId+2,ray.time) * b2;
            } else {
                intersection->dp.N = intersection->dp.Ng;
            }
            intersection->dp.GenerateTuTv();
            if(mesh->HasVertexTexCoord()) {
                intersection->dp.st = mesh->GetVertexTexCoord(firstId+0,ray.time) * (1-b1-b2) + 
                                      mesh->GetVertexTexCoord(firstId+1,ray.time) * b1 + 
                                      mesh->GetVertexTexCoord(firstId+2,ray.time) * b2;
            } else {
                intersection->dp.st = intersection->dp.uv;
            }

            // material
            intersection->m = material;
        }
#endif

        return hit;
    }

    virtual bool Intersect(const Ray& ray) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "DeformedMeshTriangle IntersectAny Queries");
        rays->Increment();
#endif

        DeformedMeshShape* mesh = (DeformedMeshShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return mesh->IntersectElement(ray, firstId/3);
#else
        float t, b1, b2;
        return IntesectTriangle(
            mesh->GetVertexPos(firstId+0,ray.time), 
            mesh->GetVertexPos(firstId+1,ray.time), 
            mesh->GetVertexPos(firstId+2,ray.time), 
            ray, &t, &b1, &b2);
#endif
    }

protected:
    int           firstId;
};

#endif
