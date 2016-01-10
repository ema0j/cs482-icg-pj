#include "rayTesselatedEngine.h"
#include <scene/shape.h>
#include <scene/surface.h>
#include <scene/instance.h>
#include "rayTesselationCache.h"
#include "rayTesselatedPrimitive.h"

float RayTesselatedEngineBuilder::_BBOX_EPSILON = 1e-5f;

void RayTesselatedEngine::CollectStats(StatsManager& stats) {
    StatsCounterVariable* primitives = stats.GetVariable<StatsCounterVariable>("Ray", "Primitives");
    StatsCounterVariable* subengines = stats.GetVariable<StatsCounterVariable>("Ray", "SubEngines");

    primitives->Increment(_primitives.size());
    subengines->Increment(_subengines.size());

    for(size_t i = 0; i < _primitives.size(); i ++) {
        _primitives[i]->CollectStats(stats);
    }
    for(size_t i = 0; i < _subengines.size(); i ++) {
        _subengines[i]->CollectStats(stats);
    }
}

Range3f RayTesselatedEngine::ComputeBoundingBox() {
    Range3f bbox;
    for(size_t i = 0; i < _primitives.size(); i ++) {
        bbox.Grow(_primitives[i]->ComputeBoundingBox(validInterval));
    }
    return bbox;
}

float RayTesselatedEngine::ComputeAverageArea() {
    float area = 0;
    for(size_t i = 0; i < _primitives.size(); i ++) {
        area += _primitives[i]->ComputeAverageArea(validInterval);
    }
    return area;
}

shared_ptr<RayTesselatedEngine> RayTesselatedEngineBuilder::Build(
    const vector<shared_ptr<Surface> >& surfaces, const vector<shared_ptr<InstanceGroup> >& instances, 
    const Intervalf& time, int timeSamples) {

    shared_ptr<RayTesselatedEngine> mainEngine = _CreateEngine(time);
    map<Shape*, shared_ptr<RayTesselatedEngine> > shapeToEngine;

    /*
    for(int i = 0; i < (int)surfaces.size(); i ++) {
		Surface* surface = surfaces[i].get();
		Shape* shape = surface->ShapeRef().get();
        if(surface->IsXformed() || shape->IsShared()) {
            shared_ptr<RayTesselatedEngine> shapeEngine;
            if(shapeToEngine.find(shape) == shapeToEngine.end()) {
                shapeEngine = _CreateEngine(time);
                _Tesselate(shapeEngine, shape, Material::DefaultMaterial().get(), time, timeSamples);
                _InitAcceleration(shapeEngine, time);
                shapeToEngine[shape] = shapeEngine;
                _AddSubengine(mainEngine, shapeEngine);
            } else {
                shapeEngine = shapeToEngine.find(shape)->second;
            }
			_Group(mainEngine, shapeEngine, surface, time);
        } else {
			_Tesselate(mainEngine, shape, surface->MaterialRef().get(), time, timeSamples);
        }
    }
    */

    for(int i = 0; i < (int)surfaces.size(); i ++) {
        shared_ptr<Surface> surface = surfaces[i];
        shared_ptr<Xform> xform = surface->XformRef();
        if(xform->IsStatic()) {
            _Tesselate(mainEngine, surface->ShapeRef(), 
                xform->GetTransform(time.Middle()), xform->GetInverseTransform(time.Middle()),
                surface->MaterialRef(), time, timeSamples);
        } else {
            shared_ptr<RayTesselatedEngine> shapeEngine = _Subengine(mainEngine, surface->ShapeRef(), time, timeSamples);
            _Group(mainEngine, shapeEngine, surface->XformRef(), surface->MaterialRef(), time);
        }
    }

    for(int i = 0; i < (int)instances.size(); i ++) {
        shared_ptr<InstanceGroup> instance = instances[i];
        shared_ptr<RayTesselatedEngine> shapeEngine = _Subengine(mainEngine, instance->ShapeRef(), time, timeSamples);
        for(int i = 0; i < (int)instance->XformArray().size(); i ++) {
            _Group(mainEngine, shapeEngine, instance->XformArray()[i], instance->MaterialArray()[i], time);
        }
    }

    // init main acceleration
    _InitAcceleration(mainEngine, time);

    return mainEngine;
}

