#include <limits.h>
#include <float.h>
#include "range3.h"
#include "range2.h"
#include "range1.h"
#include "range6.h"

template <>
Range6<uint32_t> Range6<uint32_t>::Empty()
{
    return Range6<uint32_t>(Vec6<uint32_t>(UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX), Vec6<uint32_t>(0, 0, 0, 0, 0, 0));
}

template <>
Range6<int> Range6<int>::Empty()
{
    return Range6<int>(Vec6i(INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX), Vec6i(INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN));
}

template <>
Range6<float> Range6<float>::Empty()
{
    return Range6<float>(Vec6f(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX), Vec6f(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX));
}

template <>
Range6<double> Range6<double>::Empty()
{
    return Range6<double>(Vec6d(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX), Vec6d(-DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX));
}

template <>
Range3<uint32_t> Range3<uint32_t>::Empty()
{
    return Range3<uint32_t>(Vec3<uint32_t>(UINT_MAX, UINT_MAX, UINT_MAX), Vec3<uint32_t>(0, 0, 0));
}

template <>
Range3<int> Range3<int>::Empty()
{
    return Range3<int>(Vec3i(INT_MAX, INT_MAX, INT_MAX), Vec3i(INT_MIN, INT_MIN, INT_MIN));
}

template <>
Range3<float> Range3<float>::Empty()
{
    return Range3<float>(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
}

template <>
Range3<double> Range3<double>::Empty()
{
    return Range3<double>(Vec3d(DBL_MAX, DBL_MAX, DBL_MAX), Vec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX));
}

template <>
Range2<uint32_t> Range2<uint32_t>::Empty()
{
    return Range2<uint32_t>(Vec2<uint32_t>(UINT_MAX, UINT_MAX), Vec2<uint32_t>(0, 0));
}

template <>
Range2<int> Range2<int>::Empty()
{
    return Range2<int>(Vec2i(INT_MAX, INT_MAX), Vec2i(INT_MIN, INT_MIN));
}

template <>
Range2<float> Range2<float>::Empty()
{
    return Range2<float>(Vec2f(FLT_MAX, FLT_MAX), Vec2f(-FLT_MAX, -FLT_MAX));
}

template <>
Range2<double> Range2<double>::Empty()
{
    return Range2<double>(Vec2d(DBL_MAX, DBL_MAX), Vec2d(-DBL_MAX, -DBL_MAX));
}

template <>
Range1<uint32_t> Range1<uint32_t>::Empty()
{
    return Range1<uint32_t>(UINT_MAX, 0);
}

template <>
Range1<int> Range1<int>::Empty()
{
    return Range1<int>(INT_MAX, INT_MIN);
}

template <>
Range1<float> Range1<float>::Empty()
{
    return Range1<float>(FLT_MAX, -FLT_MAX);
}

template <>
Range1<double> Range1<double>::Empty()
{
    return Range1<double>(DBL_MAX, -DBL_MAX);
}
