#ifndef _RAYMESHTRIANGLE_H_
#define _RAYMESHTRIANGLE_H_

#include "rayPrimitive.h"
#include <scene/intersectionMethods.h>
#include <scene/mesh.h>

class RayMeshTrianglePrimitive : public RayShapeElementPrimitive {
public:
    RayMeshTrianglePrimitive(MeshShape* s, Material* m, int id) : RayShapeElementPrimitive(s,m) { 
        static StatsCounterVariable* triangles = StatsManager::GetVariable<StatsCounterVariable>("Ray Primitive", "Triangles");
        triangles->Increment();

        firstId = id; 
    }

    virtual Range3f ComputeBoundingBox(const Intervalf& time) {
        MeshShape* mesh = (MeshShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return mesh->ComputeBBoxElement(0, firstId/3);
#else
        Range3f ret;
        ret.Grow(mesh->GetVertexPos(firstId+0));
        ret.Grow(mesh->GetVertexPos(firstId+1));
        ret.Grow(mesh->GetVertexPos(firstId+2));
        return ret;
#endif
    }

    virtual float ComputeAreaEstimate(const Intervalf& time) {
        MeshShape* mesh = (MeshShape*)shape;
#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return mesh->ComputeAreaElement(0, firstId/3);
#else
        return ((mesh->GetVertexPos(firstId+1)-mesh->GetVertexPos(firstId+0)) ^ 
            (mesh->GetVertexPos(firstId+2)-mesh->GetVertexPos(firstId+0))).GetLength() / 2;
#endif
    }

    virtual bool Intersect(const Ray& ray, Intersection* intersection) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "MeshTriangle Intersect Queries");
        rays->Increment();
#endif

        MeshShape* mesh = (MeshShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        float t;
        bool hit = mesh->IntersectElement(ray, t, intersection->dp, firstId/3);

        if(hit) {
            intersection->t = t;
            intersection->m = material;
        }
#else
        float t, b1, b2;
        bool hit = IntesectTriangle(mesh->GetVertexPos(firstId+0), 
                                    mesh->GetVertexPos(firstId+1), 
                                    mesh->GetVertexPos(firstId+2), ray, &t, &b1, &b2);

        if(hit) {
            // geometry
            intersection->t = t;
            intersection->dp.P = ray.Eval(t);
            intersection->dp.Ng = ((mesh->GetVertexPos(firstId+1)-mesh->GetVertexPos(firstId+0)) ^ 
                                   (mesh->GetVertexPos(firstId+2)-mesh->GetVertexPos(firstId+0))).GetNormalized();
            intersection->dp.uv = Vec2f(b1,b2);
            
            // shading
            if(mesh->HasVertexNormal()) {
                intersection->dp.N = mesh->GetVertexNormal(firstId+0) * (1-b1-b2) + 
                                    mesh->GetVertexNormal(firstId+1) * b1 + 
                                    mesh->GetVertexNormal(firstId+2) * b2;
            } else {
                intersection->dp.N = intersection->dp.Ng;
            }
            intersection->dp.GenerateTuTv();
            if(mesh->HasVertexTexCoord()) {
                intersection->dp.st = mesh->GetVertexTexCoord(firstId+0) * (1-b1-b2) + 
                                        mesh->GetVertexTexCoord(firstId+1) * b1 + 
                                        mesh->GetVertexTexCoord(firstId+2) * b2;
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
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "MeshTriangle IntersectAny Queries");
        rays->Increment();
#endif

        MeshShape* mesh = (MeshShape*)shape;

#if (FAR_SHAPE_INTERSECTION_CALLBACK == 1)
        return mesh->IntersectElement(ray, firstId/3);
#else
        float t, b1, b2;
        return IntesectTriangle(
            mesh->GetVertexPos(firstId+0), 
            mesh->GetVertexPos(firstId+1), 
            mesh->GetVertexPos(firstId+2), 
            ray, &t, &b1, &b2);
#endif
    }

protected:
    int           firstId;
};

#endif
