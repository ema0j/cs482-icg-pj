#ifndef _SMALL_LIGHT_TREE_BUILDER_H_
#define _SMALL_LIGHT_TREE_BUILDER_H_
#include "LightTree.h"
#include "vmath/range1.h"
#include "scene/vlight.h"
#include "vmath/range6.h"

struct NodePoint
{
    Vec6f       point;
    uint32_t    idx;
};

class LightTreeBuilder
{
public:
    LightTreeBuilder(void) {};
    ~LightTreeBuilder(void) {};
    shared_ptr<LightTree> Build(LightList &lightList, shared_ptr<RayEngine> engine, float normScale, float clamp);

    OrientedLightTreeNode* _BuildOrtLight( vector<NodePoint>::iterator start, vector<NodePoint>::iterator end, LightList &lightList);
    DirectionalLightTreeNode* _BuildDirLight( vector<NodePoint>::iterator start, vector<NodePoint>::iterator end, LightList &lightList);

    Vec6f ComputePoint( VLight *light, VL_TYPE type, float normScale );
};



#endif // _LIGHT_TREE_BUILDER_H_
