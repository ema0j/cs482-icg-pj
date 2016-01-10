
#ifndef VirtualLightDiffuseGenerator_h__
#define VirtualLightDiffuseGenerator_h__

#include "LightGenerator.h"
class VirtualPointLightDiffuseGenerator :
    public VirtualPointLightGenerator
{
public:
    VirtualPointLightDiffuseGenerator(Scene *scene, RayEngine *engine);
    virtual ~VirtualPointLightDiffuseGenerator(void);
protected:
    virtual void _GenerateIndirect(uint32_t indirect, VirtualLightCache *cache, float time, ReportHandler *report);
};

#endif // VirtualLightDiffuseGenerator_h__
