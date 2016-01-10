#ifndef _REFLECTANCE_LOBE_H_
#define _REFLECTANCE_LOBE_H_

#include <scene/reflectance.h>
#include <scene/reflectance_delta.h>
#include <scene/reflectance_smooth.h>

enum LOBE_TYPE 
{
    LB_NONE,
    LB_LAMBERT,
    LB_PHONG,
    LB_COOKTORR,
    LB_REFLECT,
    LB_TRANSMIT
};

enum FR_TYPE
{
    FR_NONE,
    FR_COND,
    FR_DIEL
};

class BxdfLobe
{
public:
	LOBE_TYPE GetModel() const { return _model; }

    BxdfLobe() : _model(LB_NONE), _type(UNKNOWN_BXDF) {}
    
    bool            IsSmooth() const;
    bool            IsDelta() const;

    BxdfType        GetType() const;
    inline bool     MatchesType(BxdfType type) const { return (type & GetType()) != 0; }

    Vec3f           EvalSmooth(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const;
    Vec3f           Sample(const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf) const;
    float           Pdf(const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f &wi) const;

    void            Lambert(const Vec3f &kd);
    void            Phong(const Vec3f &ks, float n);
    void            CookTorr(const Vec3f &ks, float n, float eta = 1.0f);
    void            Reflect( const Vec3f &kr, float eta = 1.0f);
    void            Transmit( const Vec3f &kt, float eta = 1.0f);

    Vec3f           EvalLambert(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const;
    Vec3f           EvalPhong(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const;
    Vec3f           EvalCookTorr(const DifferentialGeometry &dp, const Vec3f &wo, const Vec3f &wi) const;
    
    Vec3f           SampleLambert(const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf) const;
    Vec3f           SamplePhong(const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf) const;
    Vec3f           SampleCookTorr(const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, const Vec2f& angleSample, float &pdf) const;
    Vec3f           SampleReflect(const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, float &pdf) const;
    Vec3f           SampleTransmit(const DifferentialGeometry& dp, const Vec3f& wo, Vec3f& wi, float &pdf) const;

    float           LambertPdf(const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f &wi) const;
    float           PhongPdf( const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f & wi ) const;
    float           CookTorrPdf( const DifferentialGeometry& dp, const Vec3f& wo, const Vec3f & wi ) const;
protected:
    float           _Fresnel( float cosi ) const;

    Vec3f           _rho;
    Vec3f           _spec;
    float           _n;

    float           _f0;
    float           _eta;

    LOBE_TYPE       _model;
    BxdfType        _type;
    FR_TYPE         _frType;
};

#endif
