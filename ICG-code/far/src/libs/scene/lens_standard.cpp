#include "lens_standard.h"

PinholeLens::PinholeLens(const Vec2f& fovDegree) { this->fovDegree = fovDegree; _Init(); }

Ray PinholeLens::GenerateRay(const Vec2f& uv, float time) {
    Vec3f Q = _ImagePlanePoint(uv);
    return Ray(Vec3f::Zero(), Q.GetNormalized(), time);
}

Ray PinholeLens::GenerateRay(const Vec2f& uv, const Vec2f& auv, float time) {
    return GenerateRay(uv,time);
}

Matrix4d PinholeLens::ProjectionMatrixGL(float time) { 
    return Matrix4d::PerspectiveGL(2 * fovDegree.y, imagePlaneSize.y/imagePlaneSize.x, 0.1, 5000); 
}

shared_ptr<Lens> PinholeLens::Clone() { 
    return shared_ptr<Lens>(new PinholeLens(fovDegree)); 
}

string PinholeLens::serialize_typename() { return "PinholeLens"; } 

void PinholeLens::serialize(Archive * a) {
	a->member("fovDegree", fovDegree);
	
	if(a->isreading()) { _Init(); }
}

void PinholeLens::_Init() { imagePlaneSize = Vec2f(tanf(toRadians(fovDegree[0])),tanf(toRadians(fovDegree[1]))); }

Vec3f PinholeLens::_ImagePlanePoint(const Vec2f& uv) {
    return Vec3f( (2*uv.x-1)*imagePlaneSize.x, (2*uv.y-1)*imagePlaneSize.y, -1);
}

ThinLens::ThinLens(const Vec2f& fovDegree, float focalDistance, float apertureSize) { 
    this->fovDegree = fovDegree; 
	this->focalDistance = focalDistance;
	this->apertureSize = apertureSize;
    _Init();
}

Ray ThinLens::GenerateRay(const Vec2f& uv, float time) {
    Vec3f Q = _FocalPlanePoint(uv);
    return Ray(Vec3f::Zero(), Q.GetNormalized(), time);
}

Ray ThinLens::GenerateRay(const Vec2f& uv, const Vec2f& auv, float time) {
    Vec3f Q = _FocalPlanePoint(uv);
    Vec3f S = _LensPlanePoint(auv);

    return Ray(S, (Q-S).GetNormalized(), time);
}

Matrix4d ThinLens::ProjectionMatrixGL(float time) { 
    return Matrix4d::PerspectiveGL(2 * fovDegree.y, imagePlaneSize.y/imagePlaneSize.x, 0.1, 5000); 
}

shared_ptr<Lens> ThinLens::Clone() { 
    return shared_ptr<Lens>(new ThinLens(fovDegree, focalDistance, apertureSize)); 
}

string ThinLens::serialize_typename() { return "ThinLens"; } 

void ThinLens::serialize(Archive * a) {
	a->member("fovDegree", fovDegree);
	a->member("focalDistance", focalDistance);
	a->member("apertureSize", apertureSize);
	
    if(a->isreading()) { _Init(); }
}

Vec3f ThinLens::_FocalPlanePoint(const Vec2f& uv) {
    return Vec3f(
        (2*uv.x-1)*imagePlaneSize[0]*focalDistance, 
        (2*uv.y-1)*imagePlaneSize[1]*focalDistance, 
        (-1)*focalDistance);
}

Vec3f ThinLens::_LensPlanePoint(const Vec2f& uv) {
    return Vec3f( (2*uv.x-1)*lensPlaneSize.x, (2*uv.y-1)*lensPlaneSize.y, 0);
}

void ThinLens::_Init() {
    imagePlaneSize = Vec2f(tanf(toRadians(fovDegree[0])),tanf(toRadians(fovDegree[1])));
    lensPlaneSize = apertureSize;
}

