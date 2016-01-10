#include "reflectance_union.h"


Vec3f BxdfLobe::EvalSmooth(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const
{
    switch (_model)
    {
    case LB_LAMBERT:
        return EvalLambert(dp, wo, wi);
    case LB_PHONG:
        return EvalPhong(dp, wo, wi);
    case LB_COOKTORR:
        return EvalCookTorr(dp, wo, wi);
    }
    return Vec3f::Zero();
}

float BxdfLobe::Pdf( const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f &wi ) const
{
    switch (_model)
    {
    case LB_LAMBERT:
        return LambertPdf(dp, wo, wi);
    case LB_PHONG:
        return PhongPdf(dp, wo, wi);
    case LB_COOKTORR:
        return CookTorrPdf(dp, wo, wi);
    }
    return 0.0f;
}

Vec3f BxdfLobe::Sample( const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf ) const
{
	switch (_model)
    {
    case LB_LAMBERT:
        return SampleLambert(dp, wo, wi, angleSample, pdf);
    case LB_PHONG:
        return SamplePhong(dp, wo, wi, angleSample, pdf);
    case LB_COOKTORR:
        return SampleCookTorr(dp, wo, wi, angleSample, pdf);
	case LB_REFLECT:
        return SampleReflect(dp, wo, wi, pdf);
    case LB_TRANSMIT:
        return SampleTransmit(dp, wo, wi, pdf);
    }
    return Vec3f::Zero();
}

void BxdfLobe::Lambert(const Vec3f &kd)
{
    _type = DIFFUSE_BXDF;
    _model = LB_LAMBERT;
    _rho = kd;
}

void BxdfLobe::Phong(const Vec3f &ks, float n)
{
    _type = GLOSSY_BXDF;
    _model = LB_PHONG;
    _spec = ks;
    _n = n;
}

void BxdfLobe::CookTorr( const Vec3f &ks, float n, float eta)
{
    _type = GLOSSY_BXDF;
    _model = LB_COOKTORR;
    _frType = FR_COND;
    _eta = eta;
    _spec = ks;
    _n = n;
}

void BxdfLobe::Reflect( const Vec3f & kr, float eta )
{
    _type = REFLECT_BXDF;
    _model = LB_REFLECT;
    _frType = FR_DIEL;
    _eta = eta;
    _rho = kr;
}

void BxdfLobe::Transmit( const Vec3f &kt, float eta )
{
    _type = TRANSMIT_BXDF;
    _model = LB_TRANSMIT;
    _frType = FR_DIEL;
    _eta = eta;
    _rho = kt;
}


Vec3f BxdfLobe::EvalLambert(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo, wi, dp)) 
        return Vec3f::Zero();
    return (_rho / PIf);
}

Vec3f BxdfLobe::EvalPhong(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo, wi, dp)) 
        return Vec3f::Zero();
    Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
    return (_spec * (2 + _n) / (2 * PIf)) * powf(clamp((wi % R),0.0f,1.0f), _n);
}

Vec3f BxdfLobe::EvalCookTorr(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo, wi, dp)) 
        return Vec3f::Zero();

    Vec3f H = (wo + wi).GetNormalized();

    float NdotL = dp.N % wi;
    float NdotV = dp.N % wo;
    float VdotH = wo % H;
    float NdotH = dp.N % H;

    float cosThetaH = clamp(NdotH, 0.0f, 1.0f);

    float theta_h = acosf(cosThetaH);
    float expexp = tanf(theta_h) / _n;
    float D = expf(-expexp * expexp) / (_n * _n * powf(cosThetaH, 4.0f));
    float G = min(1.0f, min((2.0f * NdotH * NdotV) / VdotH, (2.0f * NdotH * NdotL) / VdotH));
    Vec3f F = _f0 + (1.0f - _f0) * powf(1.0f - NdotV, 5.0f);
    Vec3f Ps = (_spec * D * G * F) / (4.0f * PIf * NdotV * NdotL);
    return Ps;
}

