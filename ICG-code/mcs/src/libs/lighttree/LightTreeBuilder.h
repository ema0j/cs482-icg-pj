#ifndef _LIGHT_TREE_BUILDER_H_
#define _LIGHT_TREE_BUILDER_H_

#include <lighttree/LightTree.h>
#include <scene/scene.h>
#include <ray/rayEngine.h>
#include <misc/report.h>
#include <scene/vlight.h>
#include <vmath/vec3.h>
#include <vmath/vec6.h>
#include <vmath/range3.h>
#include <vmath/range6.h>
#include <lightgen/LightGenerator.h>
#include "LightTreeBuilderUtil.h"
#include <vmath/random.h>

class LightTreeBuilder
{
public:
	LightTreeBuilder(VirtualLightGenerator *gen) : _generator(gen) {}
	virtual ~LightTreeBuilder(void) {}
protected:
	template<typename T>
	void _SampleLights(T* lightTree, RayEngine *engine, Scene *scene, 
		uint32_t indirect, ReportHandler *report);    
    float                               _c;
    VirtualLightGenerator				*_generator;
};


template<typename T>
void LightTreeBuilder::_SampleLights(T* lightTree, RayEngine *engine, Scene *scene, 
	uint32_t indirect, ReportHandler *report) 
{
	BuilderLightCache<T> cache(lightTree);

	_c = engine->ComputeBoundingBox().Diagonal() / 16.0f;

	if (report) report->beginActivity("generate virtual lights");
	_generator->Generate(indirect, &cache, 0.0f);
	if (report) report->endActivity();

	stringstream sout;
	sout << "total lights: " << cache.DirectLightNum() + cache.IndirectLightNum() << " ";
	sout << "direct lights: " << cache.DirectLightNum() << " ";
	sout << "indirect lights: " << cache.IndirectLightNum() << " ";
	if (report) report->message(sout.str());
}


#endif