#include "source_delta.h"
#include "sampling.h"

Vec3f PointSource::SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist) {
    *wi = - P.GetNormalized();
    *shadowRayMaxDist = P.GetLength();
    return intensity / P.GetLengthSqr();
}

Vec3f PointSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    *E = Vec3f::Zero();
    *D = Sampling::SphericalDirection(sa, pdf);
    return intensity;
}

shared_ptr<VLightSample> PointSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
                                              const Vec2f& ss, const Vec2f& sa, float time)
{
    float pdf;
    Vec3f D = Sampling::SphericalDirection(sa, &pdf);
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new OmniVLightSample(Vec3f::Zero(), intensity));
    return sample;
}

string PointSource::serialize_typename() { return "PointSource"; } 

void PointSource::serialize(Archive * a) {
	a->member("intensity", intensity);
}

Vec3f DirectionalSource::SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist) {
    *wi = - Vec3f::Z(); // opposire light dir
    *shadowRayMaxDist = RAY_INFINITY;
    return le;
}

#define EPSILONSCALE 1.00001f
Vec3f DirectionalSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    Vec2f uv = Sampling::ConcentricDisk(sa);
    Vec3f PLocalDisk = (Vec3f::X() * uv.x + Vec3f::Y() * uv.y) * (sceneRadius * EPSILONSCALE);
    *E = PLocalDisk + sceneCenter - Vec3f::Z() * (sceneRadius * EPSILONSCALE);
    *D = Vec3f::Z();
    
    *pdf = 1.f / (PIf * sceneRadius * sceneRadius); 
    return le;
}

shared_ptr<VLightSample> DirectionalSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
                                              const Vec2f& ss, const Vec2f& sa, float time)
{
    Vec2f uv = Sampling::ConcentricDisk(sa);
    Vec3f PLocalDisk = (Vec3f::X() * uv.x + Vec3f::Y() * uv.y) * (sceneRadius * EPSILONSCALE);
    Vec3f P = PLocalDisk + sceneCenter - Vec3f::Z() * (sceneRadius * EPSILONSCALE);
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new DirVLightSample(Vec3f::Z(), le));
    return sample;
}


string DirectionalSource::serialize_typename() { return "DirectionalSource"; } 

void DirectionalSource::serialize(Archive * a) {
	a->member("intensity", le);
}

Vec3f CosineSource::SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist) {
    *wi = - P.GetNormalized();
    *shadowRayMaxDist = P.GetLength();
    return intensity * (-(*wi).z) / P.GetLengthSqr();
}

Vec3f CosineSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    *E = Vec3f::Zero();
    *D = Sampling::HemisphericalDirectionCos(sa, pdf);
    assert(0);
    return intensity;
}

shared_ptr<VLightSample> CosineSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
                                              const Vec2f& ss, const Vec2f& sa, float time)
{
    float pdf;
    Vec3f D = Sampling::HemisphericalDirectionCos(sa, &pdf);
    assert(false);
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new OmniVLightSample(Vec3f::Zero(), intensity));
    return sample;
}

string CosineSource::serialize_typename() { return "CosineSource"; } 

void CosineSource::serialize(Archive * a) {
	a->member("intensity", intensity);
}

Vec3f CosineClampedSource::SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist) {
    *wi = - P.GetNormalized();
    *shadowRayMaxDist = P.GetLength();
    return intensity * (-(*wi).z) / ClampDistSqr(P.GetLengthSqr());
}

Vec3f CosineClampedSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    *E = Vec3f::Zero();
    *D = Sampling::HemisphericalDirectionCos(sa, pdf);
    assert(0);
    return intensity;
}

shared_ptr<VLightSample> CosineClampedSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
                                              const Vec2f& ss, const Vec2f& sa, float time)
{
    float pdf;
    Vec3f D = Sampling::HemisphericalDirectionCos(sa, &pdf);
    assert(false);
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new OmniVLightSample(Vec3f::Zero(), intensity));
    return sample;
}

string CosineClampedSource::serialize_typename() { return "CosineClampedSource"; } 

void CosineClampedSource::serialize(Archive * a) {
	a->member("intensity", intensity);
	a->member("minDist", minDist);
}

Vec3f SpotSource::SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist) {
    *wi = - P.GetNormalized();
    *shadowRayMaxDist = P.GetLength();
    return intensity * Falloff(*wi) / P.GetLengthSqr();
}

Vec3f SpotSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    *E = Vec3f::Zero();
    *D = Sampling::ConicalDirection(sa, cosOuterAngle, pdf);
    return intensity * Falloff(*D);
}

shared_ptr<VLightSample> SpotSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
                                              const Vec2f& ss, const Vec2f& sa, float time)
{
    float pdf;
    Vec3f D = Sampling::ConicalDirection(sa, cosOuterAngle, &pdf);
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new SpotVLightSample(Vec3f::Zero(), intensity, cosInnerAngle, cosOuterAngle));
    return sample;
}

float SpotSource::Falloff(const Vec3f& wi) {
    float c = - wi.z;
    if(c <= cosOuterAngle) return 0;
    else if(c >= cosInnerAngle) return 1;
    else {
        float delta = (c - cosOuterAngle)/(cosInnerAngle-cosOuterAngle);
        return delta*delta*delta*delta;
    }
}

string SpotSource::serialize_typename() { return "SpotSource"; } 

void SpotSource::serialize(Archive * a) {
	a->member("intensity", intensity);
	a->member("innerAngleDegree", innerAngleDegree);
	a->member("outerAngleDegree", outerAngleDegree);
	
	if(a->isreading()) _Init();		
}

void SpotSource::_Init() {
    cosInnerAngle = cos(toRadians(innerAngleDegree));
    cosOuterAngle = cos(toRadians(outerAngleDegree));
}
