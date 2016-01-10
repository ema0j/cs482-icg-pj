#include "rayEngine.h"

#include "rayTesselatedKdTreeEngine.h"
#include "rayDoubleSidedEngine.h"
#include "rayListEngineBuilder.h"
#include "rayBVHEngineBuilder.h"

shared_ptr<RayEngine> RayEngine::BuildDefault(
    const vector<shared_ptr<Surface> >& surfaces, 
    const vector<shared_ptr<InstanceGroup> >& instances,
    const Intervalf& time, int timeSamples) {
        // return RayKdTreeEngineBuilder().Build(surfaces, time);
        // return RayKdTreeFastEngineBuilder().Build(surfaces, time);
        // return RayTesselatedListEngineBuilder().Build(surfaces, instances, time, timeSamples);

        //return shared_ptr<rayDoubleSidedEngine>(
        //    new rayDoubleSidedEngine(RayTesselatedKdTreeFastEngineBuilder().Build(surfaces, instances, time, timeSamples)));

        //return shared_ptr<rayDoubleSidedEngine>(
        //    new rayDoubleSidedEngine(
        //    shared_ptr<RayEngine>(RayListEngineBuilder().Build(surfaces, instances, time, timeSamples))));

        return shared_ptr<rayDoubleSidedEngine>(
            new rayDoubleSidedEngine(
            shared_ptr<RayEngine>(RayBVHEngineBuilder().Build(surfaces, instances, time, timeSamples))));

        //return RayTesselatedKdTreeFastEngineBuilder().Build(surfaces, instances, time, timeSamples);
        // return RayPrimitiveBVHEngineBuilder().Build(surfaces, instances, time, timeSamples);
        // return RayTesselatedBVHEngineBuilder().Build(surfaces, instances, time, timeSamples);
}
