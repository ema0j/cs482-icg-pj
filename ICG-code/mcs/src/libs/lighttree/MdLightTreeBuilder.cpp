#include "MdLightTreeBuilder.h"
#include "BuilderLightCache.h"

void MdLightTreeBuilder::_SampleLights(vector<shared_ptr<SimpleVirtualLightCache> > &caches, shared_ptr<RayEngine> engine, shared_ptr<Scene> scene, 
    const carray<float> &tsamples, uint32_t indirect, shared_ptr<ReportHandler> report) 
{
    uint32_t nDirect = 0;
    uint32_t nIndirect = 0;
    if (report) report->beginActivity("generate virtual lights");
    for (uint32_t s = 0; s < SAMPLE_FRAME_NUM; s++)
    {
        shared_ptr<SimpleVirtualLightCache> cache = shared_ptr<SimpleVirtualLightCache>(new SimpleVirtualLightCache());

        _c = engine->ComputeBoundingBox().Diagonal() / 16.0f;
        _generator->Generate(indirect, cache, tsamples[s]);
        caches.push_back(cache);

        nDirect += cache->DirectLightNum();
        nIndirect += cache->IndirectLightNum();
    }
    if (report) report->endActivity();

    stringstream sout;
    sout << "total lights: " << nDirect + nIndirect << endl;
    sout << "direct lights: " << nDirect << endl;
    sout << "indirect lights: " << nIndirect << endl;
    if (report) report->message(sout.str());
}
