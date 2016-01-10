
#ifndef _LIGHT_SERIALIZE_GENERATOR_H_
#define _LIGHT_SERIALIZE_GENERATOR_H_

#include "LightGenerator.h"
#include "SerializableLightCache.h"
class LightSerializeGenerator :	public VirtualLightGenerator
{
public:
	LightSerializeGenerator(const string& lightFilename);
	virtual ~LightSerializeGenerator(void);
	virtual bool Generate(uint32_t indirect, VirtualLightCache *cache, float time = 0.0f, ReportHandler *report = 0);
protected:
	SerializableLightCache _cache;
};
#endif // _LIGHT_SERIALIZE_GENERATOR_H_

