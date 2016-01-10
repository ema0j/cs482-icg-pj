#include "LightDiffuseGenerator.h"
#include <scene/light.h>
#include <ray/rayEngine.h>
#include <misc/report.h>
#include <scene/sampling.h>
#include <sampler/pathSampler.h>
#include <sampler/LightSampleUtils.h>


VirtualPointLightDiffuseGenerator::VirtualPointLightDiffuseGenerator(Scene *scene, RayEngine *engine)
    : VirtualPointLightGenerator(scene, engine)
{
}


VirtualPointLightDiffuseGenerator::~VirtualPointLightDiffuseGenerator(void)
{
}

void VirtualPointLightDiffuseGenerator::_GenerateIndirect(uint32_t indirect, VirtualLightCache *cache, float time, ReportHandler *report)
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
            float pdf;

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

            GLPhongApproximation glApprox = isect.m->ApprtoximateAsGLPhong(isect.dp);

            Vec3f localWi = Sampling::HemisphericalDirectionCos(_sampler.Next2D(), &pdf);
            Vec3f wi = isect.dp.VectorToWorld(localWi);
            Vec3f brdfCos = glApprox.Kd * (wi % isect.dp.N);

            if(brdfCos.IsZero()) break;

            Vec3f contrib = alpha * glApprox.Kd;
            if(contrib.IsZero() || cache->IndirectLightNum() >= indirect)
                break;

            cache->AddIndirectLight(isect.dp.P, isect.dp.N, contrib / static_cast<float>(indirect));
            
            if(report) report->progress((float)cache->IndirectLightNum() / indirect);

            Vec3f contribScale = glApprox.Kd * (wi % isect.dp.N) / pdf;

            float rrProb = min(1.0f, contribScale.Average());
            if (_sampler.Next1D() > rrProb)
                break;
            alpha *= contribScale / rrProb;
            walkRay = Ray(isect.dp.P, wi, isect.rayEpsilon, RAY_INFINITY, time);
        }
    }
    if(report) report->endActivity();
}

