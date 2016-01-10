#include "reflectance_union.h"


void BxdfUnion::AddLambert( const Vec3f &kd )
{
    if (nLobes >= MAX_LOBE_NUM)
        assert(0);
    lobes[nLobes].Lambert(kd);
    nLobes++;
}

void BxdfUnion::AddPhong( const Vec3f &ks, float n )
{
    if (nLobes >= MAX_LOBE_NUM)
        assert(0);
    lobes[nLobes].Phong(ks, n);
    nLobes++;
}

void BxdfUnion::AddCookTorr(const Vec3f &ks, float n, float eta)
{
    if (nLobes >= MAX_LOBE_NUM)
        assert(0);
    lobes[nLobes].CookTorr(ks, n, eta);
    nLobes++;
}

void BxdfUnion::AddReflect( const Vec3f &kr, float eta )
{
    if (nLobes >= MAX_LOBE_NUM)
        assert(0);
    lobes[nLobes].Reflect(kr, eta);
    nLobes++;
}

void BxdfUnion::AddTransmit( const Vec3f &kt, float eta )
{
    if (nLobes >= MAX_LOBE_NUM)
        assert(0);
    lobes[nLobes].Transmit(kt, eta);
    nLobes++;
}

bool BxdfUnion::HasSmooth() const
{
    for (uint32_t i = 0; i < MAX_LOBE_NUM; i++)
    {
        if(lobes[i].IsSmooth())
            return true;
    }
    return false;
}

bool BxdfUnion::HasDelta() const
{
    for (uint32_t i = 0; i < MAX_LOBE_NUM; i++)
    {
        if(lobes[i].IsDelta())
            return true;
    }
    return false;
}

Vec3f BxdfUnion::EvalSmooth(const Vec3f &wo, const Vec3f &wi, const DifferentialGeometry &dp ) const
{
    Vec3f brdf;
    for (uint32_t i = 0; i < LobeNumber(); i++)
    {
        if (lobes[i].IsSmooth())
            brdf += lobes[i].EvalSmooth(dp, wo, wi);
    }
    return brdf;
}

Vec3f BxdfUnion::EvalSmoothCos(const Vec3f &wo, const Vec3f &wi, const DifferentialGeometry &dp ) const
{
    return EvalSmooth(wo, wi, dp) * ReflectanceUtils::PosCos(wi, dp);
}

uint32_t BxdfUnion::MatchingLobeNum(BxdfType type) const
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < LobeNumber(); i++)
    {
        if (lobes[i].MatchesType(type))
            count++;
    }
    return count;
}


BxdfSample BxdfUnion::SampleCos( BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample ) const
{
    BxdfSample sample;
    // Choose which _BxDF_ to sample
    int matchingComps = MatchingLobeNum(type);
    if (matchingComps == 0)
        return BxdfSample();

    int which = min<uint32_t>((uint32_t)(lobeSample * matchingComps), matchingComps-1);
    int count = which;

    uint32_t lobeIdx = -1;
    for (uint32_t i = 0; i < LobeNumber(); ++i)
    {
        if (lobes[i].MatchesType(type) && count-- == 0) 
        {
            lobeIdx = i;
            break;
        }
    }
    assert(lobeIdx != -1);
    const BxdfLobe &lobe = lobes[lobeIdx];

    // Sample chosen _BxDF_
    Vec3f brdf = lobe.Sample(dp, wo, sample.wi, angleSample, sample.pdf);

    if (sample.pdf == 0.0f)
        return BxdfSample();

    // Compute overall PDF with all matching _BxDF_s
    if (!(lobe.GetType() & DELTA_BXDF) && matchingComps > 1)
    {
        for (uint32_t i = 0; i < LobeNumber(); i++)
        {
            if (i != lobeIdx && lobes[i].MatchesType(type))
                sample.pdf += lobes[i].Pdf(dp, wo, sample.wi);
        }
    }

    if (matchingComps > 1)
        sample.pdf /= matchingComps;

    // Compute value of BSDF for sampled direction
    if (!(lobe.GetType() & DELTA_BXDF))
    {
        for (uint32_t i = 0; i < LobeNumber(); ++i)
        {
            if (i != lobeIdx && lobes[i].MatchesType(type))
                brdf += lobes[i].EvalSmooth(dp, wo, sample.wi);
        }
    }

    if (!(lobe.GetType() & DELTA_BXDF))
    {
        sample.delta = false;
        sample.brdfCos = brdf * ReflectanceUtils::PosCos(sample.wi, dp);
    }
    else
    {
        sample.delta = true;
        sample.brdfCos = brdf;
    }

    return sample;
}

float BxdfUnion::Pdf( BxdfType type, const Vec3f& wo, const Vec3f& wi, const DifferentialGeometry& dp ) const
{
    if (LobeNumber() == 0) 
        return 0.0f;
    float pdf = 0.f;
    int matchingComps = 0;
    for (uint32_t i = 0; i < LobeNumber(); ++i)
    {
        if (lobes[i].MatchesType(type)) 
        {
            ++matchingComps;
            pdf += lobes[i].Pdf(dp, wo, wi);
        }
    }
    float v = matchingComps > 0 ? pdf / matchingComps : 0.f;
    return v;
}

BxdfSample BxdfUnion::Sample( BxdfType type, const Vec3f& wo, const DifferentialGeometry& dp, const Vec2f& angleSample, float lobeSample ) const
{
    BxdfSample sample;
    // Choose which _BxDF_ to sample
    int matchingComps = MatchingLobeNum(type);
    if (matchingComps == 0)
        return BxdfSample();

    int which = min<uint32_t>((uint32_t)(lobeSample * matchingComps), matchingComps-1);
    int count = which;

    uint32_t lobeIdx = -1;
    for (uint32_t i = 0; i < LobeNumber(); ++i)
    {
        if (lobes[i].MatchesType(type) && count-- == 0) 
        {
            lobeIdx = i;
            break;
        }
    }
    assert(lobeIdx != -1);
    const BxdfLobe &lobe = lobes[lobeIdx];

    // Sample chosen _BxDF_
    Vec3f brdf = lobe.Sample(dp, wo, sample.wi, angleSample, sample.pdf);

    if (sample.pdf == 0.0f)
        return BxdfSample();

    // Compute overall PDF with all matching _BxDF_s
    if (!(lobe.GetType() & DELTA_BXDF) && matchingComps > 1)
    {
        for (uint32_t i = 0; i < LobeNumber(); i++)
        {
            if (i != lobeIdx && lobes[i].MatchesType(type))
                sample.pdf += lobes[i].Pdf(dp, wo, sample.wi);
        }
    }

    if (matchingComps > 1)
        sample.pdf /= matchingComps;

    // Compute value of BSDF for sampled direction
    if (!(lobe.GetType() & DELTA_BXDF))
    {
        brdf = Vec3f::Zero();
        for (uint32_t i = 0; i < LobeNumber(); ++i)
        {
            if (lobes[i].MatchesType(type))
                brdf += lobes[i].EvalSmooth(dp, wo, sample.wi);
        }
    }

    if (!(lobe.GetType() & DELTA_BXDF))
        sample.delta = false;
    else
        sample.delta = true;

    sample.brdfCos = brdf;

    return sample;
}
