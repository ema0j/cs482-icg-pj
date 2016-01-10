//#include <arma/armadillo>
#include "KnnMatrixImpl.h"
#include "KnnMatrix.h"
#include <vlutil/LightEval.h>
#include <vmath/random.h>
#include "vmath/range1.h"

using namespace LightEvalUtil;


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
