#ifndef _LIGHT_EVALUATION_H_
#define _LIGHT_EVALUATION_H_
#include <lightgen/LightData.h>
#include <scene/scene.h>
#include <scene/material.h>
#include <ray/rayEngine.h>

#define DEFAULT_MIN_GEO_TERM 0.8f

namespace LightEvalUtil
{
    class EvalFunction
    {
    public:
        EvalFunction(float minGeoTerm = DEFAULT_MIN_GEO_TERM) : _minGeoTerm(minGeoTerm) {}
        float _minGeoTerm;
    };

    class EvalIrrad : public EvalFunction
    {
    public:
        EvalIrrad(float minGeoTerm = DEFAULT_MIN_GEO_TERM) : EvalFunction(minGeoTerm) {}
        Vec3f operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
        Vec3f operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
    };

    class EvalL : public EvalFunction
    {
    public:
        EvalL(float minGeoTerm = DEFAULT_MIN_GEO_TERM) : EvalFunction(minGeoTerm) {}
        Vec3f operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
        Vec3f operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
    };

    class EvalShading : public EvalFunction
    {
    public:
        EvalShading(float minGeoTerm = DEFAULT_MIN_GEO_TERM) : EvalFunction(minGeoTerm) {}
        Vec3f operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
        Vec3f operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
    };

    class EvalVis : public EvalFunction
    {
    public:
        EvalVis(float minGeoTerm = DEFAULT_MIN_GEO_TERM) : EvalFunction(minGeoTerm) {}
        Vec3f operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
        Vec3f operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
    };

    class EvalLight : public EvalFunction
    {
    public:
        EvalLight(float minGeoTerm = DEFAULT_MIN_GEO_TERM) : EvalFunction(minGeoTerm) {}
        Vec3f operator()(const OrientedLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
        Vec3f operator()(const DirLight& light, const DifferentialGeometry& dp, const Vec3f &wo, Material *ms, RayEngine *engine, float rayEpsilon) const;
    };
}
#endif // _LIGHT_EVALUATION_H_