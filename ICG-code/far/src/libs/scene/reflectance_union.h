#ifndef _REFLECTANCE_UNION_H_
#define _REFLECTANCE_UNION_H_

#include <scene/reflectance.h>
#include <scene/reflectance_delta.h>
#include <scene/reflectance_smooth.h>
#include "reflectance_uber.h"


#define MAX_LOBE_NUM 6

class BxdfUnion : public Bxdf
{
public:
    BxdfUnion() : nLobes(0) {}
    void Clear() { nLobes = 0; }

    inline uint32_t LobeNumber() const { return nLobes; }
    uint32_t MatchingLobeNum(BxdfType type) const;

    virtual bool HasSmooth() const;
    virtual bool HasDelta() const;

    virtual Vec3f Emission(const Vec3f& wo, const DifferentialGeometry& dp) const { return emission; };

    virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;

    virtual BxdfSample Sample(BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const;
    virtual BxdfSample SampleCos(BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const;
    virtual float Pdf(BxdfType type, const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;

    void AddLambert(const Vec3f &kd);
    void AddPhong(const Vec3f &ks, float n);
    void AddCookTorr(const Vec3f &ks, float n, float eta);
    void AddReflect(const Vec3f &kr, float eta);
    void AddTransmit(const Vec3f &kt, float eta);

    Vec3f       emission;
    BxdfLobe    lobes[MAX_LOBE_NUM];
    uint32_t    nLobes;


};

#endif
