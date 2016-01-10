#include "LightEval.h"

namespace LightEvalUtil
{
    Vec3f EvalL::operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = (light.position - dp.P).GetNormalized();
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        float maxDist = (light.position - dp.P).GetLength();
        float cosAngle = max(0.0f, light.normal % (-wi));
        float lenSqrEst = max(_minGeoTerm, (light.position - dp.P).GetLengthSqr());
        Vec3f L = light.le * ReflectanceUtils::PosCos(wi, dp) * cosAngle / lenSqrEst;
        if(!L.IsZero()) 
        {
            Ray shadowRay(dp.P, wi, rayEpsilon, maxDist, 0.0f);
            if(!engine->IntersectAny(shadowRay))
                return L;
        }
        return Vec3f::Zero();
    }

    Vec3f EvalL::operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = -light.normal;
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        Vec3f L = light.le * ReflectanceUtils::PosCos(wi, dp);
        if(!L.IsZero()) 
        {
            Ray shadowRay(dp.P, wi, rayEpsilon, RAY_INFINITY, 0.0f);
            if(!engine->IntersectAny(shadowRay))
                return L;
        }
        return Vec3f::Zero();
    }


    Vec3f EvalIrrad::operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = (light.position - dp.P).GetNormalized();
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        float maxDist = (light.position - dp.P).GetLength();
        float cosAngle = max(0.0f, light.normal % (-wi));
        float lenSqrEst = max(_minGeoTerm, (light.position - dp.P).GetLengthSqr());
        Vec3f L = light.le * ReflectanceUtils::PosCos(wi, dp) * cosAngle / lenSqrEst;
        return L;
    }

    Vec3f EvalIrrad::operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = -light.normal;
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        Vec3f L = light.le * ReflectanceUtils::PosCos(wi, dp);
        return L;
    }

    Vec3f EvalVis::operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = (light.position - dp.P).GetNormalized();
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        float maxDist = (light.position - dp.P).GetLength();
        float cosAngle = max(0.0f, light.normal % (-wi));
        float lenSqrEst = max(_minGeoTerm, (light.position - dp.P).GetLengthSqr());
        BxdfUnion msu;
        ms->SampleReflectance(dp, msu);
        Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
        Vec3f L = light.le * brdf * cosAngle / lenSqrEst;

        if(!L.IsZero()) 
        {
            Ray shadowRay(dp.P, wi, rayEpsilon, maxDist, 0.0f);
            if(!engine->IntersectAny(shadowRay))
                return Vec3f::One();
        }
        return Vec3f::Zero();
    }

    Vec3f EvalVis::operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = -light.normal;
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        Ray shadowRay(dp.P, wi, rayEpsilon, RAY_INFINITY, 0.0f);
        if(!engine->IntersectAny(shadowRay))
            return Vec3f::One();
        return Vec3f::Zero();
    }

    Vec3f EvalLight::operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = (light.position - dp.P).GetNormalized();
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        float maxDist = (light.position - dp.P).GetLength();
        float cosAngle = max(0.0f, light.normal % (-wi));
        float lenSqrEst = max(_minGeoTerm, (light.position - dp.P).GetLengthSqr());
        BxdfUnion msu;
        ms->SampleReflectance(dp, msu);
        Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
        Vec3f L = light.le * brdf * cosAngle / lenSqrEst;

        if(!L.IsZero()) 
        {
            Ray shadowRay(dp.P, wi, rayEpsilon, maxDist, 0.0f);
            if(!engine->IntersectAny(shadowRay))
                return L;
        }
        return Vec3f::Zero();
    }

    Vec3f EvalLight::operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = -light.normal;
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        BxdfUnion msu;
        ms->SampleReflectance(dp, msu);
        Vec3f L = light.le * msu.EvalSmoothCos(wo, wi, dp);
        if(!L.IsZero()) 
        {
            Ray shadowRay(dp.P, wi, rayEpsilon, RAY_INFINITY, 0.0f);
            if(!engine->IntersectAny(shadowRay))
                return L;
        }
        return Vec3f::Zero();
    }


    Vec3f EvalShading::operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = (light.position - dp.P).GetNormalized();
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        float maxDist = (light.position - dp.P).GetLength();
        float cosAngle = max(0.0f, light.normal % (-wi));
        float lenSqrEst = max(_minGeoTerm, (light.position - dp.P).GetLengthSqr());
        BxdfUnion msu;
        ms->SampleReflectance(dp, msu);
        Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
        Vec3f L = light.le * brdf * cosAngle / lenSqrEst;
        return L;
    }

    Vec3f EvalShading::operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const
    {
        Vec3f wi = -light.normal;
		if(ReflectanceUtils::PosCos(wi, dp) <= 0.0f)
			return Vec3f::Zero();
        BxdfUnion msu;
        ms->SampleReflectance(dp, msu);
        Vec3f L = light.le * msu.EvalSmoothCos(wo, wi, dp);
        return L;
    }
}