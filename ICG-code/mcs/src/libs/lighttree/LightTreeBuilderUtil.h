#ifndef _LIGHT_TREE_BUILDER_UTIL_H_
#define _LIGHT_TREE_BUILDER_UTIL_H_

#include <vmath/vec3.h>
#include <vmath/range3.h>
#include <vmath/range6.h>
#include "LightTree.h"

namespace LightTreeBuilderUtil
{
	inline Vec6f GetPoint(OrientedLightTreeNode *node, float c) {
		Vec3f center = node->bbox.GetCenter();
		Vec3f direction = node->cone.GetAxis();
		return Vec6f(center, direction * c);
	}

	inline Vec6f GetPoint(DirectionalLightTreeNode *node, float c) {
		return Vec6f(node->bbox.GetCenter(), Vec3f::Zero());
	}

	inline Vec6f GetPoint(OmniDirLightTreeNode *node, float c) {
		return Vec6f(node->bbox.GetCenter(), Vec3f::Zero());
	}

#ifdef MULTI_REP
	inline Vec6f GetPoint(MdOrientedLightTreeNode *node, float c) {
		Vec3f center = node->bbox.GetCenter();
		Vec3f direction = node->cone.GetAxis();
		return Vec6f(center, direction * c);
	}

	inline Vec6f GetPoint(MdDirectionalLightTreeNode *node, float c) {
		return Vec6f(node->bbox.GetCenter(), Vec3f::Zero());
	}

	inline Vec6f GetPoint(MdOmniDirLightTreeNode *node, float c) {
		return Vec6f(node->bbox.GetCenter(), Vec3f::Zero());
	}
#endif // MULTI_REP

	template<typename NodeType>
	struct LightTreeBuildItem
	{
		LightTreeBuildItem() : node(NULL) {}
		LightTreeBuildItem(NodeType *n, float c)
		{
			assert(n);
			node = n;
			point = GetPoint(n, c);
		}
		NodeType                                *node;
		Vec6f									point;
	};


    inline float Distance(const OrientedLightTreeNode *o1, const OrientedLightTreeNode *o2, float c)
    {
        Range3f bbox = Range3f::Union(o1->bbox, o2->bbox);
        FastConef cone = FastConef::Union(o1->cone, o2->cone);
        float a2 = bbox.GetSize().GetLengthSqr();
        float sinBeta = cone.GetAngleSin();
        float d = c * sinBeta;
        Vec3f le = o1->L + o2->L;
        return (le.Average() * (a2 + d * d));
    }

    inline float Distance(const DirectionalLightTreeNode *o1, const DirectionalLightTreeNode *o2, float c)
    {
        Range3f bbox = Range3f::Union(o1->bbox, o2->bbox);
        float a2 = bbox.GetSize().GetLengthSqr();
        Vec3f le = o1->L + o2->L;
        return le.Average() * a2;
    }

    inline float Distance(const OmniDirLightTreeNode *o1, const OmniDirLightTreeNode *o2, float c)
    {
        Range3f bbox = Range3f::Union(o1->bbox, o2->bbox);
        float a2 = bbox.GetSize().GetLengthSqr();
        Vec3f le = o1->L + o2->L;
        return le.Average() * a2;
    }
};
#endif // _LIGHT_TREE_BUILDER_UTIL_H_
