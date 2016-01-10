#include "MTMdLightcutter.h"
#include <scene/camera.h>
#include <scene/background.h>
#include <image/image.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <ray/rayEngine.h>
#include <tbbutils/tbbutils.h>
#include "lighttree/GatherTreeBuilder.h"

#define MAX_RAYTRACE_DEPTH 5

MTMdLightcutter::MTMdLightcutter(MdLightTree *lightTree, Scene *scene, RayEngine *engine, uint32_t maxCutSize) 
	: MdLightcutter(lightTree, scene, engine, maxCutSize) {}

MTMdLightcutter::~MTMdLightcutter(void)
{
}

class MTMdLightcutThread
{
public:
	typedef uint32_t argument_type;

	MTMdLightcutThread(MTMdLightcutter *lcutter, GatherTreeBuilder *gtBuilder, Image<uint64_t> *seeds, Image<Vec3f> *img, Image<uint32_t> *sampleImg, uint32_t s) 
        : lightcutter(lcutter), randSeeds(seeds), image(img), sampleImage(sampleImg), samples(s), gatherTreeBuilder(gtBuilder) {
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
    GatherTreeBuilder				*gatherTreeBuilder;
    MTMdLightcutter                 *lightcutter;
    uint32_t                        samples;
    Vec2f                           pixelSize;
};

void MTMdLightcutThread::operator()( uint32_t j ) const
{
    uint64_t seed = randSeeds->ElementAt(j);
    const float time = 0.0f;
    for(uint32_t i = 0; i < image->Width(); i ++) 
    {
        uint32_t cutSize = 0;
        vector<GatherPoint> points;
		Vec3f background;
        GatherNode *gpRoot = gatherTreeBuilder->Build(i, j, seed, points, &background);
        uint32_t cs = 0;
        Vec3f L;
        if (gpRoot) L = lightcutter->_EvaluateLightcut(gpRoot, cutSize);
        image->ElementAt(i, image->Height() - j - 1) = background + L;
        if(sampleImage) sampleImage->ElementAt(i, sampleImage->Height() - j - 1) = cutSize;
		delete gpRoot;
    }
}

void MTMdLightcutter::Lightcut(Image<Vec3f> *image, Image<uint32_t> *cutImage, uint32_t samples, ReportHandler *report)
{
    _nCore = task_scheduler_init::default_num_threads();
    stringstream sout;
    sout << "Multi-dimensional Lightcut on " << _nCore << " threads.";
    if (report) report->message(sout.str());

    float normScale = _engine->ComputeBoundingBox().Diagonal() / 8.0f;

    shared_ptr<GatherTreeBuilder> gatherTreeBuilder = 
        shared_ptr<GatherTreeBuilder>(new GatherTreeBuilder(_scene, _engine, image->Width(), image->Height(), samples, normScale));

    if (report) report->beginActivity("Multi-thread Lightcutting..");
	
	Image<uint64_t> randSeeds(image->Width(), image->Height());
	for (uint32_t i = 0; i < randSeeds.Size(); i++)
	{
		static minstd_rand0 seeder;
		uint64_t seed = seeder();
		randSeeds.ElementAt(i) = seed; 
	}

    TbbReportCounter counter(image->Height(), report);
    MTMdLightcutThread thread(this, gatherTreeBuilder.get(), &randSeeds, image, cutImage, samples);

    parallel_while<MTMdLightcutThread> w;
    w.run(counter, thread);
    if (report) report->endActivity();

}
