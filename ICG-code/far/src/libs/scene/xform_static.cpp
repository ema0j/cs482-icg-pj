#include "xform_static.h"

void XformStatic::SetIdentity() {
    m = Matrix4d::Identity();
    mi = Matrix4d::Identity();
}

void XformStatic::Translate(const Vec3f& v) {
    m = m % Matrix4d::Translation(Vec3d(v));
    mi = Matrix4d::Translation(Vec3d(-v)) % mi;
}

void XformStatic::Rotate(const Vec3f& axis, float angle) {
    m = m % Matrix4d::Rotation(Vec3d(axis),angle);
    mi = Matrix4d::Rotation(Vec3d(axis),-angle) % mi;
}

void XformStatic::Scale(float s) {
    m = m % Matrix4d::Scale(s);
    mi = Matrix4d::Scale(1/s) % mi;
}

shared_ptr<Xform> XformStatic::Clone() {
    return shared_ptr<Xform>(new XformStatic(m,mi));
}

string XformStatic::serialize_typename() { return "XformStatic"; } 

void XformStatic::serialize(Archive* a) {
	a->member("m", m);
	if(a->isreading()) mi = m.GetInverse();
}

void XformStaticMatrix::SetIdentity() {
    m = Matrix4d::Identity();
    mi = Matrix4d::Identity();
}

void XformStaticMatrix::Translate(const Vec3f& v) {
    m = m % Matrix4d::Translation(Vec3d(v));
    mi = Matrix4d::Translation(Vec3d(-v)) % mi;
}

void XformStaticMatrix::Rotate(const Vec3f& axis, float angle) {
    m = m % Matrix4d::Rotation(Vec3d(axis),angle);
    mi = Matrix4d::Rotation(Vec3d(axis),-angle) % mi;
}

void XformStaticMatrix::Scale(float s) {
    m = m % Matrix4d::Scale(s);
    mi = Matrix4d::Scale(1/s) % mi;
}

void XformStaticMatrix::Compose(const Matrix4d& cm, const Matrix4d& cmi) {
    m = m % cm;
    mi = cmi % mi;
}

shared_ptr<Xform> XformStaticMatrix::Clone() {
    return shared_ptr<Xform>(new XformStaticMatrix(m,mi));
}

string XformStaticMatrix::serialize_typename() { return "XformStaticMatrix"; } 

void XformStaticMatrix::serialize(Archive* a) {
	a->member("m", m);
	if(a->isreading()) mi = m.GetInverse();
}

shared_ptr<Xform> XformStaticTranslation::Clone() {
    return shared_ptr<Xform>(new XformStaticTranslation(translation));
}

string XformStaticTranslation::serialize_typename() { return "XformStaticTranslation"; } 
	
void XformStaticTranslation::serialize(Archive* a) {
	a->member("translation", translation);
}

shared_ptr<Xform> XformStaticRotation::Clone() {
    return shared_ptr<Xform>(new XformStaticRotation(axis,angle));
}

string XformStaticRotation::serialize_typename() { return "XformStaticRotation"; } 

void XformStaticRotation::serialize(Archive* a) {
	a->member("axis", axis);
	a->member("angle", angle);
}

shared_ptr<Xform> XformStaticXYZRotation::Clone() {
    return shared_ptr<Xform>(new XformStaticXYZRotation(angles));
}

string XformStaticXYZRotation::serialize_typename() { return "XformStaticXYZRotation"; } 

void XformStaticXYZRotation::serialize(Archive* a) {
	a->member("angles", angles);
}

shared_ptr<Xform> XformStaticScale::Clone() {
    return shared_ptr<Xform>(new XformStaticScale(scale));
}

string XformStaticScale::serialize_typename() { return "XformStaticScale"; } 
	
void XformStaticScale::serialize(Archive* a) {
	a->member("scale", scale);
}


shared_ptr<Xform> XformStaticLookAt::Clone() {
    return shared_ptr<Xform>(new XformStaticLookAt(eye, target, up, flipz));
}