// BxDF Utility Functions
float FrDiel(float cosi, float cost, const float &etai, const float &etat) 
{
    float Rparl = ((etat * cosi) - (etai * cost)) /
        ((etat * cosi) + (etai * cost));
    float Rperp = ((etai * cosi) - (etat * cost)) /
        ((etai * cosi) + (etat * cost));
    return (Rparl*Rparl + Rperp*Rperp) / 2.f;
}
float BxdfLobe::_Fresnel( float cosi ) const
{
    switch(_frType)
    {
    case FR_NONE:
        return 1.0f;
    case FR_DIEL:
        {
            // Compute Fresnel reflectance for dielectric
            cosi = clamp(cosi, -1.f, 1.f);

            // Compute indices of refraction for dielectric
            bool entering = cosi > 0.;
            float ei = _eta, et = 1.0f;
            if (!entering)
                std::swap(ei, et);

            // Compute _sint_ using Snell's law
            float sint = ei/et * sqrtf(max(0.f, 1.f - cosi*cosi));
            if (sint >= 1.) 
            {
                // Handle total internal reflection
                return 1.;
            }
            else
            {
                float cost = sqrtf(max(0.f, 1.f - sint*sint));
                return FrDiel(fabsf(cosi), cost, ei, et);
            }
        }
    }
    return 1.0f;
}

Vec3f BxdfLobe::SampleLambert( const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf ) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo,dp))
        return Vec3f::Zero();

    BxdfSample ret;
    Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &pdf);
    wi = dp.VectorToWorld(localWi);
    return EvalLambert(dp, wo, wi);
}


Vec3f BxdfLobe::SamplePhong( const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf ) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo,dp))
        return Vec3f::Zero();

    Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
    DifferentialGeometry dpr; 
    dpr.N = R;
    dpr.GenerateTuTv();
    Vec3f localWi = Sampling::HemisphericalDirectionCosPower(_n, angleSample, &pdf);
    wi = dpr.VectorToWorld(localWi).GetNormalized();
    return EvalPhong(dp, wo, wi);
}

Vec3f BxdfLobe::SampleCookTorr( const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf ) const
{
    //BUG: using incorrect sampling (hemispherical)
    if(!ReflectanceUtils::UpperHemisphere(wo,dp))
        return Vec3f::Zero();

    Vec3f localWi = Sampling::HemisphericalDirectionCos(angleSample, &pdf);
    wi = dp.VectorToWorld(localWi);
    return EvalCookTorr(dp, wo, wi);
}

Vec3f BxdfLobe::SampleReflect( const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, float &pdf ) const
{
    wi = ReflectanceUtils::UpperHemisphere(wo, dp) ? ReflectanceUtils::MirrorDirection(dp.N, wo) : -wo; // or let it straight through?
    //float F = _Fresnel(dp.N % wo);
    pdf = 1.0f;
    return _rho;
    //return _rho * F; 
}

Vec3f BxdfLobe::SampleTransmit( const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, float &pdf ) const
{
    Vec3f w = dp.VectorToLocal(wo);
    bool entering = (w.z > 0.0f);

    float eta = entering ? (1.0f / _eta) : _eta;
    float sini2 = 1.0f - w.z * w.z;
    float sint2 = eta * eta * sini2;

    // Handle total internal reflection for transmission
    if (sint2 >= 1.) 
        return Vec3f::Zero();
    float cost = sqrtf(max(0.f, 1.f - sint2));
    if (entering) 
        cost = -cost;
    float sintOverSini = eta;

    Vec3f localWi = Vec3f(sintOverSini * -w.x, sintOverSini * -w.y, cost);
    wi = dp.VectorToWorld(localWi);

    //float F = _Fresnel(dp.N % wo);
    pdf = 1.0f;
    return _rho;
    //return _rho * F;
}

float BxdfLobe::LambertPdf( const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f &wi ) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) 
        return 0;
    Vec3f localWi = dp.VectorToLocal(wi);
    return Sampling::HemisphericalDirectionCosPdf(localWi);
}

float BxdfLobe::PhongPdf( const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f & wi ) const
{
    if(!ReflectanceUtils::UpperHemisphere(wo,dp)) return 0;

    Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
    DifferentialGeometry dpr; 
    dpr.N = R; 
    dpr.GenerateTuTv();

    Vec3f localWiSpec = dpr.VectorToLocal(wi).GetNormalized();
    return Sampling::HemisphericalDirectionCosPowerPdf(_n, localWiSpec);
}

float BxdfLobe::CookTorrPdf( const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f & wi ) const
{
    //BUG: using incorrect sampling (hemispherical)
    if(!ReflectanceUtils::UpperHemisphere(wo,wi,dp)) 
        return 0;
    Vec3f localWi = dp.VectorToLocal(wi);
    return Sampling::HemisphericalDirectionCosPdf(localWi);
}

bool BxdfLobe::IsSmooth() const
{
    return (_type & SMOOTH_BXDF) != 0;
}

bool BxdfLobe::IsDelta() const
{
    return (_type & DELTA_BXDF) != 0;
}

BxdfType BxdfLobe::GetType() const
{
    return _type;
}
