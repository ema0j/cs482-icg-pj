#ifndef _CO_CLUSTER_MATRIX_UTILS_H_
#define _CO_CLUSTER_MATRIX_UTILS_H_

#include <ray/rayEngine.h>
#include <scene/scene.h>
#include <tbbutils/tbbutils.h>
#include <image/image.h>

class KnnMatrix;


struct ComputeFullColNorm
{
	ComputeFullColNorm(carray2<Vec3f> &matrix, carray<Vec3f> &norms) 
		: _matrix(matrix), _norms(norms){ }
	void operator() (const blocked_range<uint32_t> &r) const {
		for (uint32_t i = r.begin(); i != r.end(); i++)
		{
			Vec3f n;
			for (uint32_t j = 0; j < _matrix.height(); j++)
			{
				const Vec3f &v = _matrix.at(i, j);
				n += v * v;
			}
			_norms.at(i) = n.Sqrt();
		}
	}
	carray<Vec3f>                       &_norms;
	carray2<Vec3f>				        &_matrix;
};

struct ComputeRowSum
{
    ComputeRowSum(Image<Vec3f> &matrix, const vector<uint32_t> &cluster, const vector<uint32_t> &nsamples, vector<Vec3f> &norms) 
        : _matrix(matrix), _rsums(norms), _nsamples(nsamples), _cluster(cluster) { }
    void operator() (const blocked_range<uint32_t> &r) const;
    vector<Vec3f>                       &_rsums;
    Image<Vec3f>				        &_matrix;
    const vector<uint32_t>              &_nsamples;
    const vector<uint32_t>              &_cluster;
};

#endif // _CO_CLUSTER_MATRIX_UTILS_H_