Range3f RayTesselatedEngineBuilder::_ComputeBoundingBox(const vector<shared_ptr<RayPrimitive> >& primitives, const Intervalf& time) {
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

Range3f RayTesselatedEngineBuilder::_ComputeBoundingBox(const vector<shared_ptr<RayPrimitive> >& primitives, vector<Range3f>& bboxes, const Intervalf& time) {
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

void RayTesselatedEngineBuilder::_Tesselate(shared_ptr<RayTesselatedEngine> e, shared_ptr<Shape> s, 
                                            const Matrix4d& xf, const Matrix4d& xfi,
                                            shared_ptr<Material> m, const Intervalf& time, int timeSamples) {
    // tesselate the shape
    shared_ptr<RayTesselationCache> shapeCache = shared_ptr<RayTesselationCache>(new RayTesselationCache());
    s->Tesselate(shapeCache, time, timeSamples);
    e->_tesselatedShapes.push_back(shapeCache);

    // set valid interval for these primitives
    e->validInterval = time;

    // transform surfaces if needed
    if(!xf.IsIdentity()) { shapeCache->Transform(xf,xfi); }

    // add primitives
    for(uint32_t i = 0; i < shapeCache->Triangles().triangles.size(); i ++) {
        e->_primitives.push_back(shared_ptr<RayPrimitive>(new RayTesselatedTrianglePrimitive(&shapeCache->Triangles(), m.get(), i)));
    }
    for(uint32_t i = 0; i < shapeCache->Spheres().pos.size(); i ++) {
        e->_primitives.push_back(shared_ptr<RayPrimitive>(new RayTesselatedSpherePrimitive(&shapeCache->Spheres(), m.get(), i)));
    }
    for(uint32_t i = 0; i < shapeCache->Segments().segments.size(); i ++) {
        e->_primitives.push_back(shared_ptr<RayPrimitive>(new RayTesselatedSegmentPrimitive(&shapeCache->Segments(), m.get(), i)));
    }
    for(uint32_t i = 0; i < shapeCache->TimeSampledTriangles().triangles.size(); i ++) {
        e->_primitives.push_back(shared_ptr<RayPrimitive>(new RayTesselatedTimeSampledTrianglePrimitive(&shapeCache->TimeSampledTriangles(), m.get(), i)));
    }
}

void RayTesselatedEngineBuilder::_Group(shared_ptr<RayTesselatedEngine> e, shared_ptr<RayEngine> se, shared_ptr<Xform> xf, shared_ptr<Material> m, const Intervalf& time) {
    e->_primitives.push_back(shared_ptr<RayPrimitive>(new RayGroupPrimitive(se, xf.get(), m.get())));
}

shared_ptr<RayTesselatedEngine> RayTesselatedEngineBuilder::_Subengine(shared_ptr<RayTesselatedEngine> e, shared_ptr<Shape> s, const Intervalf& time, int timeSamples) {
    shared_ptr<RayTesselatedEngine> se = _CreateEngine(time);
    _Tesselate(se, s, Matrix4d::Identity(), Matrix4d::Identity(), Material::DefaultMaterial(), time, timeSamples);
    _InitAcceleration(se, time);
    e->_subengines.push_back(se);
    return se;
}

bool RayTesselatedListEngine::Intersect(const Ray& ray, Intersection* intersection) {
    Ray r = ray;
    bool hit = false;
    for(int p = 0; p < (int)_primitives.size(); p ++) {
        if(_primitives[p]->Intersect(r, intersection)) {
            r.tMax = intersection->t;
            hit = true;
        }
    }
    return hit;
}

bool RayTesselatedListEngine::IntersectAny(const Ray& ray) {
    for(int p = 0; p < (int)_primitives.size(); p ++) {
        if(_primitives[p]->Intersect(ray)) 
            return true;
    }    
    return false;
}
