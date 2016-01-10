//#ifndef _REFLECTANCE_SMOOTH_H_
//#define _REFLECTANCE_SMOOTH_H_
//
//#include <scene/reflectance.h>
//
////class SmoothReflectance : public Bxdf {
////public:
////    virtual bool HasSmooth() const { return true; }
////    virtual bool HasDelta() const { return false; }
////    virtual Vec3f Emission(const Vec3f& wo, const DifferentialGeometry& dp) const { return Vec3f::Zero(); }
////    virtual BxdfSample SampleCos(const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample) const { return SampleSmoothCos(wo, dp, angleSample, lobeSample); }
////    virtual BxdfSample SampleDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return BxdfSample(); }
////    virtual BxdfSample SampleReflectedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return BxdfSample(); }
////    virtual BxdfSample SampleTransmittedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return BxdfSample(); }
////};
////
////class NormalizedLambertReflectance : public SmoothReflectance {
////public:
////    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
////    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
////                                  const Vec2f& angleSample, float lobeSample) const ;
////    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////
////public:
////    Vec3f         rd;
////};
////
////class NormalizedPhongReflectance : public SmoothReflectance {
////public:
////    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
////    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
////                                const Vec2f& angleSample, float lobeSample) const ;
////    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////
////public:
////    Vec3f             rd;
////    Vec3f             rs;
////    float             n;
////};
////
////class NormalizedBlinnPhongReflectance : public SmoothReflectance {
////public:
////    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
////    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
////                                const Vec2f& angleSample, float lobeSample) const ;
////    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////
////public:
////    Vec3f             rd;
////    Vec3f             rs;
////    float             n;
////};
////
////class WardIsotropicReflectance : public SmoothReflectance {
////public:
////    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
////    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
////                                const Vec2f& angleSample, float lobeSample) const ;
////    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////
////public:
////    Vec3f             rd;
////    Vec3f             rs;
////    float             a;
////};
////
////class CookTorranceReflectance : public SmoothReflectance {
////public:
////    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
////    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
////                                const Vec2f& angleSample, float lobeSample) const ;
////    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////
////public:
////    Vec3f             rd;
////    Vec3f             rs;
////    float             f0;
////    float             sigma;
////
////protected:
////	float _F(float cosThetaD) const;
////	float _D(float cosThetaH) const;
////	float _G(float NdotH, float NdotV, float VdotH, float NdotL) const;
////};
////
////class KajiyaHairReflectance : public SmoothReflectance {
////public:
////    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const;
////    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
////                                const Vec2f& angleSample, float lobeSample) const ;
////    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const ;
////
////public:
////    Vec3f             kd;
////    Vec3f             ks;
////    float             n;
////};
////
//
//#endif
