#ifndef _REFLECTANCE_H_
#define _REFLECTANCE_H_

#include <scene/smath.h>
#include "sampling.h"

// delta and non-delta cannot be mixed
// for delta brdfs, pdf == 1 (should be inf)
// this might call for merging delta and non-delta by multiplying in the pdf
struct BxdfSample {
    Vec3f brdfCos;
    Vec3f wi;
    float pdf;
    bool delta;
    BxdfSample(const Vec3f &fcos, const Vec3f &wi, const float pdf, const bool delta) 
        : brdfCos(fcos), wi(wi), pdf(pdf), delta(delta) { }
    BxdfSample() : brdfCos(), wi(), pdf(0), delta(false) { }
    bool Valid() { return brdfCos > Vec3f::Zero() && pdf > 0; }
};

class ReflectanceUtils {
public:
    static Vec3f MirrorDirection(const Vec3f& n, const Vec3f& w) { return - w + n*(2*(w%n)); }

    static bool UpperHemisphere(const Vec3f& w, const DifferentialGeometry& dp) { return w % dp.N > 0; }
    static bool UpperHemisphere(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) {
        return UpperHemisphere(wo,dp) && UpperHemisphere(wi,dp);
    }

    static float Cos(const Vec3f& wi, const DifferentialGeometry& dp) { return wi % dp.N; }
    static float AbsCos(const Vec3f& wi, const DifferentialGeometry& dp) { return fabs(wi % dp.N); }
    static float PosCos(const Vec3f& wi, const DifferentialGeometry& dp) { return max(0.0f, wi % dp.N); }
};

enum BxdfType
{
    UNKNOWN_BXDF = 0,
    DIFFUSE_BXDF = 1,
    GLOSSY_BXDF = 2,
    SMOOTH_BXDF = DIFFUSE_BXDF | GLOSSY_BXDF,
    REFLECT_BXDF = 4,
    TRANSMIT_BXDF = 8,
    DELTA_BXDF = REFLECT_BXDF | TRANSMIT_BXDF,
    ALL_BXDF = SMOOTH_BXDF | DELTA_BXDF
};

class Bxdf {
public:
    virtual bool HasSmooth() const = 0;
    virtual bool HasDelta() const = 0;

    virtual Vec3f Emission(const Vec3f& wo, const DifferentialGeometry& dp) const = 0;

    virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const = 0;
    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const = 0;

    virtual BxdfSample Sample(BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const = 0;
    virtual BxdfSample SampleCos(BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const = 0;
    virtual float Pdf(BxdfType type, const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const = 0;
};

class LambertEmissionReflectance : public Bxdf {
public:
    virtual bool HasSmooth() const { return false; }
    virtual bool HasDelta() const { return false; }

    virtual Vec3f Emission(const Vec3f& wo, const DifferentialGeometry& dp) const { return le; }

    virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const { return Vec3f::Zero(); };
    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const { return Vec3f::Zero(); };

    virtual BxdfSample Sample(BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const { return BxdfSample(); };
    virtual BxdfSample SampleCos(BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const { return BxdfSample(); };
    virtual float Pdf(BxdfType type, const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const { return 0.0f; };
public:
    Vec3f         le;
};

#endif
