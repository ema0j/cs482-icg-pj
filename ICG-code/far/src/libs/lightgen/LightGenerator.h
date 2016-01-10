//////////////////////////////////////////////////////////////////////////
//!
//!	\file    LightGenerator.h
//!	\date    24:3:2010   13:13
//!	\author  Jiawei Ou
//!	
//!	\brief   LightGenerator declaration
//!
//////////////////////////////////////////////////////////////////////////
#ifndef _VIRTUAL_LIGHT_GENERATOR_H_
#define _VIRTUAL_LIGHT_GENERATOR_H_

#include <misc/stdcommon.h>
#include <vector>
#include <vmath/vec3.h>
#include <scene/vlight.h>
#include <sampler/Distribution1D.h>
#include <sampler/pathSampler.h>

class Scene;
class RayEngine;
class ReportHandler;
class Light;

//////////////////////////////////////////////////////////////////////////
// Virtual Light Declaration
class VirtualLightGenerator
{
public:
	virtual bool Generate(uint32_t indirect, VirtualLightCache* cache, float time = 0.0f, ReportHandler *report = 0) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Virtual Point Light Declaration
class VirtualPointLightGenerator : public VirtualLightGenerator
{
public:
    VirtualPointLightGenerator(Scene *scene, RayEngine *engine);
    virtual bool Generate(uint32_t indirect, VirtualLightCache* cache, float time = 0.0f, ReportHandler *report = 0);
protected:
    virtual void _GenerateDirect(VirtualLightCache* cache, float time, ReportHandler *report);
    virtual void _GenerateIndirect(uint32_t indirect, VirtualLightCache* cache, float time, ReportHandler *report);
	Vec3f GetSceneCenter() const { return _sceneCenter; }
	float GetSceneRadius() const { return _sceneRadius; }
protected:
	static void ComputeLightSamplingCDF(Distribution1Df &dist, const vector<shared_ptr<Light> > &lights, float sceneRadius);

	Scene							*_scene;
	RayEngine						*_engine;
	StratifiedPathSamplerStd		_sampler;
	Vec3f                           _sceneCenter;
	float                           _sceneRadius;
	Distribution1Df					_dist;
private:
	float							ClampDistSqr(float distSqr);
};

#endif //_VIRTUAL_LIGHT_GENERATOR_H_

