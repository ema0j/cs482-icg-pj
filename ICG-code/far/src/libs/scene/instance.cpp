#include "instance.h"

InstanceGroup::InstanceGroup(shared_ptr<Shape> s, vector<shared_ptr<Material> >& m, vector<shared_ptr<Xform> >& xf) {
    shape = s;
    materialArray = m;
    xformArray = xf;
}

Range3f InstanceGroup::ComputeBoundingBox(int instance, float time) {
    return xformArray[instance]->GetTransform(time).TransformBBox(shape->ComputeBoundingBox(time)); 
}

// this is a really bad approximation
// should extract the scale from the xform
float InstanceGroup::ComputeArea(int instance, float time) {
    float area = shape->ComputeArea(time);
    Range3f bbox(-area/2,area/2);
    return xformArray[instance]->GetTransform(time).TransformBBox(bbox).GetSize().GetLength();
}

Intervalf InstanceGroup::ComputeAnimationInterval(int instance) {
    Intervalf ret = Intervalf::Invalid();
    if(!IsStatic(instance)) {
        ret.Include(xformArray[instance]->ComputeAnimationInterval());
        ret.Include(shape->ComputeAnimationInterval());
    }
    return ret;
}

Intervalf InstanceGroup::ComputeAnimationInterval() {
    Intervalf ret = Intervalf::Invalid();
    if(!IsStatic()) { for(int i = 0; i < NInstances(); i ++) ret.Include(ComputeAnimationInterval(i)); }
    return ret;
}

void InstanceGroup::CollectStats(StatsManager& stats) {
    StatsCounterVariable* groups = stats.GetVariable<StatsCounterVariable>("Scene", "InstanceGroups");
    StatsCounterVariable* instances = stats.GetVariable<StatsCounterVariable>("Scene", "Instances");
    groups->Increment();
    instances->Increment(NInstances());

    shape->CollectStats(stats);
    for(int i = 0; i < NInstances(); i ++) {
        materialArray[i]->CollectStats(stats);
        xformArray[i]->CollectStats(stats);
    }
}

string InstanceGroup::serialize_typename() { return "InstanceGroup"; } 

void InstanceGroup::serialize(Archive* a) {
    a->optional("name", name, string(""));
	a->member("shape", shape);
	a->member("material", materialArray);
	a->member("xform", xformArray);
}
