//////////////////////////////////////////////////////////////////////////
//!
//!	\file    lightgen.cpp
//!	\date    3:3:2010   15:49
//!	\author  Jiawei Ou
//!	
//!	\brief   VirtualLightGenerator definition
//!
//////////////////////////////////////////////////////////////////////////
#include "LightGenerator.h"
#include <scene/scenearchive.h>
#include <scene/light.h>
#include <ray/rayEngine.h>
#include <misc/report.h>
#include <scene/sampling.h>
#include <sampler/pathSampler.h>
#include <sampler/LightSampleUtils.h>

//////////////////////////////////////////////////////////////////////////
// Virtual Light Generator Implementation
VirtualPointLightGenerator::VirtualPointLightGenerator(Scene *scene, RayEngine *engine) 
: _scene(scene), _engine(engine)
{
    const float interval = 0.0f;
    const int timeSamples = 1;

    _scene = scene;
    Range3f box = _engine->ComputeBoundingBox();
    _sceneCenter = box.GetCenter();
    _sceneRadius = box.Diagonal() / 2;

    ComputeLightSamplingCDF(_dist, _scene->Lights(), _sceneRadius);
}

void VirtualPointLightGenerator::ComputeLightSamplingCDF(Distribution1Df &dist, const vector<shared_ptr<Light> > &lights, 
                                                                float sceneRadius)
{
    uint32_t nLights = int(lights.size());
    assert(nLights > 0);
    vector<float>lightPower(nLights, 0.f);
    for (uint32_t i = 0; i < nLights; ++i)
        lightPower[i] = lights[i]->Power(sceneRadius).Average();
	dist.Set(&lightPower[0], nLights);
}


bool VirtualPointLightGenerator::Generate(uint32_t nIndirect, VirtualLightCache *cache, float time, ReportHandler *report)
{
    // cache->Clear();
    _GenerateDirect(cache, time, report);
    _GenerateIndirect(nIndirect, cache, time, report);
    return true;
}

void VirtualPointLightGenerator::_GenerateDirect( VirtualLightCache *cache, float time, ReportHandler *report )
{
    if(report) report->beginActivity("generate direct virtual light");
    vector<shared_ptr<Light> > &lights = _scene->Lights();


    for (uint32_t lightIdx = 0; lightIdx < lights.size(); lightIdx++)
    {
        shared_ptr<Light> light = lights[lightIdx];
        uint32_t direct = light->GetShadowSamples();
        uint32_t samples = _sampler.RoundSamples(direct);

        _sampler.BeginPixel(samples);
        for(uint32_t s = 0; s < samples; s ++)
        {
            shared_ptr<VLightSample> sample = 
                light->SampleVLight(_sceneCenter, _sceneRadius, _sampler.Pixel(), _sampler.Next2D(), time);
            if (sample->IsValid())
                sample->AddToCache(cache, samples);
            _sampler.NextPixelSample();
        }
        _sampler.EndPixel();
        if(report) report->progress((float)lightIdx / lights.size());
    }
    if(report) report->endActivity();
}

float VirtualPointLightGenerator::ClampDistSqr(float distSqr) {
    return max(distSqr, 0.1f);
}

void VirtualPointLightGenerator::_GenerateIndirect(uint32_t indirect, VirtualLightCache *cache, float time, ReportHandler *report)
{
    vector<shared_ptr<Light> > &lights = _scene->Lights();
    if(report) report->beginActivity("generate indirect virtual light");
    while(cache->IndirectLightNum() < indirect)
    {
        //PhotonSample photon = LightSampleUtils::SamplePhotonPowerDist(_scene->Lights(), _dist, _sceneCenter, _sceneRadius, _sampler->Next1D(), _sampler->Next2D(), _sampler->Next2D(), time);
        PhotonSample photon = LightSampleUtils::SamplePhotonUniform(_scene->Lights(), _sceneCenter, _sceneRadius, _sampler.Next1D(), _sampler.Next2D(), _sampler.Next2D(), time);

        if (!photon.Valid())
            continue;
        Ray walkRay(photon.P, photon.wo, time);
        Vec3f alpha = photon.le / (photon.pdf);
        Intersection isect;

		uint32_t maxSpecularDepth = 5;

        while (_engine->Intersect(walkRay, &isect) && !alpha.IsZero())
        {
            Vec3f wo = -walkRay.D;
            
            BxdfUnion msu;
            isect.m->SampleReflectance(isect.dp, msu);
            Bxdf *ms = &msu;
            BxdfSample brdfSample = ms->SampleCos(ALL_BXDF, wo, isect.dp, _sampler.Next2D(), _sampler.Next1D());
            if(!brdfSample.Valid()) break;

			if (brdfSample.delta)
			{
				if (maxSpecularDepth == 0)
					break;
				maxSpecularDepth--;
				alpha *= brdfSample.brdfCos;
				walkRay = Ray(isect.dp.P, brdfSample.wi, isect.rayEpsilon, RAY_INFINITY, time);
				continue;
			}

            Vec3f contrib = alpha * brdfSample.brdfCos / (brdfSample.wi % isect.dp.N);
            if(contrib.IsZero() || cache->IndirectLightNum() >= indirect)
                break;
            cache->AddIndirectLight(isect.dp.P, isect.dp.N, contrib / static_cast<float>(indirect));

            if(report) report->progress((float)cache->IndirectLightNum() / indirect);

            Vec3f contribScale = 
                brdfSample.brdfCos / brdfSample.pdf;

            float rrProb = min(1.0f, contribScale.Average());
            if (_sampler.Next1D() > rrProb)
                break;
            alpha *= contribScale / rrProb;
            walkRay = Ray(isect.dp.P, brdfSample.wi, isect.rayEpsilon, RAY_INFINITY, time);
        }
    }
    if(report) report->endActivity();
}