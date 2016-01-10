#ifndef _MRCS_CASCADE_H_
#define _MRCS_CASCADE_H_

#include "common.h"

//#define MULTI_REP


class MrcsCascade
{
	friend class CascadeReducedMatrixThread;
	friend struct CascadeMrcsReducedColumnThread;
	friend struct FinalMrcsCascadeThread;
public:
    MrcsCascade(VirtualLightGenerator *gen, Scene *scene, RayEngine *engine);
    ~MrcsCascade(void) {};
	const vector<ScaledLight>&	ScaledLights() const { return _scaledLights; }
    void						Render(uint32_t indirect, Image<Vec3f> *image, uint32_t samples, uint32_t rSamples, uint32_t nClusters, ReportHandler *report = 0);
protected:
    void                        _RenderRows(uint32_t rSamples);
    void                        _MrcsCluster(uint32_t budget, uint32_t samples);
    double                      _ComputeCost( const vector<uint32_t> &cluster, gsl_vector* norms, gsl_matrix* input );
	void						_RenderFinalImage(Image<Vec3f> *image, uint32_t samples);
    void                        _GenerateLights(uint32_t indirect);
    Vec3f                       _RenderCell(uint32_t col, DifferentialGeometry &dp, Vec3f &wo, Material *m, float rayEpsilon);
    void                        _RenderRow(Ray &ray, uint32_t r);
    Vec3f                       _RenderRay(Ray ray, uint32_t s);
//eunah
	void						_MRCSClustering(gsl_matrix* input, RandomPathSamplerStd &sampler, uint32_t budget, vector<vector<uint32_t> > &clusters);
	void						_MRCSReprLight(vector<vector<uint32_t>> &clusters, RandomPathSamplerStd &sampler, vector<Vec3f> &colorNorms, vector<uint32_t> &r_light);
//han
	void                        _ShootGatherPoints(uint32_t width, uint32_t height, uint32_t sample);
	void                        _GroupGatherPoints(uint32_t rows);
	void                        _KdGatherGroup(vector<GatherKdItem>::iterator start, vector<GatherKdItem>::iterator end);
	void                        _FindGatherGroupNeighbors();
	void                        _RenderReducedMatrix(Image<Vec3f> &matrix);
	template<typename T> Vec3f  RenderCell(const T &t, uint32_t col, uint32_t row);
	template<typename T> Vec3f  RenderCell(const T &t, uint32_t col, const GatherPoint &gp);
	void                        _SetBackground(Image<Vec3f> *image);

private:
    float                                   _clamp;
    LightList                               _lightList;
    VirtualLightGenerator					*_generator;
    Scene									*_scene;
    RayEngine								*_engine;
    ReportHandler							*_report;
    Image<Vec3f>                            _matrix;
    vector<ScaledLight>                     _scaledLights;

//han
	StratifiedPathSamplerStd				_sampler;

	float                                   _normScale;
	float									_diagonal;
	uint32_t                                _maxGatherGroupSize;

	vector<GatherPoint>                     _gatherPoints;
	vector<GatherGroup>                     _gpGroups;
	vector<BackgroundPixel>                 _bkPixels;

};

template<typename T>
Vec3f MrcsCascade::RenderCell(const T &t, uint32_t col, uint32_t row)
{
	GatherPoint &gp = _gatherPoints[row];
	return RenderCell(t, col, gp);
}

template<typename T>
Vec3f MrcsCascade::RenderCell(const T &t, uint32_t col, const GatherPoint &gp)
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
#endif // _MRCS_CASCADE_H_
