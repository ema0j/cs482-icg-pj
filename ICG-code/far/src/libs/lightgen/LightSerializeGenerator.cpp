#include "LightSerializeGenerator.h"


LightSerializeGenerator::LightSerializeGenerator(const string& lightFilename)
{
	_cache.Load(lightFilename);
}

LightSerializeGenerator::~LightSerializeGenerator(void)
{
}

bool LightSerializeGenerator::Generate(uint32_t indirect, VirtualLightCache *cache, float time, ReportHandler *report)
{
	for (uint32_t i = 0; i < _cache._otrLights.size(); i++)
	{
		OrientedLight& ortLight = _cache._otrLights[i];
		cache->AddOrientedLight(ortLight.position, ortLight.normal, ortLight.le);
	}

	for (uint32_t i = 0; i < _cache._dirLights.size(); i++)
	{
		DirLight& dirLight = _cache._dirLights[i];
		cache->AddDirLight(dirLight.normal, dirLight.le);
	}

	for (uint32_t i = 0; i < _cache._ptLights.size(); i++)
	{
		OmniDirLight& ptLight = _cache._ptLights[i];
		cache->AddOmniDirLight(ptLight.position, ptLight.intensity);
	}

	for (uint32_t i = 0; i < _cache._idtLights.size(); i++)
	{
		OrientedLight& idtLight = _cache._idtLights[i];
		cache->AddIndirectLight(idtLight.position, idtLight.normal, idtLight.le);
	}
	return true;
}