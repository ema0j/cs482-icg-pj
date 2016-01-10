#ifndef _XFORM_DYNAMIC_H_
#define _XFORM_DYNAMIC_H_

#include "xform.h"

class XformKeyframed : public Xform {
public:
    XformKeyframed();
    XformKeyframed(Vec3f translation, shared_ptr<Spline<Vec3f, float> > translationSpline,
                   Vec3f rotationAxis, float rotationAngle, float scale, Vec3f center);
    XformKeyframed(Vec3f translation, const Spline<Vec3f, float>& translationSpline,
                   Vec3f rotationAxis, float rotationAngle, float scale, Vec3f center);
    XformKeyframed(Vec3f translation, shared_ptr<Spline<Vec3f, float> > translationSpline,
                   Vec4f rotationAxisAngle, float scale, Vec3f center);

    virtual Matrix4d GetTransform(float time);
    virtual Matrix4d GetInverseTransform(float time) ;

    virtual bool IsIdentity() { return false; }
	virtual bool IsStatic() { return translationSpline.IsConstant(); }
    virtual Intervalf ComputeAnimationInterval();

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    Vec3f translation;
    Spline<Vec3f, float> translationSpline;
    Vec3f rotationAxis;
    float rotationAngle;
    float scale;
    Vec3f center;
};

class XformMatKeyframed : public Xform {
public:
	XformMatKeyframed() { }
    XformMatKeyframed(shared_ptr<Spline<Matrix4d, float> > xformSpline);
    XformMatKeyframed(const Spline<Matrix4d, float>& xformSpline);

    virtual Matrix4d GetTransform(float time);
    virtual Matrix4d GetInverseTransform(float time);

    virtual bool IsIdentity() { return false; }
    virtual bool IsStatic() { return false; }
    virtual Intervalf ComputeAnimationInterval() ;

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    Spline<Matrix4d, float> xformSpline;
    Spline<Matrix4d, float> xformInvSpline;

protected:
    void _BuildInverse();
};

#endif
