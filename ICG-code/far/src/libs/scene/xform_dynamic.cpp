#include "xform_dynamic.h"

XformKeyframed::XformKeyframed() {
    translation = Vec3f(0,0,0);
	translationSpline.Set(0);
	rotationAxis = Vec3f::Z();
    rotationAngle = 0;
    scale = 1;
	center = Vec3f::Zero();
}

XformKeyframed::XformKeyframed(Vec3f translation, shared_ptr<Spline<Vec3f, float> > translationSpline,
               Vec3f rotationAxis, float rotationAngle, float scale, Vec3f center) {
    this->translation = translation;
	if(translationSpline) this->translationSpline = *translationSpline;
	else this->translationSpline.Set(0);
    this->rotationAxis = rotationAxis;
    this->rotationAngle = rotationAngle;
    this->scale = scale;
    this->center = center;
}

XformKeyframed::XformKeyframed(Vec3f translation, const Spline<Vec3f, float>& translationSpline,
               Vec3f rotationAxis, float rotationAngle, float scale, Vec3f center) {
    this->translation = translation;
	this->translationSpline = translationSpline;
    this->rotationAxis = rotationAxis;
    this->rotationAngle = rotationAngle;
    this->scale = scale;
    this->center = center;
}

XformKeyframed::XformKeyframed(Vec3f translation, shared_ptr<Spline<Vec3f, float> > translationSpline,
               Vec4f rotationAxisAngle, float scale, Vec3f center) {
    this->translation = translation;
	if(translationSpline) this->translationSpline = *translationSpline;
	else this->translationSpline.Set(0);
    this->rotationAxis = rotationAxisAngle.IgnoreLast();
    this->rotationAngle = rotationAxisAngle[3];
    this->scale = scale;
    this->center = center;
}

Matrix4d XformKeyframed::GetTransform(float time) {
    Matrix4d currentMat;
    currentMat = Matrix4d::ComputeXform(Vec3d(translation + translationSpline.Eval(time)),
        Vec3d(rotationAxis), rotationAngle, scale, Vec3d(center));
    return currentMat;
}
Matrix4d XformKeyframed::GetInverseTransform(float time) {
    Matrix4d currentMatInv;
    currentMatInv = Matrix4d::ComputeXformInverse(Vec3d(translation + translationSpline.Eval(time)),
        Vec3d(rotationAxis), rotationAngle, scale, Vec3d(center));
    return currentMatInv;
}

Intervalf XformKeyframed::ComputeAnimationInterval() {
    if(translationSpline.IsConstant()) return Intervalf::Invalid();
    else return translationSpline.GetTimesInterval();
}

shared_ptr<Xform> XformKeyframed::Clone() {
    return shared_ptr<Xform>(new XformKeyframed(translation, translationSpline, rotationAxis, rotationAngle, scale, center));
}

string XformKeyframed::serialize_typename() { return "XformKeyframed"; } 

void XformKeyframed::serialize(Archive* a) {
	a->optional("translation", translation, Vec3f(0,0,0));
	a->optional("translationSpline", translationSpline, Spline<Vec3f, float>());
	a->optional("rotationAxis", rotationAxis, Vec3f::X());
	a->optional("rotationAngle", rotationAngle, 0.0f);
	a->optional("scale", scale, 1.0f);
	a->optional("center", center, Vec3f::Zero());
}

XformMatKeyframed::XformMatKeyframed(shared_ptr<Spline<Matrix4d, float> > xformSpline) {
    this->xformSpline = *xformSpline;
    _BuildInverse();
}

XformMatKeyframed::XformMatKeyframed(const Spline<Matrix4d, float>& xformSpline) {
    this->xformSpline = xformSpline;
    _BuildInverse();
}

Matrix4d XformMatKeyframed::GetTransform(float time) {
    return  xformSpline.Eval(time);
}
Matrix4d XformMatKeyframed::GetInverseTransform(float time) {
    return xformInvSpline.Eval(time);
}

Intervalf XformMatKeyframed::ComputeAnimationInterval() {
    return xformSpline.GetTimesInterval();
}

shared_ptr<Xform> XformMatKeyframed::Clone() {
    return shared_ptr<Xform>(new XformMatKeyframed(xformSpline));
}

string XformMatKeyframed::serialize_typename() { return "XformMatKeyframed"; }

void XformMatKeyframed::serialize(Archive* a) {
	a->member("spline", xformSpline);
	if(a->isreading()) _BuildInverse();
}

void XformMatKeyframed::_BuildInverse() {
	xformInvSpline = xformSpline;
    for(int i = 0; i < xformInvSpline.GetControlPoints()->size(); i ++) {
        xformInvSpline.GetControlPoints()->at(i) =
            xformInvSpline.GetControlPoints()->at(i).GetInverse();
    }
}
