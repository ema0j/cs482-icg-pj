#ifndef _STREAMMETHODS_H_
#define _STREAMMETHODS_H_

#include <ostream>
#include <istream>

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "range3.h"
#include "matrix2.h"
#include "matrix3.h"
#include "matrix4.h"
#include "projection.h"
#include "frame.h"
#include "frustum.h"
#include "geom3.h"

template<class T> inline std::ostream& operator<<(std::ostream& o, const Vec2<T>& v) {
	return o << v[0] << " " << v[1];
}
template<class T> inline std::ostream& operator<<(std::ostream& o, const Vec3<T>& v) {
	return o << v[0] << " " << v[1] << " " << v[2];
}
template<class T> inline std::ostream& operator<<(std::ostream& o, const Vec4<T>& v) {
	return o << v[0] << " " << v[1] << " " << v[2] << " " << v[3];
}
template<class T> inline std::ostream& operator<<(std::ostream& o, const Tuple3<T>& v) {
	return o << v[0] << " " << v[1] << " " << v[2];
}

template<class T> inline std::ostream& operator<<(std::ostream& o, const Matrix2<T>& v) {
	return o << v[0] << " " << v[1];
}
template<class T> inline std::ostream& operator<<(std::ostream& o, const Matrix3<T>& v) {
	return o << v[0] << " " << v[1] << " " << v[2];
}
template<class T> inline std::ostream& operator<<(std::ostream& o, const Matrix4<T>& v) {
	return o << v[0] << " " << v[1] << " " << v[2] << " " << v[3];
}

template<class T> inline std::ostream& operator<<(std::ostream& o, const Range3<T>& v) {
    return o << v.GetMin() << " " << v.GetMax();
}

template<class T> inline std::ostream& operator<<(std::ostream& o, const Frame<T>& v) {
	return o << "[" << v.GetOrigin() << "," << v.GetX() << "," << v.GetY() << "," << v.GetZ() << "]";
}

template<class T> inline std::ostream& operator<<(std::ostream& o, const Projection<T>& v) {
	return o << "[" << v.IsPerspective() << "," << v.GetFOV() << "," << v.GetAspectRatio() << "," << v.GetNear() << "," << v.GetFar() << "]";
}

template<class T> inline std::ostream& operator<<(std::ostream& o, const Frustum<T>& v) {
    return o << "Frame " << v.GetFrame() << "    " << "Projection " << v.GetProjection();
}

template<class T> inline std::istream& operator>>(std::istream& o, Vec2<T>& v) {
	return o >> v[0] >> v[1];
}
template<class T> inline std::istream& operator>>(std::istream& o, Vec3<T>& v) {
	return o >> v[0] >> v[1] >> v[2];
}
template<class T> inline std::istream& operator>>(std::istream& o, Vec4<T>& v) {
	return o >> v[0] >> v[1] >> v[2] >> v[3];
}
template<class T> inline std::istream& operator>>(std::istream& o, Tuple3<T>& v) {
	return o >> v[0] >> v[1] >> v[2];
}

template<class T> inline std::istream& operator>>(std::istream& o, Matrix2<T>& v) {
	return o >> v[0] >> v[1];
}
template<class T> inline std::istream& operator>>(std::istream& o, Matrix3<T>& v) {
	return o >> v[0] >> v[1] >> v[2];
}
template<class T> inline std::istream& operator>>(std::istream& o, Matrix4<T>& v) {
	return o >> v[0] >> v[1] >> v[2] >> v[3];
}

#endif
