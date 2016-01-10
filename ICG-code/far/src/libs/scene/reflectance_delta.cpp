//#include "reflectance_delta.h"
//
//BxdfSample PerfectReflectionBrdf::SampleReflectedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const {
//    // Bug when wo is not on the upper hemisphere of dp.N. Numerical error in sphere intersection code.
//    BxdfSample ret;
//    ret.pdf = 1;
//    ret.brdfCos = kr; 
//    //ret.brdfCos = ReflectanceUtils::UpperHemisphere(wo, dp) ? kr : Vec3f::Zero();  Invalid the brdfsample? 
//    ret.delta = true;
//	ret.wi = ReflectanceUtils::UpperHemisphere(wo, dp) ? ReflectanceUtils::MirrorDirection(dp.N, wo) : -wo; // or let it straight through?
//    return ret;
//}
//
//BxdfSample SimpleRefractionBtdf::SampleTransmittedDelta(const Vec3f& wo, const DifferentialGeometry& dp) const {
//	BxdfSample ret;
//	ret.pdf = 1;
//	ret.delta = true;
//
//	Vec3f w = dp.VectorToLocal(wo);
//	bool entering = (w.z > 0.0f);
//
//	float eta = entering ? (1.0f / index) : index;
//	float sini2 = 1.0f - w.z * w.z;
//	float sint2 = eta * eta * sini2;
//
//	// Handle total internal reflection for transmission
//	if (sint2 >= 1.) 
//		ret.brdfCos = Vec3f::Zero();
//	float cost = sqrtf(max(0.f, 1.f - sint2));
//	if (entering) 
//		cost = -cost;
//	float sintOverSini = eta;
//
//	Vec3f wi = Vec3f(sintOverSini * -w.x, sintOverSini * -w.y, cost);
//	ret.wi = dp.VectorToWorld(wi);
//	ret.brdfCos = kt / eta * eta;
//	return ret;
//}
