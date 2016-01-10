//#ifndef _REFLECTANCE_DELTA_H_
//#define _REFLECTANCE_DELTA_H_
//
//#include <scene/reflectance.h>
//
//class DeltaBxdf : public Bxdf {
//    virtual bool HasSmooth() const { return false; }
//    virtual bool HasDelta() const { return true; }
//    virtual Vec3f Emission(const Vec3f& wo, const DifferentialGeometry& dp) const { return Vec3f::Zero(); }
//	virtual Vec3f EvalSmoothBrdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp)  const { return Vec3f::Zero(); }
//    virtual Vec3f EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp)  const { return Vec3f::Zero(); }
//	virtual Vec3f EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp)  const { return Vec3f::Zero(); }
//    virtual BxdfSample SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                        const Vec2f& angleSample, float lobeSample) const { return BxdfSample(); }
//    virtual float SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const { return 0; }
//};
//
//class PerfectReflectionBrdf : public DeltaBxdf {
//public:
//    virtual BxdfSample SampleCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                        const Vec2f& angleSample, float lobeSample) const { return SampleDelta(wo,dp); }
//    virtual BxdfSample SampleDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return SampleReflectedDelta(wo, dp); }
//    virtual BxdfSample SampleReflectedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const;
//    virtual BxdfSample SampleTransmittedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return BxdfSample(); }
//
//public:
//    Vec3f       kr;
//};
//
//class SimpleRefractionBtdf : public DeltaBxdf {
//public:
//	virtual BxdfSample SampleCos(const Vec3f& wo, const DifferentialGeometry& dp,
//		const Vec2f& angleSample, float lobeSample) const { return SampleDelta(wo,dp); }
//	virtual BxdfSample SampleDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return SampleTransmittedDelta(wo, dp); }
//    virtual BxdfSample SampleReflectedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const { return BxdfSample(); }
//    virtual BxdfSample SampleTransmittedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const;
//
//public:
//	float		index;
//	Vec3f       kt;
//};
//
//
//#endif
