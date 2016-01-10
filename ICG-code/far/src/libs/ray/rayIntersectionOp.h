#ifndef _RAY_INTERSECTION_OP_H_
#define _RAY_INTERSECTION_OP_H_
#include <misc/arrays.h>


template<typename T>
inline T SegmentInterpolate(carray<T> &v, const Vec2i &seg, float u) {
    T &a0 = v[seg[0]];
    T &a1 = v[seg[1]];

    return a0 * (1-u) + a1 * u;
}

struct WithoutSegRadius
{
    template<typename T>
    inline static float ComputeRadius(float r, T* data, const Vec2i &seg)
    {
        return r;
    }
};

struct WithSegRadius
{
    template<typename T>
    inline static float ComputeRadius(float r, T* data, const Vec2i &seg)
    {
        return r;
    }
};

struct WithoutSegTangent
{
    template<typename T>
    inline static Vec3f ComputeTangent(T* data, DifferentialGeometry &dp, float u, const Vec2i &seg)
    {
        return dp.Ng;
    }
};

struct WithSegTangent
{
    template<typename T>
    inline static Vec3f ComputeTangent(T* data, DifferentialGeometry &dp, float u, const Vec2i &seg)
    {
        return SegmentInterpolate(data->tangents, seg, u).GetNormalized();
    }
};

struct WithoutSegUv
{
    template<typename T>
    static Vec2f ComputeTexcoord(T* data, DifferentialGeometry &dp, float u, const Vec2i &seg)
    {
        return dp.uv;
    }
};

struct WithSegUv
{
    template<typename T>
    static Vec2f ComputeTexcoord(T* data, DifferentialGeometry &dp, float u, const Vec2i &seg)
    {
        return SegmentInterpolate(data->uvs, seg, u);
    }
};

template<typename T>
inline T TriangleInterpolate(carray<T> &v, const Vec3i &face, float b1, float b2)
{
    T &n0 = v[face[0]];
    T &n1 = v[face[1]];
    T &n2 = v[face[2]];

    return n0 * (1-b1-b2) + n1 * b1 + n2 * b2;
}

struct WithNormal
{
    template<typename T>
    static Vec3f ComputeNormal(T* data, DifferentialGeometry &dp, float b1, float b2, const Vec3i &f)
    {
        return TriangleInterpolate(data->normals, f, b1, b2).GetNormalized();
    }
};

struct WithoutNormal
{
    template<typename T>
    static Vec3f ComputeNormal(T* data, DifferentialGeometry &dp, float b1, float b2, const Vec3i &f)
    {
        return dp.Ng;
    }
};


struct WithUv
{
    template<typename T>
    static Vec2f ComputeTexcoord(T* data, DifferentialGeometry &dp, float b1, float b2, const Vec3i &f)
    {
        return TriangleInterpolate(data->uvs, f, b1, b2);
    }
};

struct WithoutUv
{
    template<typename T>
    static Vec2f ComputeTexcoord(T* data, DifferentialGeometry &dp, float b1, float b2, const Vec3i &f)
    {
        return dp.uv;
    }
};

#endif // _RAY_INTERSECTION_OP_H_
