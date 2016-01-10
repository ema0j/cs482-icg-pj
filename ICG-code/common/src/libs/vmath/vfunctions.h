#ifndef _VFUNCTIONS_H_
#define _VFUNCTIONS_H_

#include "consts.h"


inline float SphericalTheta(const Vec3f &v) {
    return acosf(clamp(v.z, -1.f, 1.f));
}

inline float SphericalPhi(const Vec3f &v) {
    float p = atan2f(v.y, v.x);
    return (p < 0.f) ? p + TWO_PIf : p;
}

// A[0] = phi(0:2pi) | A[1] = theta(0:pi)
template<class T>
Vec2<T> dirToAngle(const Vec3<T>& dirUnnormalized) {
	Vec2<T> angle;
	Vec3<T> dir = dirUnnormalized.GetNormalized();
    angle[0] = SphericalPhi(dir);
	angle[1] = SphericalTheta(dir);
	if(angle[0]<0) angle[0] = angle[0]+2*(T)PI;
	return angle;
}

template<class T>
Vec3<T> angleToDir(const Vec2<T>& angle) {
    Vec3<T> dir;
    dir[0] = sinf(angle[1])*cosf(angle[0]);
    dir[1] = sinf(angle[1])*sinf(angle[0]);
    dir[2] = cosf(angle[1]);
	return dir.GetNormalized();
}

#endif

