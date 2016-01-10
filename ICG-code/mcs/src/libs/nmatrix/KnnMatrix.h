#ifndef _KNN_MATRIX_H_
#define _KNN_MATRIX_H_

#include <ray/intersection.h>
#include <lightgen/LightData.h>
#include <image/image.h>
#include <sampler/Distribution1D.h>
#include <lightgen/LightGenerator.h>
#include <vmath/random.h>
#include "kdtree.h"
#include <gpshoot/GatherPointShooter.h>
#include "vmath/vec6.h"
#include "gsl/gsl_matrix_double.h"
#include "gsl/gsl_blas.h"
#include "vmath/fastcone.h"
#include "tbb/spin_mutex.h"

struct GatherKdItem
{
    GatherKdItem() : idx(0) {}
    GatherKdItem(uint32_t i, const Vec6f& point) : idx(i), p(point) {} 
    uint32_t    idx;
    Vec6f       p;
};

struct ScaleLight
{
    vector<uint32_t>    indices;
    vector<Vec3f>       weights;
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
            closeSeeds[nLookup-1].Set(&hpItem, dist2);
            std::push_heap(&closeSeeds[0], &closeSeeds[nLookup]);
            maxDistSquared = closeSeeds[0].distanceSquared;
        }
    }

    mutable CloseSeed   closeSeeds[MAX_FOUND_SEED];
    uint32_t            nLookup;
    mutable uint32_t    foundSeeds;
};


class KnnMatrix
{
    friend class RenderKNNImageThread;
	friend class RenderReducedMatrixThread;
	friend struct FinalRenderThread;
	friend struct FinalGroupRenderThread;
public:
	KnnMatrix(Scene *scene, RayEngine *engine, VirtualLightGenerator *gen, ReportHandler *report = 0);
	~KnnMatrix(void) {};

	void									RenderGatherGroup(Image<Vec3f> *image);
	void                                    Render(Image<Vec3f> *image, Image<uint32_t> *sampleImage, uint32_t samples, uint32_t indirect, uint32_t seedNum, uint32_t budget = 400);

protected:
	template<typename T> Vec3f              RenderCell(const T &t, uint32_t col, uint32_t row );
	template<typename T> Vec3f              RenderCell(const T &t, uint32_t col, const GatherPoint &gp );
	void                                    _KdGatherGroup( vector<GatherKdItem>::iterator start, vector<GatherKdItem>::iterator end);
	void                                    _KmeanGatherGroup( vector<GatherKdItem> items, uint32_t seedNum );
	void                                    _SetBackground(Image<Vec3f> *image);
	void                                    _GroupGatherPoints(uint32_t seedNum);
    void                                    _ShootGatherPoints(uint32_t width, uint32_t height, uint32_t sample);
    void                                    _GenerateLights(uint32_t indirect);
    void                                    _BuildGatherGroupKdTree();
    void                                    _RenderReducedMatrix(carray2<Vec3f> &matrix);
    void                                    _FindGatherGroupNeighbors();

	void									_NewInitialClusters(vector<vector<uint32_t> > &clusters, carray2<Vec3f> &matrix, uint32_t budget);
	void									_InitialClusters(vector<vector<uint32_t> > &clusters, carray2<Vec3f> &matrix, uint32_t budget, bool randProj);
	void									_RefineClusters(vector<vector<uint32_t> > &clusters, carray2<Vec3f> &matrix, uint32_t budget, uint32_t samples, Image<Vec3f> *image, Image<uint32_t> *sampleImage);
    
	ReportHandler							*_report;
    Scene									*_scene;
    RayEngine								*_engine;
    VirtualLightGenerator					*_generator;

	StratifiedPathSamplerStd				_sampler;

    float                                   _normScale;
    float                                   _clamp;
	float									_diagonal;
    uint32_t                                _maxGatherGroupSize;
    
    vector<BackgroundPixel>                 _bkPixels;
    LightList                               _lightList;

	vector<vector<ScaleLight> >				_scaledLights;
	vector<GatherPoint>                     _gatherPoints;
	vector<GatherGroup>                     _gpGroups;
};


template<typename T>
Vec3f KnnMatrix::RenderCell(const T &t, uint32_t col, uint32_t row )
{
    GatherPoint &gp = _gatherPoints[row];
    return RenderCell(t, col, gp);
}

template<typename T>
Vec3f KnnMatrix::RenderCell(const T &t, uint32_t col, const GatherPoint &gp )
{
    Vec3f L;
    switch (_lightList.GetLightType(col))
    {
    case OMNIDIR_LIGHT:
        assert(false); 
        L = Vec3f::Zero();
        break;
    case DIRECTIONAL_LIGHT:
        L = t(*reinterpret_cast<DirLight*>(_lightList.GetLight(col)), 
            gp.isect.dp, gp.wo, gp.isect.m, _engine, gp.isect.rayEpsilon);
        break;
    case ORIENTED_LIGHT:
        L = t(*reinterpret_cast<OrientedLight*>(_lightList.GetLight(col)), 
            gp.isect.dp, gp.wo, gp.isect.m, _engine, gp.isect.rayEpsilon);
        break;
    }
    return L;
}

#endif // _KNN_MATRIX_H_

