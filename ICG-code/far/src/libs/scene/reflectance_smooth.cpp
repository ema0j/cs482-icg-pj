//#include "reflectance_smooth.h"
//#include "sampling.h"
//using std::swap;
//
//Vec3f NormalizedLambertReflectance::EvalSmoothCos(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//    return (rd/PIf) * ReflectanceUtils::Cos(wi,dp);
//}
//
//Vec3f NormalizedLambertReflectance::EvalSmooth(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//	if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//	return (rd/PIf);
//}
//
//BxdfSample NormalizedLambertReflectance::SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                                                const Vec2f& angleSample, float lobeSample) const {
//    if(!ReflectanceUtils::UpperHemisphere(wo,dp)) return BxdfSample();
//
//
//    BxdfSample ret;
//	Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &ret.pdf);
//	ret.wi = dp.VectorToWorld(localWi);
//	ret.brdfCos = EvalSmoothCos(wo,ret.wi,dp);
//	return ret;
//}
//
//float NormalizedLambertReflectance::SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return 0;
//
//    // check same hemisphere
//    Vec3f localWi = dp.VectorToLocal(wi);
//    return Sampling::HemisphericalDirectionCosPdf(localWi);
//}
//
//
//Vec3f NormalizedPhongReflectance::EvalSmoothCos(const Vec3f& wo, const Vec3f& wi,
//                          const DifferentialGeometry& dp) const {
//    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//	Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
//    return (rd/PIf + (rs*(2+n)/(2*PIf)) * powf(clamp((wi % R),0.0f,1.0f),n)) * ReflectanceUtils::Cos(wi,dp);
//}
//
//
//Vec3f NormalizedPhongReflectance::EvalSmooth(const Vec3f& wo, const Vec3f& wi,
//	const DifferentialGeometry& dp) const {
//		if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//		Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
//		return (rd/PIf + (rs*(2+n)/(2*PIf)) * powf(clamp((wi % R),0.0f,1.0f),n));
//}
//
//BxdfSample NormalizedPhongReflectance::SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                                              const Vec2f& angleSample, float lobeSample) const 
//{
//    BxdfSample ret;
//	bool sampleSpecular = lobeSample < (rs/(rd+rs)).Average();
//	if(!sampleSpecular)
//	{
//		Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &ret.pdf);
//		ret.wi = dp.VectorToWorld(localWi).GetNormalized();
//	} 
//	else 
//	{
//		Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
//		DifferentialGeometry dpr; 
//		dpr.N = R; 
//		dpr.GenerateTuTv();
//		Vec3f localWi = Sampling::HemisphericalDirectionCosPower(n, angleSample, &ret.pdf);
//		ret.wi = dpr.VectorToWorld(localWi).GetNormalized();
//	}
//	//ret.pdf = SampleSmoothPdf(wo,ret.wi,dp); //Bug here, comment out. the pdf is already set by Sampling::HemisphericalDirectionCosPower().
//	ret.brdfCos = EvalSmoothCos(wo,ret.wi,dp);
//    return ret;
//}
//
//float NormalizedPhongReflectance::SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    // if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return 0;
//    if(!ReflectanceUtils::UpperHemisphere(wo,dp)) return 0;
//
//    float pd = (rd/(rd+rs)).Average();
//    float ps = (rs/(rd+rs)).Average();
//
//	Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
//	DifferentialGeometry dpr; dpr.N = R; dpr.GenerateTuTv();
//
//    Vec3f localWiDiff = dp.VectorToLocal(wi); Vec3f localWiSpec = dpr.VectorToLocal(wi).GetNormalized();
//    return pd*Sampling::HemisphericalDirectionCosPdf(localWiDiff)+ps*Sampling::HemisphericalDirectionCosPowerPdf(n,localWiSpec);
//}
//
//Vec3f NormalizedBlinnPhongReflectance::EvalSmoothCos(const Vec3f& wo, const Vec3f& wi,
//                          const DifferentialGeometry& dp) const {
//    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//    Vec3f H = (wo + wi).GetNormalized();
//    return (rd/PIf + (rs*(2+n)/(2*PIf)) * powf(max(0.0f,(H % dp.N)),n)) * ReflectanceUtils::Cos(wi,dp);
//}
//
//Vec3f NormalizedBlinnPhongReflectance::EvalSmooth(const Vec3f& wo, const Vec3f& wi,
//	const DifferentialGeometry& dp) const {
//		if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//		Vec3f H = (wo + wi).GetNormalized();
//		return (rd/PIf + (rs*(2+n)/(2*PIf)) * powf(max(0.0f,(H % dp.N)),n));
//}
//
//BxdfSample NormalizedBlinnPhongReflectance::SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                                              const Vec2f& angleSample, float lobeSample) const {
//    BxdfSample ret;
//    bool sampleSpecular = lobeSample < (rs/(rd+rs)).Average();
//    if(!sampleSpecular) {
//	    Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &ret.pdf);
//        ret.wi = dp.VectorToWorld(localWi);
//    } else {
//        Vec3f localH = Sampling::HemisphericalDirectionCosPower(n, angleSample, &ret.pdf);
//        Vec3f localWo = dp.VectorToLocal(wo);
//        Vec3f localWi = ReflectanceUtils::MirrorDirection(localH, localWo);
//        ret.wi = dp.VectorToWorld(localWi);
//    }
//    ret.pdf = SampleSmoothPdf(wo,ret.wi,dp);
//    ret.brdfCos = EvalSmoothCos(wo,ret.wi,dp);
//    return ret;
//}
//
//float NormalizedBlinnPhongReflectance::SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    // if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return 0;
//    if(!ReflectanceUtils::UpperHemisphere(wo,dp)) return 0;
//
//    float pd = (rd/(rd+rs)).Average();
//    float ps = (rs/(rd+rs)).Average();
//
//    Vec3f localWi = dp.VectorToLocal(wi); Vec3f localH = dp.VectorToLocal((wo+wi).GetNormalized());
//    return pd*Sampling::HemisphericalDirectionCosPdf(localWi)+ps*Sampling::HemisphericalDirectionCosPowerPdf(n,localH);
//}
//
//Vec3f WardIsotropicReflectance::EvalSmoothCos(const Vec3f& wo, const Vec3f& wi,
//	const DifferentialGeometry& dp) const {
//		if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//		float NdotH = dp.N % (wo + wi).GetNormalized();
//		float d = acosf(clamp(NdotH,0.0f,1.0f));
//		float expexp = -tanf(d)*tanf(d)/(a*a);
//		return ( rd/PIf + rs * (1.0f/sqrtf((dp.N % wi)*(dp.N % wo))) * (expf(expexp)/(4*PIf*a*a)) ) * ReflectanceUtils::Cos(wi,dp);
//}
//
//Vec3f WardIsotropicReflectance::EvalSmooth(const Vec3f& wo, const Vec3f& wi,
//	const DifferentialGeometry& dp) const {
//		if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//		float NdotH = dp.N % (wo + wi).GetNormalized();
//		float d = acosf(clamp(NdotH,0.0f,1.0f));
//		float expexp = -tanf(d)*tanf(d)/(a*a);
//		return ( rd/PIf + rs * (1.0f/sqrtf((dp.N % wi)*(dp.N % wo))) * (expf(expexp)/(4*PIf*a*a)) );
//}
//
//BxdfSample WardIsotropicReflectance::SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                                              const Vec2f& angleSample, float lobeSample) const {
//	//BUG: using incorrect sampling (hemispherical)
//    BxdfSample ret;
//	Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &ret.pdf);
//    ret.wi = dp.VectorToWorld(localWi);
//    ret.brdfCos = EvalSmoothCos(wo,ret.wi,dp);
//    return ret;
//}
//
//float WardIsotropicReflectance::SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    //BUG: using incorrect sampling (hemispherical)
//	if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return 0;
//
//    // check same hemisphere
//    Vec3f localWi = dp.VectorToLocal(wi);
//    return Sampling::HemisphericalDirectionCosPdf(localWi);
//}
//
//Vec3f CookTorranceReflectance::EvalSmoothCos(const Vec3f& wo, const Vec3f& wi,
//                          const DifferentialGeometry& dp) const {
//    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//    Vec3f H = (wo + wi).GetNormalized();
//	//using lambertian for diffuse
//	Vec3f Pd = (rd/PIf);
//
//	float NdotL = dp.N%wi;
//	float NdotV = dp.N%wo;
//	float VdotH = wo%H;
//	float NdotH = dp.N%H;
//	Vec3f Ps = (rs * _D(clamp(NdotH,0.0f,1.0f)) * _G(NdotH,NdotV,VdotH,NdotL) * _F(VdotH)) / (4.0f*PIf*NdotL*NdotV); 
//    return (Pd + Ps) * ReflectanceUtils::Cos(wi,dp);
//}
//
//Vec3f CookTorranceReflectance::EvalSmooth(const Vec3f& wo, const Vec3f& wi,
//	const DifferentialGeometry& dp) const {
//		if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return Vec3f::Zero();
//
//		Vec3f H = (wo + wi).GetNormalized();
//		//using lambertian for diffuse
//		Vec3f Pd = (rd/PIf);
//
//		float NdotL = dp.N%wi;
//		float NdotV = dp.N%wo;
//		float VdotH = wo%H;
//		float NdotH = dp.N%H;
//		Vec3f Ps = (rs * _D(clamp(NdotH,0.0f,1.0f)) * _G(NdotH,NdotV,VdotH,NdotL) * _F(VdotH)) / (4.0f*PIf*NdotL*NdotV); 
//		return (Pd + Ps);
//}
//
//BxdfSample CookTorranceReflectance::SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                                    const Vec2f& angleSample, float lobeSample) const {
//	//BUG: using incorrect sampling (hemispherical)
//    BxdfSample ret;
//	Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &ret.pdf);
//    ret.wi = dp.VectorToWorld(localWi);
//    ret.brdfCos = EvalSmoothCos(wo,ret.wi,dp);
//    return ret;
//}
//
//float CookTorranceReflectance::SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    //BUG: using incorrect sampling (hemispherical)
//	if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) return 0;
//
//    // check same hemisphere
//    Vec3f localWi = dp.VectorToLocal(wi);
//    return Sampling::HemisphericalDirectionCosPdf(localWi);
//}
//
//float CookTorranceReflectance::_F(float cosThetaD) const {
//	//Schlick's approximation to Fresnel fraction.  Ngan paper says this is as good as full Frenel for fitting Matusik.
//	return f0 + (1.0f-f0)*powf(1.0f-cosThetaD,5.0f);
//}
//
//float CookTorranceReflectance::_D(float cosThetaH) const{
//	float theta_h = acosf(cosThetaH);
//	float expexp = tanf(theta_h)/sigma;
//	return expf(-expexp*expexp) / (sigma*sigma*powf(cosThetaH,4.0f));
//}
//
//float CookTorranceReflectance::_G(float NdotH, float NdotV, float VdotH, float NdotL) const{
//	return min(min(1.0f,(2.0f*NdotH*NdotV)/VdotH),(2.0f*NdotH*NdotL)/VdotH);
//}
//
//Vec3f KajiyaHairReflectance::EvalSmoothCos(const Vec3f& wo, const Vec3f& wi,
//                          const DifferentialGeometry& dp) const {
//    // TODO: put here the checks
//    Vec3f T = dp.N; // cheat
//    float cosTL = T % wi;
//    float cosTV = T % wo;
//    float sinTL = sqrtf(1 - cosTL * cosTL);
//    float sinTV = sqrtf(1 - cosTV * cosTV);
//    float diff = sinTL;
//    float spec = powf(max(0.0f,cosTL * cosTV + sinTL * sinTV), n);
//    return kd * diff + ks * spec;
//}
//
//Vec3f KajiyaHairReflectance::EvalSmooth(const Vec3f& wo, const Vec3f& wi,
//	const DifferentialGeometry& dp) const {
//		// TODO: put here the checks
//		Vec3f T = dp.N; // cheat
//		float cosTL = T % wi;
//		float cosTV = T % wo;
//		float sinTL = sqrtf(1 - cosTL * cosTL);
//		float sinTV = sqrtf(1 - cosTV * cosTV);
//		float diff = sinTL;
//		float spec = powf(max(0.0f,cosTL * cosTV + sinTL * sinTV), n);
//		return kd * diff + ks * spec;
//}
//
//BxdfSample KajiyaHairReflectance::SampleSmoothCos(const Vec3f& wo, const DifferentialGeometry& dp,
//                                                         const Vec2f& angleSample, float lobeSample) const {
//    // TODO: put here the checks
//    BxdfSample ret;
//    Vec3f localWi = Sampling::HemisphericalDirection(angleSample, &ret.pdf);
//    ret.wi = dp.VectorToWorld(localWi);
//    ret.brdfCos = EvalSmoothCos(wo,ret.wi,dp);
//    return ret;
//}
//
//float KajiyaHairReflectance::SampleSmoothPdf(const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp) const {
//    // TODO: put here the checks
//    // check same hemisphere
//    Vec3f localWi = dp.VectorToLocal(wi);
//    return Sampling::HemisphericalDirectionCosPdf(localWi);
//}
