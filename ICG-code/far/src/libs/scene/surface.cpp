#include "surface.h"

Surface::Surface(shared_ptr<Shape> s, shared_ptr<Material> m, shared_ptr<Xform> xf, bool il) {
    name = "";
    shape = s;
    material = m;
    xform = xf;
    isAreaLight = il;
}

Range3f Surface::ComputeBoundingBox(float time) {
    return xform->GetTransform(time).TransformBBox(shape->ComputeBoundingBox(time)); 
}

Range3f Surface::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    if(IsStatic()) return ComputeBoundingBox(time.Middle());
    else {
        Range3f bbox;
        for(int i = 0; i < approximationSamples; i ++) {
            float t = (float)i / (approximationSamples-1);
            bbox.Grow(ComputeBoundingBox(time.Get(t)));
        }
        return bbox;
    }
}

// this is a really bad approximation
// should extract the scale from the xform
float Surface::ComputeArea(float time) {
    float area = shape->ComputeArea(time);
    Range3f bbox(-area/2,area/2);
    return xform->GetTransform(time).TransformBBox(bbox).GetSize().GetLength();
}

Intervalf Surface::ComputeAnimationInterval() {
    Intervalf ret = Intervalf::Invalid();
    if(!IsStatic()) {
        ret.Include(xform->ComputeAnimationInterval());
        ret.Include(shape->ComputeAnimationInterval());
    }
    return ret;
}

bool Surface::FlattenXform() {
    if(!xform->IsStatic()) return false;
    if(xform->IsIdentity()) { xform = XformIdentity::IdentityXform(); return false; }
    
    // flatten
    bool flattened = shape->ApplyXform(xform);
    if(flattened) xform = XformIdentity::IdentityXform();
    return flattened;
}

void Surface::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Surfaces");
    stat->Increment();

    shape->CollectStats(stats);
    material->CollectStats(stats);
    xform->CollectStats(stats);
}

string Surface::serialize_typename() { return "Surface"; } 

void Surface::serialize(Archive* a) {
    a->optional("name", name, string(""));
	a->member("shape", shape);
	a->member("material", material);
	a->optional("xform", xform, XformIdentity::IdentityXform());
}
