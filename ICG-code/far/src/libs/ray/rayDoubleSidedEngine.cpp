#include "rayDoubleSidedEngine.h"


bool rayDoubleSidedEngine::Intersect(const Ray& ray, Intersection* intersection)
{
    bool hit = _engine->Intersect(ray, intersection);
    if(hit)
    {
        if(!(intersection->m->IntersectOption() & IOPT_SINGLE_SIDED))
        {
            if(intersection->dp.N % ray.D > 0) {
                intersection->dp.N = -intersection->dp.N;
                intersection->dp.Ng = -intersection->dp.Ng;
                intersection->dp.GenerateTuTv();
            }
        }
    }
    return hit;
}