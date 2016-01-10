#ifndef _GATHER_TREE_H_
#define _GATHER_TREE_H_
#include <vmath/vec3.h>
#include <vmath/cone.h>
#include <vmath/range3.h>
#include <scene/material.h>
#include <vector>
#include "LightTree.h"
#include "ray/intersection.h"
#include "vmath/random.h"
#include "image/cubemap.h"

struct GatherPoint
{
	Vec3f           emission;
	Vec3f			weight;
	Vec2i           pixel;
	Intersection    isect;
	Vec3f           wo;
};

struct GatherNode
{
    GatherNode();
    inline bool             IsLeaf() const { return left == NULL && right == NULL; }
    GatherNode              *left;
    GatherNode              *right;
    GatherPoint             *gp;

    Range3f                 bbox;
    FastConef               normalCone;
    FastConef               mirrorCone;
    bool                    hasGlossy;

    float                   strength;
    Vec3f                   emission;
    GLPhongApproximation    mat;
};



#endif // _GATHER_TREE_H_
