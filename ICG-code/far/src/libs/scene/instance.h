#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "shape.h"
#include "material.h"
#include "xform.h"

class InstanceGroup : public SceneObject {
public:
    InstanceGroup() { }
    InstanceGroup(shared_ptr<Shape> s, vector<shared_ptr<Material> >& m, vector<shared_ptr<Xform> >& xf);

    uint64_t NInstances() { return xformArray.size(); }

    string& Name() { return name; }
    shared_ptr<Shape>& ShapeRef() { return shape; }
    vector<shared_ptr<Material> >& MaterialArray() { return materialArray; }
    vector<shared_ptr<Xform> >& XformArray() { return xformArray; }

    bool IsXformed(int instance) { return !xformArray[instance]->IsIdentity(); }
    bool IsDeformed(int instance) { return !shape->IsStatic(); }
    bool IsStatic(int instance) { return xformArray[instance]->IsStatic() && shape->IsStatic(); }
    bool IsDeformed() { for(int i = 0; i < NInstances(); i ++) { if(IsDeformed(i)) return true; } return false; }
    bool IsStatic() { for(int i = 0; i < NInstances(); i ++) { if(!IsStatic(i)) return false; } return true; }

    virtual Range3f ComputeBoundingBox(int instance, float time);
    // this is a really bad approximation
    // should extract the scale from the xform
    virtual float ComputeArea(int instance, float time);

    virtual Intervalf ComputeAnimationInterval(int instance);
    virtual Intervalf ComputeAnimationInterval();

    virtual void CollectStats(StatsManager& stats);

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    shared_ptr<Shape> shape;
    vector<shared_ptr<Material> > materialArray;
    vector<shared_ptr<Xform> > xformArray;

    string name;
};

#endif
