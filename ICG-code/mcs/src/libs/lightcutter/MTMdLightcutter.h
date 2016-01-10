#ifndef _MT_MD_LIGHT_CUT_INTEGRATOR_H_
#define _MT_MD_LIGHT_CUT_INTEGRATOR_H_
#include <lighttree/LightTree.h>
#include "MdLightcutter.h"

class MTMdLightcutter : public MdLightcutter
{
    friend class MTMdLightcutThread;
public:
    MTMdLightcutter(MdLightTree *lightTree, Scene *scene, RayEngine *engine, uint32_t maxCutSize = 5000);
    virtual ~MTMdLightcutter(void);

    virtual void Lightcut(Image<Vec3f> *image, Image<uint32_t> *cutImage, uint32_t samples, ReportHandler *report = 0);
protected:
    uint32_t                _nCore;
    uint32_t                _curLine;
};

#endif // _MT_MD_LIGHT_CUT_INTEGRATOR_H_
