#include "rayPrimitiveEngine.h"
#include <scene/shape.h>

float RayPrimitiveEngineBuilder::_BBOX_EPSILON = 1e-5f;

void RayPrimitiveEngine::CollectStats(StatsManager& stats) {
    for(size_t i = 0; i < _primitives.size(); i ++) {
        _primitives[i]->CollectStats(stats);
    }
}

Range3f RayPrimitiveEngine::ComputeBoundingBox() {
    Range3f bbox;
    for(size_t i = 0; i < _primitives.size(); i ++) {
        bbox.Grow(_primitives[i]->ComputeBoundingBox(validInterval));
    }
    return bbox;
}

float RayPrimitiveEngine::ComputeAverageArea() {
    float area = 0;
    for(size_t i = 0; i < _primitives.size(); i ++) {
        area += _primitives[i]->ComputeAverageArea(validInterval);
    }
    return area;
}


shared_ptr<RayPrimitiveEngine> RayPrimitiveEngineBuilder::Build(const vector<shared_ptr<Surface> >& surfaces, 
    const vector<shared_ptr<InstanceGroup> >& instances, const Intervalf& time, int timeSamples) {

    shared_ptr<RayPrimitiveEngine> mainEngine = _CreateEngine(time);
    map<Shape*, shared_ptr<RayPrimitiveEngine> > shapeToEngine;

    for(int i = 0; i < (int)surfaces.size(); i ++) {
        shared_ptr<Surface> surface = surfaces[i];
        shared_ptr<Xform> xform = surface->XformRef();

        shared_ptr<RayEngine> shapeEngine = _Subengine(mainEngine, surface->ShapeRef(), time, timeSamples);
        _Group(mainEngine, shapeEngine, surface->XformRef(), surface->MaterialRef(), time);
    }

    for(int i = 0; i < (int)instances.size(); i ++) {
        shared_ptr<InstanceGroup> instance = instances[i];
        shared_ptr<RayEngine> shapeEngine = _Subengine(mainEngine, instance->ShapeRef(), time, timeSamples);
        for(int i = 0; i < (int)instance->XformArray().size(); i ++) {
            _Group(mainEngine, shapeEngine, instance->XformArray()[i], instance->MaterialArray()[i], time);
        }
    }

    // init main acceleration
    _InitAcceleration(mainEngine, time);

    return mainEngine;
}

Range3f RayPrimitiveEngineBuilder::_ComputeBoundingBox(const vector<shared_ptr<RayPrimitive> >& primitives, const Intervalf& time) {
    Range3f bbox = Range3f();
    for(int i = 0; i < (int)primitives.size(); i++) {
        bbox.Grow(primitives[i]->ComputeBoundingBox(time));
    }
    // HACK
    // bbox.Scale(1 + _BBOX_EPSILON);
    bbox.GetMaxRef() += _BBOX_EPSILON;
    bbox.GetMinRef() -= _BBOX_EPSILON;
    return bbox;
}

Range3f RayPrimitiveEngineBuilder::_ComputeBoundingBox(const vector<shared_ptr<RayPrimitive> >& primitives, vector<Range3f>& bboxes, const Intervalf& time) {
    Range3f bbox = Range3f();
    for(int i = 0; i < (int)primitives.size(); i++) {
        bboxes[i] = primitives[i]->ComputeBoundingBox(time);
        bbox.Grow(bboxes[i]);
    }
    // HACK
    // bbox.Scale(1 + _BBOX_EPSILON);
    bbox.GetMaxRef() += _BBOX_EPSILON;
    bbox.GetMinRef() -= _BBOX_EPSILON;
    return bbox;
}

shared_ptr<RayEngine> RayPrimitiveEngineBuilder::_Subengine(shared_ptr<RayPrimitiveEngine> e, shared_ptr<Shape> shape, const Intervalf& time, int timeSamples) {
    shared_ptr<RayPrimitiveEngine> se = _CreateEngine(time);
    _InitAcceleration(se, time);
    e->_subengines.push_back(se);
    return se;
}

void RayPrimitiveEngineBuilder::_Group(shared_ptr<RayPrimitiveEngine> e, shared_ptr<RayEngine> se, shared_ptr<Xform> xf, shared_ptr<Material> m, const Intervalf& time) {
    e->_primitives.push_back(shared_ptr<RayPrimitive>(new RayGroupPrimitive(se, xf.get(), m.get())));
}
