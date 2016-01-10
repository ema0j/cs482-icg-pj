#ifndef _FAR_SMATH_H_
#define _FAR_SMATH_H_

#include <vmath/consts.h>
#include <vmath/functions.h>

#include <vmath/vec2.h>
#include <vmath/vec3.h>
#include <vmath/vec4.h>
#include <vmath/frustum.h>
#include <vmath/range3.h>
#include <vmath/matrix4.h>
#include <vmath/interval.h>
#include <vmath/vfunctions.h>

struct DifferentialGeometry {
    // geomety components
    Vec3f         P;
    Vec3f         Ng;
    Vec2f         uv;

    // shading components
    Vec3f         N;
    Vec3f         Tu;
    Vec3f         Tv;
    Vec2f         st;

    Vec3f VectorToWorld(const Vec3f& v) const {
        return Tu * v[0] + Tv * v[1] + N * v[2];
    }

    Vec3f PointToWorld(const Vec3f& v) const {
        return P + Tu * v[0] + Tv * v[1] + N * v[2];
    }

    Vec3f VectorToLocal(const Vec3f& v) const {
        return Vec3f(Tu % v, Tv % v, N % v);
    }

    Vec3f PointToLocal(const Vec3f& v) const {
        Vec3f vv = v - P;
        return Vec3f(Tu % vv, Tv % vv, N % vv);
    }

    void GenerateTuTv() {
        Tu = Vec3f::X() ^ N;
        if(Tu == Vec3f::Zero()) {
            Tu = Vec3f::Y() ^ N;
        }
        Tu.Normalize();

        Tv = N ^ Tu;
    }

    // only support rotation, uniform scale and translation
    void Transform(const Matrix4d& m) {
        P = m.TransformPoint(P);
        N = m.TransformNormalNoAdjoint(N).GetNormalized();
        Ng = m.TransformNormalNoAdjoint(Ng).GetNormalized();
        Tu = m.TransformVector(Tu).GetNormalized();
        Tv = m.TransformVector(Tv).GetNormalized();
    }
};

#define RAY_EPSILON 5e-2f
#define RAY_INFINITY 1e12f

struct Ray {
    Vec3f E;
    Vec3f D;

    float tMin;
    float tMax;

    float time;

    Ray() {
        E = Vec3f(0,0,0);
        D = Vec3f(0,0,1);

        tMin = RAY_EPSILON;
        tMax = RAY_INFINITY;

        time = 0;
    }

    Ray(const Vec3f& e, const Vec3f& d, float time) {
        E = e;
        D = d;

        tMin = RAY_EPSILON;
        tMax = RAY_INFINITY;

        this->time = time;
    }

    Ray(const Vec3f& e, const Vec3f& d, float max, float time) {
        E = e;
        D = d;

        tMin = RAY_EPSILON;
        tMax = max - RAY_EPSILON;

        this->time = time;
    }

    Ray(const Vec3f& e, const Vec3f& d, float min, float max, float time) {
        E = e;
        D = d;

        tMin = min;
        tMax = max - min;

        this->time = time;
    }

    Vec3f Eval(float t) const {
        return E + D*t;
    }

    void Transform(const Matrix4d& m) {
        E = m.TransformPoint(E);
        D = m.TransformVector(D);
    }
};

#endif
