#ifndef _XFORM_STATIC_H_
#define _XFORM_STATIC_H_

#include "xform.h"
#include <vmath/spline.h>

// keeps a transform in place
// operation on the zform (such as translate, rotate and such)
// concatenate the current one with the ones in
//
// does not support non-uniform scaling, which implies
// everything is easier to write
class XformStatic : public Xform {
public:
    XformStatic() : Xform() { SetIdentity(); }
    XformStatic(const Matrix4d& m, const Matrix4d& mi) : Xform(), m(m), mi(mi) { }

    virtual Matrix4d GetTransform(float time) { return m; }
    virtual Matrix4d GetInverseTransform(float time) { return mi; }

    virtual bool IsIdentity() { return m.IsIdentity(); }
    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

    void Set(XformStatic* xf) { m = xf->m; mi = xf->mi; }
    void Set(const XformStatic* const xf) { m = xf->m; mi = xf->mi; }

    void SetIdentity();
    void Translate(const Vec3f& v);
    void Rotate(const Vec3f& axis, float angle);
    void Scale(float s);

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    Matrix4d m;
    Matrix4d mi;
};

struct XformStaticNode : public Xform {
public:
	virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }
};

// keeps a transform in place
// operation on the zform (such as translate, rotate and such)
// concatenate the current one with the ones in
//
// does not support non-uniform scaling, which implies
// everything is easier to write
class XformStaticMatrix : public XformStaticNode {
public:
    XformStaticMatrix() { SetIdentity(); }
    XformStaticMatrix(const Matrix4d& cm, const Matrix4d& cmi) : m(cm), mi(cmi) { }

    virtual Matrix4d GetTransform(float time) { return m; }
    virtual Matrix4d GetInverseTransform(float time) { return mi; }

    virtual bool IsIdentity() { return m.IsIdentity(); }

    void Set(const XformStaticMatrix& xf) { m = xf.m; mi = xf.mi; }

    void SetIdentity();
    void Translate(const Vec3f& v);
    void Rotate(const Vec3f& axis, float angle);
    void Scale(float s);
    void Compose(const Matrix4d& m, const Matrix4d& mi);

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    Matrix4d m;
    Matrix4d mi;
};

struct XformStaticTranslation : public XformStaticNode {
public:
    XformStaticTranslation() { SetIdentity(); }
    XformStaticTranslation(const Vec3f& t) { Set(t); }

	virtual Matrix4d GetTransform(float time) { return Matrix4d::Translation(Vec3d(translation)); }
    virtual Matrix4d GetInverseTransform(float time) { return Matrix4d::Translation(-Vec3d(translation)); }

	virtual bool IsIdentity() { return translation == Vec3f::Zero(); }

	void Set(const Vec3f& t) { translation = t; }
	void SetIdentity() { translation = Vec3f::Zero(); }

    Vec3f& Translation() { return translation; }

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename(); 
	virtual void serialize(Archive* a) ;

protected:
	Vec3f translation;
};

struct XformStaticRotation : public XformStaticNode {
public:
    XformStaticRotation() { SetIdentity(); }
    XformStaticRotation(const Vec3f& axis, float angle) { Set(axis, angle); }

	virtual Matrix4d GetTransform(float time) { return Matrix4d::Rotation(Vec3d(axis), toRadians(angle)); }
	virtual Matrix4d GetInverseTransform(float time) { return Matrix4d::Rotation(Vec3d(axis), toRadians(-angle)); }

	virtual bool IsIdentity() { return angle == 0; }

	void Set(const Vec3f& axis, float angle) { this->axis = axis; this->angle = angle; updated = false; }
	void SetIdentity() { axis = Vec3f::Z(); angle = 0; updated = false; }

    Vec3f& RotationAxis() { return axis; }
    float& RotationAngle() { return angle; }

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    void _UpdateMatrice()
    {
        if (!updated)
        {
            m = Matrix4d::Rotation(Vec3d(axis), toRadians(angle));
            mi = Matrix4d::Rotation(Vec3d(axis), toRadians(-angle));
            updated = true;
        }
    }
    Matrix4d m, mi;
    bool updated;
	Vec3f axis;
	float angle;
};

struct XformStaticXYZRotation : public XformStaticNode {
public:
    XformStaticXYZRotation() { SetIdentity(); }
    XformStaticXYZRotation(const Vec3f& angles) { Set(angles); }

