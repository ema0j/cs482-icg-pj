//#include <arma/armadillo>
#include "NMatUtils.h"
#include "KnnMatrix.h"
#include <vlutil/LightEval.h>
#include <vmath/random.h>
#include "vmath/range1.h"

using namespace LightEvalUtil;

inline float Random01()
{
	static random_mt_engine engine;
    return uniform1D01<float>(engine);
}



void ComputeColNorm::operator()( const blocked_range<uint32_t> &r ) const
{
    for (uint32_t i = r.begin(); i != r.end(); i++)
    {
        Vec3f n;
        for (uint32_t j = 0; j < _nsamples.size(); j++)
        {
            const Vec3f &v = _matrix.ElementAt(i, _nsamples[j]);
            n += v * v;
        }
        _norms[i] = n.Sqrt();
    }
}

void ComputeRowSum::operator()( const blocked_range<uint32_t> &r ) const
{
    for (uint32_t j = r.begin(); j != r.end(); j++)
    {
        Vec3f n;
        for (uint32_t i = 0; i < _cluster.size(); i++)
        {
            const Vec3f &v = _matrix.ElementAt(_cluster[i], _nsamples[j]);
            n += v;
        }
        _rsums[j] = n;
    }
}

void ComputeFullColNorm::operator()( const blocked_range<uint32_t> &r ) const
{
    for (uint32_t i = r.begin(); i != r.end(); i++)
    {
        Vec3f n;
        for (uint32_t j = 0; j < _matrix.Height(); j++)
        {
            const Vec3f &v = _matrix.ElementAt(i, j);
            n += v * v;
        }
        _norms[i] = n.Sqrt();
    }
}
