#include "light.h"

ShadowSample Light::SampleShadow(const Vec3f& P, const Vec2f& s, float time) {
    ShadowSample ret;
    if(xform->IsIdentity()) {
        ret.le = source->SampleShadow(P,s,time,&ret.wi,&ret.maxDist,&ret.pdf);
    } else {
        Vec3f Pl = xform->GetInverseTransform(time).TransformPoint(P);
        ret.le = source->SampleShadow(Pl,s,time,&ret.wi,&ret.maxDist,&ret.pdf);
        ret.wi = xform->GetTransform(time).TransformVector(ret.wi).GetNormalized();
    }
    return ret;
}

float Light::SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time) {
    if(xform->IsIdentity()) { 
        return source->SampleShadowPdf(P,wi,time);
    } else {
        Vec3f Pl = xform->GetInverseTransform(time).TransformPoint(P);
        Vec3f wil = xform->GetInverseTransform(time).TransformVector(wi).GetNormalized();
        return source->SampleShadowPdf(Pl,wil,time);
    }
}

PhotonSample Light::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time) {
    PhotonSample ret;
    if(xform->IsIdentity()) {
        ret.le = source->SamplePhoton(sceneCenter,sceneRadius,ss,sa,time,&ret.P,&ret.wo,&ret.pdf);
    } else {
        Vec3f Cl = xform->GetInverseTransform(time).TransformPoint(sceneCenter);
        ret.le = source->SamplePhoton(Cl,sceneRadius,ss,sa,time,&ret.P,&ret.wo,&ret.pdf);
        Matrix4d m = xform->GetTransform(time);
        ret.P = m.TransformPoint(ret.P);
        ret.wo = m.TransformVector(ret.wo).GetNormalized();
    }
    return ret;
}

shared_ptr<VLightSample> Light::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time)
{
    shared_ptr<VLightSample> ret;
    if(xform->IsIdentity()) {
        ret = source->SampleVLight(sceneCenter, sceneRadius, ss, sa, time);
    } else {
        Vec3f Cl = xform->GetInverseTransform(time).TransformPoint(sceneCenter);
        ret = source->SampleVLight(Cl, sceneRadius, ss, sa, time);
        Matrix4d m = xform->GetTransform(time);
        ret->Transform(m);
    }
    return ret;
}


Vec3f Light::SampleBackground(const Vec3f& dir, float time) {
    if(xform->IsIdentity()) { 
        return source->SampleBackground(dir, time);
    } else {
        return source->SampleBackground(xform->GetInverseTransform(time).TransformVector(dir).GetNormalized(), time);
    }
}

GLLightApproximation Light::ApproximateAsGLLight(float time) {
    GLLightApproximation approx;
    source->ApproximateAsGLLight(time, approx.Cl, approx.isDirectional);
    if(!approx.isDirectional) {
        approx.posDir = xform->GetTransform(time).TransformPoint<float>(Vec3f(0,0,0));
    } else {
        approx.posDir = xform->GetTransform(time).TransformVector<float>(Vec3f(0,0,1)).GetNormalized();
    }
    return approx;
}

void Light::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Lights");
    stat->Increment();
    if(source) source->CollectStats(stats);
    if(xform) xform->CollectStats(stats);
}

string Light::serialize_typename() { return "Light"; } 
void Light::serialize(Archive * a) {
    a->optional("name", name, string(""));
	a->member("source", source);
    a->optional("xform", xform, XformIdentity::IdentityXform());
}