    virtual Matrix4d GetTransform(float time) { _UpdateMatrice(); return m; }
	virtual Matrix4d GetInverseTransform(float time) { _UpdateMatrice(); return mi; }

    virtual bool IsIdentity() { return angles == Vec3f::Zero(); }

	void Set(const Vec3f& angles) { this->angles = angles; updated = false; }
	void SetIdentity() { angles = 0; updated = false; }

    Vec3f& RotationAngles() { return angles; }

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    void _UpdateMatrice()
    {
        if (!updated)
        {
            m = Matrix4d::RotationZ(toRadians(angles.z)) 
                % Matrix4d::RotationY(toRadians(angles.y)) 
                % Matrix4d::RotationX(toRadians(angles.x));
            mi = Matrix4d::RotationX(toRadians(-angles.x)) 
                % Matrix4d::RotationY(toRadians(-angles.y)) 
                % Matrix4d::RotationZ(toRadians(-angles.z));
            updated = true;
        }
    }
    bool updated;
    Vec3f angles;
    Matrix4d m, mi;
};

struct XformStaticScale : public XformStaticNode {
public:
    XformStaticScale() { SetIdentity(); }
    XformStaticScale(float s) { Set(s); }

	virtual Matrix4d GetTransform(float time) { return Matrix4d::Scale(scale); }
    virtual Matrix4d GetInverseTransform(float time) { return Matrix4d::Scale(1/scale); }

	virtual bool IsIdentity() { return scale == 1; }

	void Set(float s) { scale = s; }
	void SetIdentity() { scale = 1; }

    float& Scale() { return scale; }

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
	float scale;
};

// lines up the positive z (for camera use, turn on flipz)
struct XformStaticLookAt : public XformStaticNode {
public:
    XformStaticLookAt() { SetIdentity(); }
    XformStaticLookAt(const Vec3f& eye, const Vec3f& target, const Vec3f& up, bool flipz) { Set(eye,target,up,flipz); }

	virtual Matrix4d GetTransform(float time) { return m; }
    virtual Matrix4d GetInverseTransform(float time) { return mi; }

    virtual bool IsIdentity() { return m.IsIdentity(); }

    Vec3f GetEye() { return eye; }
    Vec3f GetTarget() { return target; }

    void SetEye(const Vec3f& eye) { this->eye = eye; _Init(); }
    void SetTarget(const Vec3f& target) { this->target = target; _Init(); }

    // tranform the xform wit a transform in local coordinates
    void TransformLocal(const Matrix4d& xformLocal);
    void TransformLocalTargetPivot(const Matrix4d& xformLocal) ;

	void Set(const Vec3f& eye, const Vec3f& target, const Vec3f& up, bool flipz);
    void SetIdentity() { flipz = false; eye = Vec3f::Zero(); target = eye + Vec3f::Z(); up = Vec3f::Y(); _Init(); }

    Vec3f& Eye() { return eye; }
    Vec3f& Target() { return target; }
    Vec3f& Up() { return up; }
    bool& Flipz() { return flipz; }
    void InitFromRefs() { _Init(); }

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename() ;
	virtual void serialize(Archive* a);

protected:
    Vec3f eye;
    Vec3f target;
    Vec3f up;
    bool flipz;

    Matrix4d m;
    Matrix4d mi;

    void _Init();
};

// same as XformStatic, but keeps a static stack in memory
class XformStaticStack : public Xform {
public:
    XformStaticStack() { SetIdentity(); }

    virtual Matrix4d GetTransform(float time) { return m; }
    virtual Matrix4d GetInverseTransform(float time) { return mi; }

    virtual bool IsIdentity() { return stack.empty(); }
    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

	void SetIdentity() { stack.clear(); _UpdateMatrices(); }

	vector<shared_ptr<Xform> >& Stack() { return stack; }
    void InitFromRefs() { _UpdateMatrices(); }

    void Translate(const Vec3f& v);
    void Rotate(const Vec3f& axis, float angle);
    void RotateXYZ(const Vec3f& angles);
    void Scale(float s);
    void Compose(const Matrix4d& m, const Matrix4d& mi);

    virtual shared_ptr<Xform> Clone();

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
	vector<shared_ptr<Xform> > stack;

	// these are caches for fast access - always valid since the xform are static
    Matrix4d m;
    Matrix4d mi;

	void _UpdateMatrices();
};

#endif
