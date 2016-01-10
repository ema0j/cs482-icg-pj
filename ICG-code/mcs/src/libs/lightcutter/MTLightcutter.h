#ifndef _MT_LIGHT_CUT_INTEGRATOR_H_
#define _MT_LIGHT_CUT_INTEGRATOR_H_
#include <lighttree/LightTree.h>
#include "Lightcutter.h"

class MTLightcutter : public Lightcutter
{
    friend class MTLightcutThread;
public:
    MTLightcutter(LightTree *lightTree, Scene *scene, RayEngine *engine, float error = 0.02f, uint32_t maxCutSize = 1000);
    virtual ~MTLightcutter(void);

    virtual void Lightcut(Image<Vec3f> *image, uint32_t samples, Image<uint32_t> *cutImage, ReportHandler *report = 0);
protected:
    uint32_t                _nCore;
    uint32_t                _curLine;
};

#endif // _MT_LIGHT_CUT_INTEGRATOR_H_
