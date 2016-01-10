#include "GatherPointShooter.h"
#include "vlutil/LightEval.h"
#include <tbbutils/tbbutils.h>
#include <sampler/pathSampler.h>

using namespace LightEvalUtil;

class ShootGatherPointThread
{
public:
    typedef uint32_t argument_type;

    ShootGatherPointThread(Scene* scene, RayEngine *engine, const Image<uint64_t> *randomSeeds, uint32_t width, uint32_t height, uint32_t samples);
    void operator()(uint32_t j) const;
    void operator()(const blocked_range2d<uint32_t> &r) const;
    void Init(void);
    void Clean(void);
    static concurrent_vector<GatherPoint>      _gatherPoints;
    static concurrent_vector<BackgroundPixel>  _bkPixels;
private:
    void _Trace(const Vec2i &pixel, const Vec2f &offset, const Vec2f &auv, const Vec2f &pixelSize, uint32_t index, float time ) const;
    uint32_t                            _width;
    uint32_t                            _height;
    uint32_t                            _samples;
    Vec2f                               _pixelSize;
    uint32_t                            _nss;
    const Image<uint64_t>               *_randSeeds;
    Scene                               *_scene;
    RayEngine                           *_engine;
};


ShootGatherPointThread::ShootGatherPointThread(Scene* scene, RayEngine *engine, const Image<uint64_t> *randomSeeds, uint32_t width, uint32_t height, uint32_t samples)
    : _scene(scene), _engine(engine), _width(width), _height(height), _samples(samples), _randSeeds(randomSeeds)
{
    _pixelSize = Vec2f(1.0f / _width, 1.0f / _height); 
}

void ShootGatherPointThread::Init()
{
    _gatherPoints.clear();
    _bkPixels.clear();
    _gatherPoints.reserve(_width * _height * _samples);
}

void ShootGatherPointThread::Clean()
{
    _gatherPoints.clear();
    _gatherPoints.shrink_to_fit();
    _bkPixels.clear();
    _bkPixels.shrink_to_fit();
}

void ShootGatherPointThread::operator()(uint32_t j) const 
{
	uint64_t seed = _randSeeds->ElementAt(j);
	StratifiedPathSamplerStd::Engine e(seed);
	StratifiedPathSamplerStd sampler(e);
    for (uint32_t i = 0; i < _width; i++)
    {
        Vec2i pixel(i, j);
        if (_samples == 1)
            _Trace(pixel, Vec2f((float)0.5f, (float)0.5f), Vec2f((float)0.5f, (float)0.5f), _pixelSize, 0, 0.0f);
        else
        {
            sampler.BeginPixel(_samples);
            for (uint32_t s = 0; s < _samples; s++)
            {
                Vec2i pixel(i, j);
                _Trace(pixel, sampler.Pixel(), sampler.Lens(), _pixelSize, s, sampler.Time());
                sampler.NextPixelSample();
            }
            sampler.EndPixel();
        }
    }
}

void ShootGatherPointThread::operator()(const blocked_range2d<uint32_t> &r) const 
{
    const float time = 0.0f;
    for (uint32_t j = r.rows().begin(); j != r.rows().end(); j++)
    {
        for (uint32_t i = r.cols().begin(); i != r.cols().end(); i++)
        {
            Vec2i pixel(i, j);
            if (_samples == 1)
                _Trace(pixel, Vec2f((float)0.5f, (float)0.5f), Vec2f((float)0.5f, (float)0.5f), _pixelSize, 0, 0.0f);
            else
            {
				uint64_t seed = _randSeeds->ElementAt(i, j);
				StratifiedPathSamplerStd::Engine e(seed);
				StratifiedPathSamplerStd sampler(e);
                sampler.BeginPixel(_samples);
                for (uint32_t s = 0; s < _samples; s++)
                {
                    Vec2i pixel(i, j);
                    _Trace(pixel, sampler.Pixel(), sampler.Lens(), _pixelSize, s, sampler.Time());
                    sampler.NextPixelSample();
                }
                sampler.EndPixel();
            }
        }
    }
}

concurrent_vector<GatherPoint>          ShootGatherPointThread::_gatherPoints;
concurrent_vector<BackgroundPixel>      ShootGatherPointThread::_bkPixels;

void ShootGatherPointThread::_Trace( const Vec2i &pixel, const Vec2f &offset, const Vec2f &auv, const Vec2f &pixelSize, uint32_t index, float time ) const
{
    Vec2f puv = (Vec2f(pixel) + offset) * pixelSize;
    Ray ray = _scene->MainCamera()->GenerateRay(puv, auv, time);
    GatherPoint gp;

    Vec3f throughput = Vec3f::One();
    bool hit = false;

    uint32_t maxSpecDepth = 10;
    while(_engine->Intersect(ray, &gp.isect))
    {
        BxdfUnion msu;
        gp.isect.m->SampleReflectance(gp.isect.dp, msu);
        if(msu.HasDelta())
        {
            if (maxSpecDepth == 0)
            {
                hit = true;
                break;
            }
            maxSpecDepth--;
            BxdfSample sample = msu.SampleCos(DELTA_BXDF, -ray.D, gp.isect.dp, Vec2f::Zero(), 0.0f);
            throughput *= sample.brdfCos;
            ray = Ray(gp.isect.dp.P, sample.wi, gp.isect.rayEpsilon, RAY_INFINITY, ray.time);
            continue;
        }
        else if (msu.HasSmooth())
        {
            gp.emission = msu.Emission(gp.wo, gp.isect.dp);
            gp.pixel = pixel;
            gp.wo = -ray.D;
            gp.weight = throughput;
            gp.strength = 1.0f / _samples;
            gp.index = index;
            _gatherPoints.push_back(gp);
            hit = true;
        }
        break;
    }
    if (!hit)
    {
        if(_scene->MainBackground()) 
        {	
            BackgroundPixel bkPixel;
            bkPixel.pixel = pixel;
            bkPixel.background = 
                throughput * _scene->MainBackground()->SampleBackground(ray.D, ray.time);
            bkPixel.strength = 1.0f / _samples;
            _bkPixels.push_back(bkPixel);
        }
    }
}


