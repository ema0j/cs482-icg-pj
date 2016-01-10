#include "LightTree.h"
#include <fstream>
#include <vmath/random.h>

OrientedLightTreeNode::OrientedLightTreeNode(OrientedLight &l) : left(0), right(0)
{
    light = &l;
    L = l.le;
    bbox = Range3f(l.position, l.position);
    cone = FastConef(l.normal);
}

DirectionalLightTreeNode::DirectionalLightTreeNode(DirLight &l) : left(0), right(0)
{
    light = &l;
    L = l.le;
    bbox = Range3f(l.normal, l.normal);
}

OmniDirLightTreeNode::OmniDirLightTreeNode(OmniDirLight &l) : left(0), right(0)
{
    light = &l;
    L = l.intensity;
    bbox = Range3f(l.position, l.position);
}

OrientedLightTreeNode::~OrientedLightTreeNode()
{
	if(left) delete left;
	if(right) delete right;
}

DirectionalLightTreeNode::~DirectionalLightTreeNode()
{
	if(left) delete left;
	if(right) delete right;
}

OmniDirLightTreeNode::~OmniDirLightTreeNode()
{
	if(left) delete left;
	if(right) delete right;
}

#ifdef MULTI_REP
MdOrientedLightTreeNode::MdOrientedLightTreeNode(OrientedLight &l, uint32_t idx) : left(0), right(0)
{
	memset(lights, 0, sizeof(lights));
	lights[idx] = &l;
	L = l.le;
	bbox = Range3f(l.position, l.position);
	cone = FastConef(l.normal);
}

MdDirectionalLightTreeNode::MdDirectionalLightTreeNode(DirLight &l, uint32_t idx) : left(0), right(0)
{
	memset(lights, 0, sizeof(lights));
	lights[idx] = &l;
	L = l.le;
	bbox = Range3f(l.normal, l.normal);
}

MdOmniDirLightTreeNode::MdOmniDirLightTreeNode(OmniDirLight &l, uint32_t idx) : left(0), right(0)
{
	memset(lights, 0, sizeof(lights));
	lights[idx] = &l;
	L = l.intensity;
	bbox = Range3f(l.position, l.position);
}

MdOrientedLightTreeNode::~MdOrientedLightTreeNode()
{
	if(left) delete left;
	if(right) delete right;
}

MdDirectionalLightTreeNode::~MdDirectionalLightTreeNode()
{
	if(left) delete left;
	if(right) delete right;
}

MdOmniDirLightTreeNode::~MdOmniDirLightTreeNode()
{
	if(left) delete left;
	if(right) delete right;
}
#endif