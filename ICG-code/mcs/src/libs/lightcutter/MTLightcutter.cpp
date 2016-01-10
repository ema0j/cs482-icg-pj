#include "MTLightcutter.h"
#include <scene/camera.h>
#include <scene/background.h>
#include <image/image.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <ray/rayEngine.h>
#include <tbbutils/tbbutils.h>

#define MAX_RAYTRACE_DEPTH 5

MTLightcutter::MTLightcutter(LightTree *lightTree, Scene *scene, RayEngine *engine, float error, uint32_t maxCutSize) 
: Lightcutter(lightTree, scene, engine, error, maxCutSize) {}

MTLightcutter::~MTLightcutter(void)
{
}

class MTLightcutThread
{
public:
    typedef uint32_t argument_type;

    MTLightcutThread(MTLightcutter *lcutter, Image<Vec3f> *img, Image<uint32_t> *sampleImg, Image<uint64_t> *seeds, uint32_t s) 
        : lightcutter(lcutter), image(img), sampleImage(sampleImg), randSeeds(seeds), samples(s)
    {
        scene = lightcutter->_scene;
        engine = lightcutter->_engine;
        pixelSize = Vec2f(1.0f/image->Width(), 1.0f/image->Height());
    }
    void operator()(uint32_t j) const;
private:
    Image<Vec3f>					*image;
    Image<uint32_t>					*sampleImage;
	Image<uint64_t>					*randSeeds;
    Scene							*scene;
    RayEngine						*engine;
    MTLightcutter                   *lightcutter;
    uint32_t                        samples;
    Vec2f                           pixelSize;
};

void MTLightcutThread::operator()( uint32_t j ) const
{
	uint64_t seed = randSeeds->ElementAt(j);
	StratifiedPathSamplerStd::Engine e(seed);
	StratifiedPathSamplerStd sampler(e);

    const float time = 0.0f;
    for(uint32_t i = 0; i < image->Width(); i ++) 
    {
        sampler.BeginPixel(samples);
        uint32_t cutSize = 0;
        Vec3f L;
        for (uint32_t s = 0; s < samples; s++)
        {
            uint32_t cs = 0;
            Vec2f pixel(Vec2i(i,j));
            Vec2f puv = (pixel + ((samples == 1) ? Vec2f(0.5f, 0.5f) : sampler.Pixel())) * pixelSize;
            Ray ray = scene->MainCamera()->GenerateRay(puv, time);
            L += lightcutter->EvaluateLightcut(ray, cs) / static_cast<float>(samples);
            cutSize += cs;
            sampler.NextPixelSample();
        }
        sampler.EndPixel();
        image->ElementAt(i, image->Height() - j - 1) = L;
        if(sampleImage) sampleImage->ElementAt(i, sampleImage->Height() - j - 1) = cutSize;
    }
}

void MTLightcutter::Lightcut(Image<Vec3f> *image, uint32_t samples, Image<uint32_t> *cutImage, ReportHandler *report)
{
    _nCore = task_scheduler_init::default_num_threads();
    stringstream sout;
    sout << "Lightcut on " << _nCore << " threads.";
    if (report) report->message(sout.str());

	Image<uint64_t> randSeeds(image->Width(), image->Height());
	for (uint32_t i = 0; i < randSeeds.Size(); i++)
	{
		static minstd_rand0 seeder;
		uint64_t seed = seeder();
		randSeeds.ElementAt(i) = seed; 
	}

    if (report) report->beginActivity("Multi-thread Lightcutting..");
    TbbReportCounter counter(image->Height(), report);
    MTLightcutThread thread(this, image, cutImage, &randSeeds, samples);

    parallel_while<MTLightcutThread> w;
    w.run(counter, thread);
    if (report) report->endActivity();

}
