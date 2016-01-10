#ifndef _LIGHT_SAMPLE_UTILS_H_
#define _LIGHT_SAMPLE_UTILS_H_
#include "Distribution1D.h"
#include <scene/scene.h>
#include <scene/light.h>

namespace LightSampleUtils
{
    shared_ptr<Distribution1Df> ComputeLightPowerDistribution(const vector<shared_ptr<Light> >& lights, float sceneRadius);

    LightSample SampleLightUniform(const vector<shared_ptr<Light> >& lights, float s);
    LightSample SampleLightPowerDist(const vector<shared_ptr<Light> >& lights, shared_ptr<Distribution1Df> dist, float s);

    PhotonSample SamplePhotonUniform(const vector<shared_ptr<Light> >& lights, 
        const Vec3f& sceneCenter, float sceneRadius, float ls, const Vec2f& ss, const Vec2f& sa, float time);
    PhotonSample SamplePhotonPowerDist(const vector<shared_ptr<Light> >& lights, shared_ptr<Distribution1Df> dist, const Vec3f& sceneCenter, float sceneRadius, float ls, const Vec2f& ss, const Vec2f& sa, float time);
}

#endif // _LIGHT_SAMPLE_UTILS_H_