void GatherPointShooter::InitRandomSeeds(Image<uint64_t> *randSeeds)
{
    for (uint32_t i = 0; i < randSeeds->Size(); i++)
    {
        static minstd_rand0 seeder;
        uint64_t seed = seeder();
        randSeeds->ElementAt(i) = seed; 
    }
}

void GatherPointShooter::Shoot(Scene* scene, RayEngine *engine, uint32_t width, uint32_t height, uint32_t samples, 
	vector<GatherPoint> &gatherPoints, vector<BackgroundPixel> &bgPixels, ReportHandler *report /*= shared_ptr<ReportHandler>()*/ )
{
	Image<uint64_t> randSeeds(width, height);
	InitRandomSeeds(&randSeeds);

    ShootGatherPointThread thread(scene, engine, &randSeeds, width, height, samples);
    thread.Init();

    parallel_while<ShootGatherPointThread> w;
    TbbReportCounter counter(height, report, 10);
    w.run( counter, thread );

    //parallel_for(blocked_range2d<uint32_t, uint32_t>(0, height, 0, width), thread);

    bgPixels.assign(thread._bkPixels.begin(), thread._bkPixels.end());
    gatherPoints.assign(thread._gatherPoints.begin(), thread._gatherPoints.end());

    //thread.Clean();
}

void _Trace(Scene* scene, RayEngine *engine, const Vec2i &pixel, const Vec2f &offset, const Vec2f &auv, const Vec2f &pixelSize, uint32_t index, uint32_t samples, float time, vector<GatherPoint> &gatherPoints, vector<BackgroundPixel> &backPxs)
{
	Vec2f puv = (Vec2f(pixel) + offset) * pixelSize;
	Ray ray = scene->MainCamera()->GenerateRay(puv, auv, time);
	GatherPoint gp;

	Vec3f throughput = Vec3f::One();
	bool hit = false;

	uint32_t maxSpecDepth = 10;
	while(engine->Intersect(ray, &gp.isect))
	{
		BxdfUnion msu;
		gp.isect.m->SampleReflectance(gp.isect.dp, msu);
		if(msu.HasDelta())
		{
			if (maxSpecDepth == 0)
			{
				hit = true;
				break;
			}
			maxSpecDepth--;
			BxdfSample sample = msu.SampleCos(DELTA_BXDF, -ray.D, gp.isect.dp, Vec2f::Zero(), 0.0f);
			throughput *= sample.brdfCos;
			ray = Ray(gp.isect.dp.P, sample.wi, gp.isect.rayEpsilon, RAY_INFINITY, ray.time);
			continue;
		}
		else if (msu.HasSmooth())
		{
			gp.emission = msu.Emission(gp.wo, gp.isect.dp);
			gp.pixel = pixel;
			gp.wo = -ray.D;
			gp.weight = throughput;
			gp.strength = 1.0f / samples;
			gp.index = index;
			gatherPoints.push_back(gp);
			hit = true;
		}
		break;
	}
	if (!hit)
	{
		if(scene->MainBackground()) 
		{	
			BackgroundPixel bkPixel;
			bkPixel.pixel = pixel;
			bkPixel.background = 
				throughput * scene->MainBackground()->SampleBackground(ray.D, ray.time);
			backPxs.push_back(bkPixel);
		}
	}
}

void Shoot(Scene* scene, RayEngine *engine, uint32_t i, uint32_t j, uint32_t samples, Image<uint64_t> *randSeeds, vector<GatherPoint> &gatherPoint, vector<BackgroundPixel> &bgPixels, ReportHandler *report)
{
	Vec2f pixelSize(1.0f / randSeeds->Width(), 1.0f / randSeeds->Height()); 
	Vec2i pixel(i, j);
	if (samples == 1)
		_Trace(scene, engine, pixel, Vec2f((float)0.5f, (float)0.5f), Vec2f((float)0.5f, (float)0.5f), pixelSize, 0, 1, 0.0f, gatherPoint, bgPixels);
	else
	{
		uint64_t seed = randSeeds->ElementAt(i, j);
		StratifiedPathSamplerStd::Engine e(seed);
		StratifiedPathSamplerStd sampler(e);
		sampler.BeginPixel(samples);
		for (uint32_t s = 0; s < samples; s++)
		{
			Vec2i pixel(i, j);
			_Trace(scene, engine, pixel, sampler.Pixel(), sampler.Lens(), pixelSize, s, samples, sampler.Time(), gatherPoint, bgPixels);
			sampler.NextPixelSample();
		}
		sampler.EndPixel();
	}
}