#ifndef _CO_CLUSTER_MATRIX_UTILS_H_
#define _CO_CLUSTER_MATRIX_UTILS_H_

#include <ray/rayEngine.h>
#include <scene/scene.h>
#include <tbbutils/tbbutils.h>
#include <image/image.h>

class KnnMatrix;


struct ComputeColNorm
{
    ComputeColNorm(Image<Vec3f> &matrix, vector<Vec3f> &norms, const vector<uint32_t> &nsamples) 
        : _matrix(matrix), _norms(norms), _nsamples(nsamples) { }
    void operator() (const blocked_range<uint32_t> &r) const;
    vector<Vec3f>                       &_norms;
    Image<Vec3f>				        &_matrix;
    const vector<uint32_t>              &_nsamples;
};

struct ComputeFullColNorm
{
    ComputeFullColNorm(Image<Vec3f> &matrix, vector<Vec3f> &norms) 
        : _matrix(matrix), _norms(norms){ }
    void operator() (const blocked_range<uint32_t> &r) const;
    vector<Vec3f>                       &_norms;
    Image<Vec3f>				        &_matrix;
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
