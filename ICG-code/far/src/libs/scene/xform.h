#ifndef _XFORM_H_
#define _XFORM_H_

#include "sceneObject.h"
#include <misc/stats.h>
#include <scene/smath.h>

// based class
class Xform : public SceneObject {
public:
    virtual Matrix4d GetTransform(float time) = 0;
    virtual Matrix4d GetInverseTransform(float time) = 0;

    virtual bool IsIdentity() = 0;
    virtual bool IsStatic() = 0;
    virtual Intervalf ComputeAnimationInterval() = 0;

    virtual shared_ptr<Xform> Clone() = 0;

    virtual void CollectStats(StatsManager& stats);

	static shared_ptr<Xform> IdentityXform();
};

class XformIdentity : public Xform {
public:
    XformIdentity() { }

    virtual Matrix4d GetTransform(float time) { return Matrix4d::Identity(); }
    virtual Matrix4d GetInverseTransform(float time) { return Matrix4d::Identity(); }

    virtual bool IsIdentity() { return true; }
    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

    virtual shared_ptr<Xform> Clone() { return Xform::IdentityXform(); }

	static string serialize_typename();
	
	virtual void serialize(Archive* a);
};

#endif
