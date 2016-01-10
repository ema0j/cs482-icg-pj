#pragma once

#include <lightgen/LightGenerator.h>
#include <lightgen/LightData.h>
#include <image/image.h>
#include <scene/material.h>
#include <gsl/gsl_matrix_double.h>
#include <gpshoot/GatherPointShooter.h>
#include "vmath/vec6.h"
#include <vmath/random.h>
#include "vmath/fastcone.h"
#include "gsl/gsl_blas.h"
#include "nmatrix\kdtree.h"

struct GatherKdItem
{
	GatherKdItem() : idx(0) {}
	GatherKdItem(uint32_t i, const Vec6f& point) : idx(i), p(point) {}
	uint32_t    idx;
	Vec6f       p;
};

struct GatherGroup
{
	GatherGroup() : bbox(Range3f::Empty()) {}
	vector<uint32_t>	indices;
	uint32_t            seed;
	vector<uint32_t>    neighbors;
	Range3f				bbox;
	Vec3f               normal;
};

struct LightGroup
{
	Vec3f               L;
	Range3f				bbox;
	Vec3f               normal;
	FastConef           cone;
};


struct CloseSeed
{
	void Set(GatherKdItem* p, float d2) { seedKdItem = p; distanceSquared = d2; }
	bool operator<(const CloseSeed &p2) const
	{
		return distanceSquared == p2.distanceSquared ?
			(seedKdItem < p2.seedKdItem) : distanceSquared < p2.distanceSquared;
	}

	GatherKdItem*         seedKdItem;
	float               distanceSquared;
};

#define MAX_FOUND_SEED 64
struct SeedProcess
{
	SeedProcess(uint32_t n) : nLookup(n), foundSeeds(0) { }

	void operator()(GatherKdItem& hpItem, float dist2, float& maxDistSquared) const
	{
		if (foundSeeds < nLookup)
		{
			// Add photon to unordered array of photons
			closeSeeds[foundSeeds].Set(&hpItem, dist2);
			foundSeeds++;
			if (foundSeeds == nLookup)
			{
				std::make_heap(&closeSeeds[0], &closeSeeds[nLookup]);
				maxDistSquared = closeSeeds[0].distanceSquared;
			}
		}
		else
		{
			// Remove most distant photon from heap and add new photon
			std::pop_heap(&closeSeeds[0], &closeSeeds[nLookup]);
			closeSeeds[nLookup - 1].Set(&hpItem, dist2);
			std::push_heap(&closeSeeds[0], &closeSeeds[nLookup]);
			maxDistSquared = closeSeeds[0].distanceSquared;
		}
	}

	mutable CloseSeed   closeSeeds[MAX_FOUND_SEED];
	uint32_t            nLookup;
	mutable uint32_t    foundSeeds;
};

//#define MULTI_REP
struct ScaledLight
{
#ifdef MULTI_REP
	vector<uint32_t>    idx;
	vector<Vec3f>       weight;
#else
	uint32_t    idx;
	Vec3f       weight;
#endif
};

