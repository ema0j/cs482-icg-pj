#include "rayListEngine.h"
#include "rayTesselationCache.h"


Intervalf ListEngineGroupNode::ValidInterval()
{
    return _interval;
}
bool ListEngineGroupNode::Intersect(const Ray& r, Intersection* intersection)
{
    Ray ray = r;
    bool hit = false;
    for(uint64_t p = 0; p < _groups.size(); p++) 
    {
        if(_groups[p]->Intersect(ray, intersection)) 
        {
            ray.tMax = intersection->t;
            hit = true;
        }
    }
    return hit;
}
bool ListEngineGroupNode::IntersectAny(const Ray& ray)
{
    for(size_t p = 0; p < _groups.size(); p++) 
    {
        if(_groups[p]->IntersectAny(ray)) 
            return true;
    }    
    return false;
}
void ListEngineGroupNode::CollectStats(StatsManager& stats)
{
    for(size_t p = 0; p < _groups.size(); p++) 
        _groups[p]->CollectStats(stats);
}
Range3f ListEngineGroupNode::ComputeBoundingBox()
{
    Range3f bbox = Range3f::Empty();
    for(size_t i = 0; i < _groups.size(); i ++)
        bbox.Grow(_groups[i]->ComputeBoundingBox());
    return bbox;
}
float ListEngineGroupNode::ComputeAverageArea()
{
    float area = 0;
    for(size_t i = 0; i < _groups.size(); i ++)
        area += _groups[i]->ComputeAverageArea();
    return area;
}
