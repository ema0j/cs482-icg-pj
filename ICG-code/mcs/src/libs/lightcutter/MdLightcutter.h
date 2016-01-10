#ifndef _MULTIDIMENSIONAL_LIGHTCUTTER_H_
#define _MULTIDIMENSIONAL_LIGHTCUTTER_H_
#include <lighttree/GatherTree.h>
#include <lighttree/LightTree.h>
#include <image/image.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <scene/scene.h>
#include <ray/rayEngine.h>

class MdLightcutter
{
    friend class MTMdLightcutThread;
public:
    MdLightcutter(MdLightTree *lightTree, Scene *scene, RayEngine *engine, uint32_t maxCutSize = 1000);
    ~MdLightcutter(void);
    void Lightcut(Image<Vec3f> *image, Image<uint32_t> *cutImage, uint32_t samples, ReportHandler *report = 0);
protected:
    Vec3f _EvaluateLightcut(GatherNode *node, uint32_t &cutSize);
    Vec3f _EvalutateNode(const OrientedLight*l, const GatherPoint* g);
    Vec3f _EvalutateNode(const DirLight* l, const GatherPoint* g);
    Vec3f _ComputeUpperBound( MdOrientedLightTreeNode* ltNode, GatherNode *gpNode, bool &refineLight);
    Vec3f _ComputeUpperBound( MdDirectionalLightTreeNode* ltNode, GatherNode *gpNode, bool &refineLight);
    Vec3f _BoundMaterialOrientedLight( MdOrientedLightTreeNode* ltNode, GatherNode *gpNode );
    Vec3f _BoundMaterialDirLight( MdDirectionalLightTreeNode* ltNode, GatherNode *gpNode );
	template<typename LIGHT> uint32_t _SelectRandomLight(LIGHT* lights);
    Scene                   *_scene;
    RayEngine               *_engine;
    uint32_t                 _maxCutSize;
    MdLightTree				*_lightTree;
    float                    _clamp;
	RandomPathSamplerStd	 _sampler;
};

template<typename LIGHT>
uint32_t MdLightcutter::_SelectRandomLight(LIGHT* lights)
{
	int matchingComps = 0;
	for (uint32_t i = 0; i < MD_REP_SLOTS; i++)
	{
		if (lights[i])
			matchingComps++;
	}
	
	int which = min<uint32_t>((uint32_t)(_sampler.Next1D() * matchingComps), matchingComps-1);
	for (uint32_t i = 0; i < MD_REP_SLOTS; ++i)
	{
		if (lights[i] && which-- == 0) 
			return i;
	}
	assert(0);
	return 0;
}


#endif // _MULTIDIMENSIONAL_LIGHTCUTTER_H_
