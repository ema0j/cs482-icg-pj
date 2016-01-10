#ifndef _RAYTESSELATEDENGINE_H_
#define _RAYTESSELATEDENGINE_H_

#include <ray/rayPrimitive.h>

// assumes scene can be tesselated in simple primitives
// handle instancing as recursive engine (may disallow this)
class RayTesselationCache;
class RayTesselatedEngine : public RayEngine {
public:
    virtual Intervalf ValidInterval() { return validInterval; }

    virtual void CollectStats(StatsManager& stats);

    virtual Range3f ComputeBoundingBox();
    virtual float ComputeAverageArea();

    friend class RayTesselatedEngineBuilder;
protected:
    Intervalf validInterval;
    vector<shared_ptr<RayPrimitive> >   _primitives;
    vector<shared_ptr<RayEngine> >      _subengines;
    vector<shared_ptr<RayTesselationCache> > _tesselatedShapes;
};

class RayTesselatedEngineBuilder {
public:
    shared_ptr<RayTesselatedEngine> Build(const vector<shared_ptr<Surface> >& surfaces, 
        const vector<shared_ptr<InstanceGroup> >& instances, const Intervalf& time, int timeSamples);

protected:
    virtual void _Tesselate(shared_ptr<RayTesselatedEngine> engine, shared_ptr<Shape> s, 
        const Matrix4d& xf, const Matrix4d& xfi, shared_ptr<Material> m, const Intervalf& time, int timeSamples) ;
    virtual void _Group(shared_ptr<RayTesselatedEngine> engine, shared_ptr<RayEngine> instanceEngine, 
        shared_ptr<Xform> xform, shared_ptr<Material> m, const Intervalf& time);
    virtual shared_ptr<RayTesselatedEngine> _Subengine(shared_ptr<RayTesselatedEngine> mainEngine, shared_ptr<Shape> shape, const Intervalf& time, int timeSamples);

    virtual shared_ptr<RayTesselatedEngine> _CreateEngine(const Intervalf& time) = 0;
    virtual void _InitAcceleration(shared_ptr<RayTesselatedEngine> engine, const Intervalf& time) = 0;

    static Range3f _ComputeBoundingBox(const vector<shared_ptr<RayPrimitive> >& primitives, const Intervalf& time);
    // this version saves the bboxes, whose array has to be properly sized before hand!!!
    static Range3f _ComputeBoundingBox(const vector<shared_ptr<RayPrimitive> >& primitives, vector<Range3f>& bboxes, const Intervalf& time); 

    static float _BBOX_EPSILON;
};

class RayTesselatedListEngine : public RayTesselatedEngine {
public:
    // contract: intersection is modified iff the ray hits
    virtual bool Intersect(const Ray& ray, Intersection* intersection);
    virtual bool IntersectAny(const Ray& ray);
};

class RayTesselatedListEngineBuilder : public RayTesselatedEngineBuilder {
protected:
    virtual shared_ptr<RayTesselatedEngine> _CreateEngine(const Intervalf& time) { return shared_ptr<RayTesselatedListEngine>(new RayTesselatedListEngine()); }
    virtual void _InitAcceleration(shared_ptr<RayTesselatedEngine> engine, const Intervalf& time) { }
};

#endif
