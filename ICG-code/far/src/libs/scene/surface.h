#ifndef _SURFACE_H_
#define _SURFACE_H_

#include "shape.h"
#include "material.h"
#include "xform.h"

class AreaLight;

class Surface : public SceneObject {
public:
    Surface() { isAreaLight = false; }
    Surface(shared_ptr<Shape> s, shared_ptr<Material> m, shared_ptr<Xform> xf, bool il = false);

    string& Name() { return name; }
    shared_ptr<Shape>& ShapeRef() { return shape; }
    shared_ptr<Material>& MaterialRef() { return material; }
    shared_ptr<Xform>& XformRef() { return xform; }

    bool IsXformed() { return !xform->IsIdentity(); }
    bool IsDeformed() { return !shape->IsStatic(); }
    bool IsStatic() { return xform->IsStatic() && shape->IsStatic(); }
    bool IsAreaLight() { return isAreaLight; }

    virtual Range3f ComputeBoundingBox(float time);
    virtual Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16);
    // this is a really bad approximation
    // should extract the scale from the xform
    virtual float ComputeArea(float time);

    virtual Intervalf ComputeAnimationInterval();

    bool FlattenXform();

    virtual void CollectStats(StatsManager& stats);

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    shared_ptr<Shape> shape;
    shared_ptr<Material> material;
    shared_ptr<Xform> xform;
    bool isAreaLight;

    string name;
};

#endif
