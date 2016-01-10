#ifndef _RAYMONITOREDENGINE_H_
#define _RAYMONITOREDENGINE_H_

#include "RayEngine.h"
#include <misc/stats.h>

// NOTE: enabling timings slows things down a lot !!!

class RayMonitoredEngine : public RayEngine {
public:
    RayMonitoredEngine(RayEngine* e, bool timing = false) {
        engine = e;
        measureTiming = timing;
    }

    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "Intersect Queries");
        static StatsTimerVariable* timer = StatsManager::GetVariable<StatsTimerVariable>("Ray", "Intersect Time");
        rays->Increment();
        if(measureTiming) timer->Start();
#endif
        bool ret = engine->Intersect(ray, intersection);
#if (FAR_MULTITHREAD == 0)
        if(measureTiming) timer->Stop();
#endif
        return ret;

        // return engine->Intersect(ray, intersection);
    }
    virtual bool IntersectAny(const Ray& ray) {
#if (FAR_MULTITHREAD == 0)
        static StatsCounterVariable* rays = StatsManager::GetVariable<StatsCounterVariable>("Ray", "IntersectAny Queries");
        static StatsTimerVariable* timer = StatsManager::GetVariable<StatsTimerVariable>("Ray", "IntersectAny Time");
        rays->Increment();
        if(measureTiming) timer->Start();
#endif
        bool ret = engine->IntersectAny(ray);
#if (FAR_MULTITHREAD == 0)
        if(measureTiming) timer->Stop();
#endif
        return ret;
    }

    virtual void CollectStats(StatsManager& stats) {
        engine->CollectStats(stats);
    }

protected:
    RayEngine* engine;
    bool measureTiming;
};

#endif