string XformStaticLookAt::serialize_typename() { return "XformStaticLookAt"; }
	
void XformStaticLookAt::serialize(Archive* a) {
	a->member("eye", eye);
	a->member("target", target);
	a->member("up", up);
    a->member("flipz", flipz);

    if(a->isreading()) _Init();
}

// tranform the xform wit a transform in local coordinates
void XformStaticLookAt::TransformLocal(const Matrix4d& xformLocal) {
    Matrix4d xformWorld = m % xformLocal % mi;
    eye = xformWorld.TransformPoint(eye);
    target = xformWorld.TransformPoint(target);
    up = xformWorld.TransformVector(up);
    _Init();
}

void XformStaticLookAt::TransformLocalTargetPivot(const Matrix4d& xformLocal) {
    Vec3d targetLocalTranslation = Vec3d::Z() * ((flipz)?1.0f:-1.0f) * (eye-target).GetLength();
    Matrix4d targetPivotXform = Matrix4d::Translation(targetLocalTranslation);
    Matrix4d targetPivotInvXform = Matrix4d::Translation(-targetLocalTranslation);
    Matrix4d xformWorld = m % targetPivotInvXform % xformLocal % targetPivotXform % mi;
    eye = xformWorld.TransformPoint(eye);
    target = xformWorld.TransformPoint(target);
    up = xformWorld.TransformVector(up);
    _Init();
}

void XformStaticLookAt::Set(const Vec3f& eye, const Vec3f& target, const Vec3f& up, bool flipz) { 
    this->eye = eye;
    this->target = target;
    this->up = up;
    this->flipz = flipz;
    _Init();
}
void XformStaticLookAt::_Init() { 
    // fix up UP
    // _OrthogonalizeUp();
    if(flipz) mi = Matrix4d::LookAtGL(Vec3d(eye), Vec3d(target), Vec3d(up)); 
    else mi = Matrix4d::LookAt(Vec3d(eye), Vec3d(target), Vec3d(up)); 
    m = mi.GetInverse(); 
}

void XformStaticStack::Translate(const Vec3f& v) {
	stack.push_back(shared_ptr<XformStaticNode>(new XformStaticTranslation(v)));
	_UpdateMatrices();
}

void XformStaticStack::Rotate(const Vec3f& axis, float angle) {
	stack.push_back(shared_ptr<XformStaticNode>(new XformStaticRotation(axis, angle)));
	_UpdateMatrices();
}

void XformStaticStack::RotateXYZ(const Vec3f& angles) {
	stack.push_back(shared_ptr<XformStaticNode>(new XformStaticXYZRotation(angles)));
	_UpdateMatrices();
}

void XformStaticStack::Scale(float s) {
	stack.push_back(shared_ptr<XformStaticNode>(new XformStaticScale(s)));
	_UpdateMatrices();
}

void XformStaticStack::Compose(const Matrix4d& cm, const Matrix4d& cmi) {
    stack.push_back(shared_ptr<XformStaticNode>(new XformStaticMatrix(cm,cmi)));
	_UpdateMatrices();
}

shared_ptr<Xform> XformStaticStack::Clone() {
    shared_ptr<XformStaticStack> ret = shared_ptr<XformStaticStack>(new XformStaticStack());
    for(size_t i = 0; i < stack.size(); i ++) {
        ret->Stack().push_back(dynamic_pointer_cast<XformStaticNode>(stack[i]->Clone()));
    }
    ret->InitFromRefs();
    return ret;
}

string XformStaticStack::serialize_typename() { return "XformStaticStack"; } 

void XformStaticStack::serialize(Archive* a) {
	a->iterate("stack", stack);
	if(a->isreading()) _UpdateMatrices();
}


void XformStaticStack::_UpdateMatrices() {
	m = Matrix4d::Identity();
	mi = Matrix4d::Identity();
	for(size_t i = 0; i < stack.size(); i ++) {
		m = m % stack[i]->GetTransform(0);
		mi = stack[i]->GetInverseTransform(0) % mi;
	}
}

