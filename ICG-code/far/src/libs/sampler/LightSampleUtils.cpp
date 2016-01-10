#include "LightSampleUtils.h"


namespace LightSampleUtils
{

    shared_ptr<Distribution1Df> ComputeLightPowerDistribution( const vector<shared_ptr<Light> >& lights, float sceneRadius )
    {
        vector<float> powers(lights.size());
        for(uint32_t i = 0; i < lights.size(); i++)
            powers[i] = lights[i]->Power(sceneRadius).Average();
        shared_ptr<Distribution1Df> dist = shared_ptr<Distribution1Df>(new Distribution1Df(&powers[0], (uint32_t)powers.size()));
        return dist;
    }

    LightSample SampleLightUniform(const vector<shared_ptr<Light> >& lights, float s) {
        LightSample sample;
        int l = (int)(s * lights.size());
        l = clamp(l,0,(int)lights.size()-1); // clamp to avoid numerical precision issues
        sample.pdf = 1.0f / lights.size();
        sample.light = lights[l];
        return sample;
    }

    LightSample SampleLightPowerDist( const vector<shared_ptr<Light> >& lights, shared_ptr<Distribution1Df> dist, float s )
    {
        LightSample sample;
        uint64_t l = dist->SampleDiscrete(s, &sample.pdf);
        sample.light = lights[l];
        return sample;
    }

    PhotonSample SamplePhotonPowerDist(const vector<shared_ptr<Light> >& lights, shared_ptr<Distribution1Df> dist, 
        const Vec3f& sceneCenter, float sceneRadius, float ls, const Vec2f& ss, 
        const Vec2f& sa, float time )
    {
        LightSample lightSample = SampleLightPowerDist(lights, dist, ls);
        PhotonSample photonSample = lightSample.light->SamplePhoton(sceneCenter, sceneRadius, ss, sa, time);
        photonSample.pdf *= lightSample.pdf;
        return photonSample;
    }

    PhotonSample SamplePhotonUniform(const vector<shared_ptr<Light> >& lights, 
        const Vec3f& sceneCenter, float sceneRadius, 
        float ls, const Vec2f& ss, const Vec2f& sa, float time) {
            LightSample lightSample = SampleLightUniform(lights, ls);
            PhotonSample photonSample = lightSample.light->SamplePhoton(sceneCenter, sceneRadius, ss, sa, time);
            photonSample.pdf *= lightSample.pdf;
            return photonSample;
    }

};