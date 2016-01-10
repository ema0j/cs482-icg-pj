#ifndef _SUBDIV_H_
#define _SUBDIV_H_

#include "shape.h"

class CatmullClarkShape : public Shape {
public:
    CatmullClarkShape() { }

    virtual void CollectStats(StatsManager& stats) {
        StatsCounterVariable* subdivs = stats.GetVariable<StatsCounterVariable>("Shapes", "Subdivs");
        subdivs->Increment();
    }

    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

    virtual float ComputeArea(float time) { assert(0); return -1; }
    virtual Range3f ComputeBoundingBox(float time);

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time);
    virtual void Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples) { Tesselate(cache,time.Middle()); }

    virtual bool ApplyXform(shared_ptr<Xform> xform) { return false; }

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    int subdivision;
    carray<Vec3f> posArray;
    carray<Vec3f> normalArray;
    carray<Vec2f> uvArray;
    carray<Vec4i> faceArray;
};

#endif
