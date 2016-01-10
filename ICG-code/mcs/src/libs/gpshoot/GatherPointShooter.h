#ifndef GatherPointShooter_h__
#define GatherPointShooter_h__

#include <ray/rayEngine.h>
#include <scene/scene.h>
#include <scene/camera.h>
#include <iostream>
#include <misc/report.h>
#include <sampler/pathSampler.h>
#include <vlutil/LightEval.h>
#include <scene/background.h>
#include <imageio/imageio.h>

struct GatherPoint 
{
    Vec3f			emission;
    Vec3f			weight;
    Vec2i           pixel;
    Intersection    isect;
    Vec3f           wo;
	float			strength;
    uint32_t        index;
};

struct BackgroundPixel 
{
    Vec3f			background;
    Vec2i           pixel;
    float			strength;
};


namespace GatherPointShooter
{
	void InitRandomSeeds(Image<uint64_t> *randSeeds);
	void Shoot(Scene* scene, RayEngine *engine, uint32_t i, uint32_t j, uint32_t samples, Image<uint64_t> *randSeeds, vector<GatherPoint> &gatherPoint, vector<BackgroundPixel> &bgPixels, ReportHandler *report = 0);
    void Shoot(Scene* scene, RayEngine *engine, uint32_t width, uint32_t height, uint32_t samples, vector<GatherPoint> &gatherPoint, vector<BackgroundPixel> &bgPixels, ReportHandler *report = 0);
};

#endif // GatherPointShooter_h__