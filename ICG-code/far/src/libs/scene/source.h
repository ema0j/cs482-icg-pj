#ifndef _SOURCE_H_
#define _SOURCE_H_

#include <scene/smath.h>
#include "sceneObject.h"
#include "vlight.h"

class Source : public SceneObject {
public:
    virtual Vec3f SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf) = 0;
    virtual float SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time) = 0;

    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) = 0;

    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time) = 0;

    virtual Vec3f SampleBackground(const Vec3f& dir, float time) = 0;

    virtual Vec3f Power(float sceneRadius) = 0;

    virtual bool IsDelta() = 0;
    virtual int GetShadowSamples() = 0;

    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) = 0;

    virtual shared_ptr<Surface> GetSurface(shared_ptr<Xform> xform) = 0; // this is for area lights

    virtual void CollectStats(StatsManager& stats);

};

#endif
