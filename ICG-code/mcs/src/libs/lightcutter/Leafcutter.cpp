#include "Leafcutter.h"
#include <scene/camera.h>
#include <iostream>


Leafcutter::Leafcutter(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, shared_ptr<PathSampler> sampler) 
    : _sampler(sampler), _scene(scene), _engine(engine) {}

Leafcutter::~Leafcutter(void)
{
}


Vec3f Leafcutter::EvalutateLight(const OrientedLight& light, DifferentialGeometry& dp, const Vec3f &wo, Material *ms)
{
    Vec3f wi = (light.position - dp.P).GetNormalized();
    float maxDist = (light.position - dp.P).GetLength();
    float cosAngle = max(0.0f, light.normal % (-wi));
    float lenSqrEst = max(0.1f, (light.position - dp.P).GetLengthSqr());
    //float lenSqrEst = (light.position - dp.P).GetLengthSqr();
    BxdfUnion msu;
    ms->SampleReflectance(dp, msu);
    Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
    Vec3f estimate = brdf * cosAngle / lenSqrEst;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, maxDist, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
        {
            return light.le * estimate;
        }
    }
    return Vec3f::Zero();
}

Vec3f Leafcutter::EvalutateLight(const DirLight& light, DifferentialGeometry& dp, const Vec3f &wo, Material *ms)
{
    Vec3f wi = -light.normal;
    BxdfUnion msu;
    ms->SampleReflectance(dp, msu);
    Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
    Vec3f estimate = brdf;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, RAY_INFINITY, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
        {
            return light.le * estimate;
        }
    }
    return Vec3f::Zero();
}

void* Leafcutter::TreeWalkThread(void *ptr)
{
    Leafcutter *cutter = static_cast<Leafcutter*>(ptr);
    if (!cutter)
        return NULL;

    cutter->TreeWalk();
    return NULL;
}

void Leafcutter::leafcut(shared_ptr<Image<Vec4f> > image, shared_ptr<LightTree > lightTree, uint32_t samples, shared_ptr<ReportHandler> report)
{
    _report = report;
    _image = image;
    _lightTree = lightTree;

    _nCore = pthread_num_processors_np();

    stringstream sout;
    sout << "Leafcut on " << _nCore << " threads";
    if (_report) _report->beginActivity(sout.str());

    uint32_t lightNum = lightTree->DirectionalLightNum() + lightTree->OrientedLightNum();
    image->Alloc(lightNum, samples);
    _curLine = 0;

    _mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&_mutex, NULL);
    vector<pthread_t> threads(_nCore);

    _sampler->BeginPixel(samples);
    for (uint64_t idx = 0; idx < threads.size(); idx++)
    {
        pthread_create(&threads[idx], NULL, TreeWalkThread, this);
    }

    for (uint64_t idx = 0; idx < threads.size(); idx++)
    {
        pthread_join(threads[idx], NULL);
    }
    _sampler->EndPixel();

    pthread_mutex_destroy(&_mutex);
    if (_report) _report->endActivity();
}

void Leafcutter::TreeWalk()
{
    uint32_t j = 0;
    while (true)
    {
        const float time = 0.0f; // lightcut do not support motion blur;
        pthread_mutex_lock(&_mutex);
        if(_report) _report->progress((float)(_curLine)/ (float)(_image->Height()), 1);
        j = _curLine++;
        Vec2f puv = _sampler->Pixel();
        _sampler->NextPixelSample();
        pthread_mutex_unlock(&_mutex);

        if (j >= _image->Height())
            return;

        Ray ray = _scene->MainCamera()->GenerateRay(puv, time);
        EvaluateRow(ray, j);
    }
}

void Leafcutter::EvaluateRow( Ray &r, uint32_t line )
{
    uint32_t idx = 0;
    Ray ray = r;
    Vec3f throughput = Vec3f::One();
    Intersection isect;
    uint32_t depth = 0;
    while(true)
    {
        if(!_engine->Intersect(ray, &isect)) 
            return;

        BxdfUnion msu;
        isect.m->SampleReflectance(isect.dp, msu);
        Bxdf* ms = &msu;

        Vec3f wo = -ray.D;
        DifferentialGeometry &dp = isect.dp;

        if(!ms->HasDelta())
        {
            EvaluateTree(_lightTree->GetOrientedRoot(), dp, wo, isect.m, idx, line);
            EvaluateTree(_lightTree->GetDirectionalRoot(), dp, wo, isect.m, idx, line);
            return;
        }
        else
        {
            if (depth > 2)
                return;

            BxdfSample bxdfSample = ms->SampleCos(DELTA_BXDF, wo, dp, _sampler->Next2D(), _sampler->Next1D());
            if (!bxdfSample.Valid())
                return;
            throughput *= bxdfSample.brdfCos;
            ray = Ray(dp.P, bxdfSample.wi, ray.time);
            depth++;
        }
    }
}

void Leafcutter::leafcut(shared_ptr<Image<Vec4f> > image, shared_ptr<LightTree > lightTree, 
                         DifferentialGeometry & dp, Vec3f wo, Material *m, uint32_t line)
{
    uint32_t idx = 0;
    EvaluateTree(image, lightTree, lightTree->GetOrientedRoot(), dp, wo, m, idx, line);
    EvaluateTree(image, lightTree, lightTree->GetDirectionalRoot(), dp, wo, m, idx, line);
}