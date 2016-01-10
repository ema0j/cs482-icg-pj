#ifndef _MATRIX4_H_
#define _MATRIX4_H_

// INCLUDES ====================================================
#include "vec4.h"
#include "vec3.h"
#include "geom3.h"
#include "range3.h"
#include "geomrange3.h"
#include "consts.h"
#include "plane.h"
#include "matrix3.h"
#include <assert.h>
#include <stdio.h>
#include <misc/arrays.h>

#define _SLOW_NORMAL_TRANSFORM 0

// TYPE DECLARATION ============================================
template<class T>
class Matrix4 {
	// data ---------------------------------------------
protected:
	Vec4<T> _d[4]; // rows of the matrix

public:
	// Constructors -------------------------------------
	Matrix4();
	Matrix4(T* v);
	Matrix4(const Matrix4& v);
	template<class TT> Matrix4(const Matrix4<TT>& v);
	Matrix4(const Vec4<T>& r0, const Vec4<T>& r1, const Vec4<T>& r2, const Vec4<T>& r3);
	Matrix4(T m00, T m01, T m02, T m03,
			T m10, T m11, T m12, T m13,
			T m20, T m21, T m22, T m23,
			T m30, T m31, T m32, T m33);

	// Access ops ---------------------------------------
	const Vec4<T>& operator[](int i) const; // Access the row
	Vec4<T>& operator[](int i);
	const T& operator()(int i, int j) const; // Access the element
	T& operator()(int i, int j);
	operator T* ();
	operator const T* () const;
	Vec4<T> Row(int i) const;
	Vec4<T> Col(int i) const;

	// Comparison ops -----------------------------------
	bool operator == (const Matrix4& v) const;
	bool operator != (const Matrix4& v) const;
    bool IsIdentity() const;
    bool IsIdentity(T tolerance) const;
    bool IsRigid() const;
    bool IsRigid(T tolerance) const;
    bool IsAffine() const;
    bool IsAffine(T tolerance) const;

	// Binary ops ---------------------------------------
	Matrix4 operator+(const Matrix4& v) const;
	Matrix4 operator-(const Matrix4& v) const;
	Matrix4 operator*(const Matrix4& v) const;
	Matrix4 operator/(const Matrix4& v) const;
	Matrix4 operator+(T v) const;
	Matrix4 operator-(T v) const;
	Matrix4 operator*(T v) const;
	Matrix4 operator/(T v) const;

	// Assignment ops -----------------------------------
	Matrix4& operator=(const Matrix4& v);
	Matrix4& operator=(T v);
	Matrix4& operator+=(const Matrix4& v);
	Matrix4& operator-=(const Matrix4& v);
	Matrix4& operator*=(const Matrix4& v);
	Matrix4& operator/=(const Matrix4& v);
	Matrix4& operator+=(T v);
	Matrix4& operator-=(T v);
	Matrix4& operator*=(T v);
	Matrix4& operator/=(T v);

	// Matrix ops ---------------------------------------
	Matrix4 GetTranspose() const;
	T GetDeterminant() const;
	Matrix4 GetAdjoint() const;
	Matrix4 GetInverse() const;
	Matrix4 operator %(const Matrix4& v) const; // matrix multiply
	Vec4<T> operator %(const Vec4<T>& v) const; // transform

	// Transforms ---------------------------------------
	// Transform assume the multiplication are of the type
	// M % v.
	Vec4<T> TransformPoint(const Vec4<T>& v) const;
	template<class TT> Vec3<TT> TransformPoint(const Vec3<TT>& v) const;
	template<class TT> Vec3<TT> TransformNormalNoAdjoint(const Vec3<TT>& v) const;
	template<class TT> Vec3<TT> TransformVector(const Vec3<TT>& v) const;
	template<class TT> Vec3<TT> TransformNormalizedVector(const Vec3<TT>& v) const;
	template<class TT> Vec4<TT> TransformPoint(const Vec4<TT>& v) const;
	template<class TT> Range3<TT> TransformBBox(const Range3<TT>& v) const;
	template<class TT> Point3<TT> TransformPoint(const Point3<TT>& v) const;
	template<class TT> Normal3<TT> TransformNormalNoAdjoint(const Normal3<TT>& v) const;
	template<class TT> Vector3<TT> TransformVector(const Vector3<TT>& v) const;
	template<class TT> Direction3<TT> TransformNormalizedVector(const Direction3<TT>& v) const;
	template<class TT> PointRange3<TT> TransformBBox(const PointRange3<TT>& v) const;
#if _SLOW_NORMAL_TRANSFORM
	template<class TT> Vec3<TT> TransformNormal(const Vec3<TT>& v) const;
#endif

	template<class TT> void TransformPointArray(carray<Vec3<TT> >& v) const;
	template<class TT> void TransformVectorArray(carray<Vec3<TT> >& v) const;
	template<class TT> void TransformNormalizedVectorArray(carray<Vec3<TT> >& v) const;
	template<class TT> void TransformNormalArrayNoAdjoint(carray<Vec3<TT> >& v) const;
	template<class TT> void TransformPointArray(carray<Point3<TT> >& v) const;
	template<class TT> void TransformVectorArray(carray<Vector3<TT> >& v) const;
	template<class TT> void TransformNormalArrayNoAdjoint(carray<Normal3<TT> >& v) const;
	template<class TT> void TransformNormalizedVectorArray(carray<Direction3<TT> >& v) const;
#if _SLOW_NORMAL_TRANSFORM
	template<class TT> void TransformNormalArray(carray<Vec3<TT> >& v) const;
#endif

	template<class TT> void TransformPointArray(Vec3<TT>* v, uint64_t l) const;
	template<class TT> void TransformVectorArray(Vec3<TT>* v, uint64_t l) const;
	template<class TT> void TransformNormalizedVectorArray(Vec3<TT>* v, uint64_t l) const;
	template<class TT> void TransformNormalArrayNoAdjoint(Vec3<TT>* v, uint64_t l) const;
	template<class TT> void TransformPointArray(Point3<TT>* v, uint64_t l) const;
	template<class TT> void TransformVectorArray(Vector3<TT>* v, uint64_t l) const;
	template<class TT> void TransformNormalArrayNoAdjoint(Normal3<TT>* v, uint64_t l) const;
	template<class TT> void TransformNormalizedVectorArray(Direction3<TT>* v, uint64_t l) const;
#if _SLOW_NORMAL_TRANSFORM
	template<class TT> void TransformNormalArray(Vec3<TT>* v, int l) const;
#endif

	// Typical matrices ---------------------------------
	static Matrix4 Identity();
	static Matrix4 Zero();
	static Matrix4 Translation(const Vec3d& v);
	template<class TT> static Matrix4 Translation(const Vector3<TT>& v);
	static Matrix4 RotationX(T angle);
	static Matrix4 RotationY(T angle);
	static Matrix4 RotationZ(T angle);
	static Matrix4 Rotation(const Vec3<T>& v, T angle);
	template<class TT> static Matrix4 Rotation(const Direction3<TT>& v, TT angle);
	static Matrix4 Rotation(const Vec3<T>& v0, const Vec3<T>& v1); // rotation that brings v0 in v1
	static Matrix4 Rotation(T anglex, T angley, T anglez);
	static Matrix4 Scale(T v);
	static Matrix4 Scale(const Vec3<T>& v);
	static Matrix4 OrthographicGL(T l, T r,
								T b, T t, T n, T f);
	static Matrix4 OrthographicGL(T fovy, T aspectRatio,
								T n, T f);
	static Matrix4 OrthographicDX(T l, T r,
								T b, T t, T n, T f);
	static Matrix4 OrthographicDX(T fovy, T aspectRatio,
								T n, T f);
    // right handed eye space -> [-1,1]^2*[-1,1] oriented along -z
	static Matrix4 PerspectiveGL(T l, T r,
								T b, T t, T n, T f);
	static Matrix4 PerspectiveGL(T fovy, T aspectRatio,
								T n, T f); // fovy in degrees
    // left handed eye space -> [-1,1]^2*[0,1] oriented along z
	static Matrix4 PerspectiveDX(T l, T r,
								T b, T t, T n, T f);
	static Matrix4 PerspectiveDX(T fovy, T aspectRatio,
								T n, T f);
	static Matrix4 LookAt(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up); // along positive z
	template<class TT> static Matrix4 LookAt(const Point3<TT>& eye, const Point3<TT>& center, const Direction3<TT>& up, bool flipz); // along positive z
	static Matrix4 LookAtGL(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up); // along negative z
	static Matrix4 LookAtRenderMan(const Vec3<T>& eye, const Vec3<T>& forward, const Vec3<T>& up);

    // Plane extraction for frustum culling --------------
    static void ExtractPlanesGL(Plane<T> planes[6], const Matrix4& mvp); // this is the modelviewproj (plane order: l,r,t,b,n,f)
    static void ExtractPlanesDX(Plane<T> planes[6], const Matrix4& mvp); // this is the modelviewproj (plane order: l,r,t,b,n,f)

    static Matrix4 ComputeXform(const Vec3<T>& translation, const Vec3<T>& rotationAxis, T rotationAngle, T scale, const Vec3<T>& center);
    static Matrix4 ComputeXformInverse(const Vec3<T>& translation, const Vec3<T>& rotationAxis, T rotationAngle, T scale, const Vec3<T>& center);
};

// IMPLEMENTATION ==============================================
// Constructors -------------------------------------
template<class T> inline Matrix4<T>::Matrix4() {
    _d[0] = Vec4<T>::X();
    _d[1] = Vec4<T>::Y();
    _d[2] = Vec4<T>::Z();
    _d[3] = Vec4<T>::W();
}

template<class T> template<class TT> inline Matrix4<T>::Matrix4(const Matrix4<TT>& v) {
	_d[0] = Vec4<T>(v.Row(0));
	_d[1] = Vec4<T>(v.Row(1));
	_d[2] = Vec4<T>(v.Row(2));
	_d[3] = Vec4<T>(v.Row(3));
}

template<class T> inline Matrix4<T>::Matrix4(const Matrix4<T>& v) {
	_d[0] = v._d[0];
	_d[1] = v._d[1];
	_d[2] = v._d[2];
	_d[3] = v._d[3];
}

template<class T> inline Matrix4<T>::Matrix4(T* v) {
    _d[0] = Vec4<T>(v[0],v[1],v[2],v[3]);
    _d[1] = Vec4<T>(v[4],v[5],v[6],v[7]);
    _d[2] = Vec4<T>(v[8],v[9],v[10],v[11]);
    _d[3] = Vec4<T>(v[12],v[13],v[14],v[15]);
}

template<class T> inline Matrix4<T>::Matrix4(const Vec4<T>&  r0, const Vec4<T>&  r1, const Vec4<T>&  r2, const Vec4<T>&  r3) {
	this->_d[0] = r0;
	this->_d[1] = r1;
	this->_d[2] = r2;
	this->_d[3] = r3;
}

template<class T> inline Matrix4<T>::Matrix4(T m00, T m01, T m02, T m03,
						T m10, T m11, T m12, T m13,
						T m20, T m21, T m22, T m23,
						T m30, T m31, T m32, T m33) {
	this->_d[0] = Vec4<T>(m00,m01,m02,m03);
	this->_d[1] = Vec4<T>(m10,m11,m12,m13);
	this->_d[2] = Vec4<T>(m20,m21,m22,m23);
	this->_d[3] = Vec4<T>(m30,m31,m32,m33);
}

// Access ops ---------------------------------------
template<class T> inline const Vec4<T>& Matrix4<T>::operator[](int i) const {
	return _d[i];
}

template<class T> inline Vec4<T>& Matrix4<T>::operator[](int i) {
	return _d[i];
}

template<class T> inline const T& Matrix4<T>::operator()(int i, int j) const {
	return _d[i][j];
}

template<class T> inline T& Matrix4<T>::operator()(int i, int j) {
	return _d[i][j];
}

template<class T> inline Matrix4<T>::operator T*() {
	return _d[0].operator T *();
}

template<class T> inline Matrix4<T>::operator const T*() const {
	return _d[0].operator const T *();
}

template<class T> inline Vec4<T> Matrix4<T>::Row(int i) const {
	return _d[i];
}

template<class T> inline Vec4<T> Matrix4<T>::Col(int i) const {
	return Vec4<T>(_d[0][i], _d[1][i], _d[2][i], _d[3][i]);
}

// Comparison ops -----------------------------------
template<class T> inline bool Matrix4<T>::IsIdentity() const {
    for(int i = 0; i < 4; i ++) {
        for(int j = 0; j < 4; j ++) {
            if(i == j && _d[i][j] != 1) return false;
            if(i != j && _d[i][j] != 0) return false;
        }
    }
    return true;
}

template<class T> inline bool Matrix4<T>::IsIdentity(T tolerance) const {
    for(int i = 0; i < 4; i ++) {
        for(int j = 0; j < 4; j ++) {
            if(i == j && !eq(_d[i][j], T(1), tolerance) ) return false;
            if(i != j && !eq(_d[i][j], T(0), tolerance) ) return false;
        }
    }
    return true;
}

template<class T> inline bool Matrix4<T>::IsAffine() const {
    return _d[3][0] == 0 && _d[3][1] == 0 && _d[3][2] == 0 && _d[3][3] == 1;
}

template<class T> inline bool Matrix4<T>::IsAffine(T tolerance) const {
    return eq(_d[3][0], T(0), tolerance) && 
           eq(_d[3][1], T(0), tolerance) && 
           eq(_d[3][2], T(0), tolerance) && 
           eq(_d[3][3], T(1), tolerance);
}

template<class T> inline bool Matrix4<T>::IsRigid() const {
    if(!IsAffine()) return false;
    Vec3<T> u = Vec3<T>(_d[0][0], _d[1][0], _d[2][0]);
    Vec3<T> v = Vec3<T>(_d[0][1], _d[1][1], _d[2][1]);
    Vec3<T> w = Vec3<T>(_d[0][2], _d[1][2], _d[2][2]);
    return 
        u.GetLength() == 1 &&
        v.GetLength() == 1 &&
        w.GetLength() == 1 &&
        (u % v) == 0 &&
        (u % w) == 0 &&
        (v % w) == 0;
}

template<class T> inline bool Matrix4<T>::IsRigid(T tolerance) const {
    if(!IsAffine(tolerance)) return false;
    Vec3<T> u = Vec3<T>(_d[0][0], _d[1][0], _d[2][0]);
    Vec3<T> v = Vec3<T>(_d[0][1], _d[1][1], _d[2][1]);
    Vec3<T> w = Vec3<T>(_d[0][2], _d[1][2], _d[2][2]);
    return 
        eq(u.GetLength(), T(1), tolerance) &&
        eq(v.GetLength(), T(1), tolerance) &&
        eq(w.GetLength(), T(1), tolerance) &&
        eq((u % v), T(0), tolerance) &&
        eq((u % w), T(0), tolerance) &&
        eq((v % w), T(0), tolerance);
}

// Matrix ops ---------------------------------------
template<class T> inline Matrix4<T> Matrix4<T>::GetTranspose() const {
	return Matrix4<T>(Col(0), Col(1), Col(2), Col(3));
}

template<class T> inline T Matrix4<T>::GetDeterminant() const {
    return _d[0][3]*_d[1][2]*_d[2][1]*_d[3][0] -
           _d[0][2]*_d[1][3]*_d[2][1]*_d[3][0] -
           _d[0][3]*_d[1][1]*_d[2][2]*_d[3][0] +
           _d[0][1]*_d[1][3]*_d[2][2]*_d[3][0] +
           _d[0][2]*_d[1][1]*_d[2][3]*_d[3][0] -
           _d[0][1]*_d[1][2]*_d[2][3]*_d[3][0] -
           _d[0][3]*_d[1][2]*_d[2][0]*_d[3][1] +
           _d[0][2]*_d[1][3]*_d[2][0]*_d[3][1] +
           _d[0][3]*_d[1][0]*_d[2][2]*_d[3][1] -
           _d[0][0]*_d[1][3]*_d[2][2]*_d[3][1] -
           _d[0][2]*_d[1][0]*_d[2][3]*_d[3][1] +
           _d[0][0]*_d[1][2]*_d[2][3]*_d[3][1] +
           _d[0][3]*_d[1][1]*_d[2][0]*_d[3][2] -
           _d[0][1]*_d[1][3]*_d[2][0]*_d[3][2] -
           _d[0][3]*_d[1][0]*_d[2][1]*_d[3][2] +
           _d[0][0]*_d[1][3]*_d[2][1]*_d[3][2] +
           _d[0][1]*_d[1][0]*_d[2][3]*_d[3][2] -
           _d[0][0]*_d[1][1]*_d[2][3]*_d[3][2] -
           _d[0][2]*_d[1][1]*_d[2][0]*_d[3][3] +
           _d[0][1]*_d[1][2]*_d[2][0]*_d[3][3] +
           _d[0][2]*_d[1][0]*_d[2][1]*_d[3][3] -
           _d[0][0]*_d[1][2]*_d[2][1]*_d[3][3] -
           _d[0][1]*_d[1][0]*_d[2][2]*_d[3][3] +
           _d[0][0]*_d[1][1]*_d[2][2]*_d[3][3];
}

template<class T> inline Matrix4<T> Matrix4<T>::GetAdjoint() const {
    return Matrix4<T>(-_d[1][3]*_d[2][2]*_d[3][1] + _d[1][2]*_d[2][3]*_d[3][1] + _d[1][3]*_d[2][1]*_d[3][2] - _d[1][1]*_d[2][3]*_d[3][2] - _d[1][2]*_d[2][1]*_d[3][3] + _d[1][1]*_d[2][2]*_d[3][3],
                     _d[0][3]*_d[2][2]*_d[3][1] - _d[0][2]*_d[2][3]*_d[3][1] - _d[0][3]*_d[2][1]*_d[3][2] + _d[0][1]*_d[2][3]*_d[3][2] + _d[0][2]*_d[2][1]*_d[3][3] - _d[0][1]*_d[2][2]*_d[3][3],
                    -_d[0][3]*_d[1][2]*_d[3][1] + _d[0][2]*_d[1][3]*_d[3][1] + _d[0][3]*_d[1][1]*_d[3][2] - _d[0][1]*_d[1][3]*_d[3][2] - _d[0][2]*_d[1][1]*_d[3][3] + _d[0][1]*_d[1][2]*_d[3][3],
                     _d[0][3]*_d[1][2]*_d[2][1] - _d[0][2]*_d[1][3]*_d[2][1] - _d[0][3]*_d[1][1]*_d[2][2] + _d[0][1]*_d[1][3]*_d[2][2] + _d[0][2]*_d[1][1]*_d[2][3] - _d[0][1]*_d[1][2]*_d[2][3],
                     _d[1][3]*_d[2][2]*_d[3][0] - _d[1][2]*_d[2][3]*_d[3][0] - _d[1][3]*_d[2][0]*_d[3][2] + _d[1][0]*_d[2][3]*_d[3][2] + _d[1][2]*_d[2][0]*_d[3][3] - _d[1][0]*_d[2][2]*_d[3][3],
                    -_d[0][3]*_d[2][2]*_d[3][0] + _d[0][2]*_d[2][3]*_d[3][0] + _d[0][3]*_d[2][0]*_d[3][2] - _d[0][0]*_d[2][3]*_d[3][2] - _d[0][2]*_d[2][0]*_d[3][3] + _d[0][0]*_d[2][2]*_d[3][3],
                     _d[0][3]*_d[1][2]*_d[3][0] - _d[0][2]*_d[1][3]*_d[3][0] - _d[0][3]*_d[1][0]*_d[3][2] + _d[0][0]*_d[1][3]*_d[3][2] + _d[0][2]*_d[1][0]*_d[3][3] - _d[0][0]*_d[1][2]*_d[3][3],
                    -_d[0][3]*_d[1][2]*_d[2][0] + _d[0][2]*_d[1][3]*_d[2][0] + _d[0][3]*_d[1][0]*_d[2][2] - _d[0][0]*_d[1][3]*_d[2][2] - _d[0][2]*_d[1][0]*_d[2][3] + _d[0][0]*_d[1][2]*_d[2][3],
                    -_d[1][3]*_d[2][1]*_d[3][0] + _d[1][1]*_d[2][3]*_d[3][0] + _d[1][3]*_d[2][0]*_d[3][1] - _d[1][0]*_d[2][3]*_d[3][1] - _d[1][1]*_d[2][0]*_d[3][3] + _d[1][0]*_d[2][1]*_d[3][3],
                     _d[0][3]*_d[2][1]*_d[3][0] - _d[0][1]*_d[2][3]*_d[3][0] - _d[0][3]*_d[2][0]*_d[3][1] + _d[0][0]*_d[2][3]*_d[3][1] + _d[0][1]*_d[2][0]*_d[3][3] - _d[0][0]*_d[2][1]*_d[3][3],
                    -_d[0][3]*_d[1][1]*_d[3][0] + _d[0][1]*_d[1][3]*_d[3][0] + _d[0][3]*_d[1][0]*_d[3][1] - _d[0][0]*_d[1][3]*_d[3][1] - _d[0][1]*_d[1][0]*_d[3][3] + _d[0][0]*_d[1][1]*_d[3][3],
                     _d[0][3]*_d[1][1]*_d[2][0] - _d[0][1]*_d[1][3]*_d[2][0] - _d[0][3]*_d[1][0]*_d[2][1] + _d[0][0]*_d[1][3]*_d[2][1] + _d[0][1]*_d[1][0]*_d[2][3] - _d[0][0]*_d[1][1]*_d[2][3],
                     _d[1][2]*_d[2][1]*_d[3][0] - _d[1][1]*_d[2][2]*_d[3][0] - _d[1][2]*_d[2][0]*_d[3][1] + _d[1][0]*_d[2][2]*_d[3][1] + _d[1][1]*_d[2][0]*_d[3][2] - _d[1][0]*_d[2][1]*_d[3][2],
                    -_d[0][2]*_d[2][1]*_d[3][0] + _d[0][1]*_d[2][2]*_d[3][0] + _d[0][2]*_d[2][0]*_d[3][1] - _d[0][0]*_d[2][2]*_d[3][1] - _d[0][1]*_d[2][0]*_d[3][2] + _d[0][0]*_d[2][1]*_d[3][2],
                     _d[0][2]*_d[1][1]*_d[3][0] - _d[0][1]*_d[1][2]*_d[3][0] - _d[0][2]*_d[1][0]*_d[3][1] + _d[0][0]*_d[1][2]*_d[3][1] + _d[0][1]*_d[1][0]*_d[3][2] - _d[0][0]*_d[1][1]*_d[3][2],
                    -_d[0][2]*_d[1][1]*_d[2][0] + _d[0][1]*_d[1][2]*_d[2][0] + _d[0][2]*_d[1][0]*_d[2][1] - _d[0][0]*_d[1][2]*_d[2][1] - _d[0][1]*_d[1][0]*_d[2][2] + _d[0][0]*_d[1][1]*_d[2][2]);
}

template<class T> inline Matrix4<T> Matrix4<T>::GetInverse() const {
    // return GetAdjoint() * (1 / GetDeterminant());
    Matrix4<T> inv = GetAdjoint() * (1 / GetDeterminant());
    Matrix4<T> id = inv % *this;
    if(!id.IsIdentity(T(0.0001))) { printf("Matrix4<T>: bogus inverse\n"); assert(0); }
    return inv;
}

// PBRT inverse ...
/*
 Reference<Matrix4x4> Matrix4x4::Inverse() const {
 int indxc[4], indxr[4];
 int ipiv[4] = { 0, 0, 0, 0 };
 float minv[4][4];
 memcpy(minv, m, 4*4*sizeof(float));
 for (int i = 0; i < 4; i++) {
 int irow = -1, icol = -1;
 float big = 0.;
 // Choose pivot
 for (int j = 0; j < 4; j++) {
 if (ipiv[j] != 1) {
 for (int k = 0; k < 4; k++) {
 if (ipiv[k] == 0) {
 if (fabsf(minv[j][k]) >= big) {
 big = float(fabsf(minv[j][k]));
 irow = j;
 icol = k;
 }
 }
 else if (ipiv[k] > 1)
 Error("Singular matrix in MatrixInvert");
 }
 }
 }
 ++ipiv[icol];
 // Swap rows _irow_ and _icol_ for pivot
 if (irow != icol) {
 for (int k = 0; k < 4; ++k)
 swap(minv[irow][k], minv[icol][k]);
 }
 indxr[i] = irow;
 indxc[i] = icol;
 if (minv[icol][icol] == 0.)
 Error("Singular matrix in MatrixInvert");
 // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
 float pivinv = 1.f / minv[icol][icol];
 minv[icol][icol] = 1.f;
 for (int j = 0; j < 4; j++)
 minv[icol][j] *= pivinv;
 // Subtract this row from others to zero out their columns
 for (int j = 0; j < 4; j++) {
 if (j != icol) {
 float save = minv[j][icol];
 minv[j][icol] = 0;
 for (int k = 0; k < 4; k++)
 minv[j][k] -= minv[icol][k]*save;
 }
 }
 }
 // Swap columns to reflect permutation
 for (int j = 3; j >= 0; j--) {
 if (indxr[j] != indxc[j]) {
 for (int k = 0; k < 4; k++)
 swap(minv[k][indxr[j]], minv[k][indxc[j]]);
		}
	}
	return new Matrix4x4(minv);
}
*/

template<class T> inline Matrix4<T> Matrix4<T>::operator %(const Matrix4<T>& v) const {
	Matrix4<T> t = v.GetTranspose();
	return Matrix4<T>(_d[0] % t._d[0], _d[0] % t._d[1], _d[0] % t._d[2], _d[0] % t._d[3],
				   _d[1] % t._d[0], _d[1] % t._d[1], _d[1] % t._d[2], _d[1] % t._d[3],
				   _d[2] % t._d[0], _d[2] % t._d[1], _d[2] % t._d[2], _d[2] % t._d[3],
				   _d[3] % t._d[0], _d[3] % t._d[1], _d[3] % t._d[2], _d[3] % t._d[3]);
}

template<class T> inline Vec4<T> Matrix4<T>::operator %(const Vec4<T>& v) const {
	return Vec4<T>(_d[0] % v, _d[1] % v, _d[2] % v, _d[3] % v);
}

// Transform ops ---------------------------------------
template<class T> inline Vec4<T> Matrix4<T>::TransformPoint(const Vec4<T>& v) const {
	return Vec4<T>(_d[0] % v, _d[1] % v, _d[2] % v, _d[3] % v);
}

template<class T> template<class TT> inline Vec3<TT> Matrix4<T>::TransformPoint(const Vec3<TT>& v) const {
	Vec4<T> vv(v[0],v[1],v[2],1);
	return Vec4<TT>(TT(_d[0] % vv), TT(_d[1] % vv), TT(_d[2] % vv), TT(_d[3] % vv)).Project();
}

template<class T> template<class TT> inline Point3<TT> Matrix4<T>::TransformPoint(const Point3<TT>& v) const {
	Vec4<T> vv(v[0],v[1],v[2],1);
	Vec3<TT> vvv = Vec4<TT>(TT(_d[0] % vv), TT(_d[1] % vv), TT(_d[2] % vv), TT(_d[3] % vv)).Project();
    return Point3<TT>(vvv.x,vvv.y,vvv.z);
}

template<class T> template<class TT> inline Vec3<TT> Matrix4<T>::TransformVector(const Vec3<TT>& v) const {
	Vec4<T> vv(v[0],v[1],v[2],0);
	return Vec3<TT>(TT(_d[0] % vv), TT(_d[1] % vv), TT(_d[2] % vv));
}

template<class T> template<class TT> inline Vector3<TT> Matrix4<T>::TransformVector(const Vector3<TT>& v) const {
	Vec4<T> vv(v[0],v[1],v[2],0);
	Vec3<TT> vvv = Vec3<TT>(TT(_d[0] % vv), TT(_d[1] % vv), TT(_d[2] % vv));
    return Vector3<TT>(vvv.x,vvv.y,vvv.z);
}

template<class T> template<class TT> inline Vec3<TT> Matrix4<T>::TransformNormalizedVector(const Vec3<TT>& v) const {
    return TransformVector(v).GetNormalized();
}

template<class T> template<class TT> inline Direction3<TT> Matrix4<T>::TransformNormalizedVector(const Direction3<TT>& v) const {
    return TransformVector(v).GetNormalized();
}

#if _SLOW_NORMAL_TRANSFORM
template<class T> template<class TT> inline Vec3<TT> Matrix4<T>::TransformNormal(const Vec3<TT>& v) const {
    Matrix4<T> it = GetInverse().GetTranspose();
	return it.TransformVector(v).GetNormalized();
}
#endif

template<class T> template<class TT> inline Vec3<TT> Matrix4<T>::TransformNormalNoAdjoint(const Vec3<TT>& v) const {
	return TransformVector(v).GetNormalized();
}

template<class T> template<class TT> inline Normal3<TT> Matrix4<T>::TransformNormalNoAdjoint(const Normal3<TT>& v) const {
	return Normal3<TT>(TransformVector(v).GetNormalized());
}

template<class T> template<class TT> inline Vec4<TT> Matrix4<T>::TransformPoint(const Vec4<TT>& v) const {
	Vec4<T> vv(v);
	return Vec4<TT>(TT(_d[0] % vv), TT(_d[1] % vv), TT(_d[2] % vv), TT(_d[3] % vv));
}

template<class T> template<class TT> inline Range3<TT> Matrix4<T>::TransformBBox(const Range3<TT>& b) const {
	Vec3<TT> corners[8];
    b.GetCorners(corners);
    Range3<TT> ret = Range3<TT>::Empty();
    for(int i = 0; i < 8; i ++) ret.Grow(TransformPoint(corners[i]));
    return ret;
}

template<class T> template<class TT> inline PointRange3<TT> Matrix4<T>::TransformBBox(const PointRange3<TT>& b) const {
	Point3<TT> corners[8];
    b.GetCorners(corners);
    PointRange3<TT> ret = PointRange3<TT>();
    for(int i = 0; i < 8; i ++) ret.Grow(TransformPoint(corners[i]));
    return ret;
}

template<class T> template<class TT> inline void Matrix4<T>::TransformPointArray(carray<Vec3<TT> >& v) const {
    TransformPointArray(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformPointArray(carray<Point3<TT> >& v) const {
    TransformPointArray(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformVectorArray(carray<Vec3<TT> >& v) const {
    TransformVectorArray(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformVectorArray(carray<Vector3<TT> >& v) const {
    TransformVectorArray(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalizedVectorArray(carray<Vec3<TT> >& v) const {
    TransformNormalizedVectorArray(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalizedVectorArray(carray<Direction3<TT> >& v) const {
    TransformNormalizedVectorArray(v.data(), v.size());
}

#if _SLOW_NORMAL_TRANSFORM
template<class T> template<class TT> inline void Matrix4<T>::TransformNormalArray(carray<Vec3<TT> >& v) const {
    TransformNormalArray(v.data(), v.size());
}
#endif

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalArrayNoAdjoint(carray<Vec3<TT> >& v) const {
    TransformNormalArrayNoAdjoint(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalArrayNoAdjoint(carray<Normal3<TT> >& v) const {
    TransformNormalArrayNoAdjoint(v.data(), v.size());
}

template<class T> template<class TT> inline void Matrix4<T>::TransformPointArray(Vec3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformPoint(v[i]);
}

template<class T> template<class TT> inline void Matrix4<T>::TransformPointArray(Point3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformPoint(v[i]);
}

template<class T> template<class TT> inline void Matrix4<T>::TransformVectorArray(Vec3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformVector(v[i]);
}

template<class T> template<class TT> inline void Matrix4<T>::TransformVectorArray(Vector3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformVector(v[i]);
}

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalizedVectorArray(Vec3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformNormalizedVector(v[i]);
}

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalizedVectorArray(Direction3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformNormalizedVector(v[i]);
}

#if _SLOW_NORMAL_TRANSFORM
template<class T> template<class TT> inline void Matrix4<T>::TransformNormalArray(Vec3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformNormal(v[i]);
}
#endif

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalArrayNoAdjoint(Vec3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformNormalNoAdjoint(v[i]);
}

template<class T> template<class TT> inline void Matrix4<T>::TransformNormalArrayNoAdjoint(Normal3<TT>* v, uint64_t l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformNormalNoAdjoint(v[i]);
}

// Typical matrices ---------------------------------
template<class T> inline Matrix4<T> Matrix4<T>::Identity() {
	return Matrix4<T>(Vec4<T>(1,0,0,0), Vec4<T>(0,1,0,0), Vec4<T>(0,0,1,0), Vec4<T>(0,0,0,1));
}

template<class T> inline Matrix4<T> Matrix4<T>::Zero() {
	return Matrix4<T>(Vec4<T>(0,0,0,0), Vec4<T>(0,0,0,0), Vec4<T>(0,0,0,0), Vec4<T>(0,0,0,0));
}

template<class T> inline Matrix4<T> Matrix4<T>::Translation(const Vec3d& v) {
	return Matrix4<T>(1,0,0,v[0], 0,1,0,v[1], 0,0,1,v[2], 0,0,0,1);
}

template<class T> template<class TT> inline Matrix4<T> Matrix4<T>::Translation(const Vector3<TT>& v) {
	return Matrix4<T>(1,0,0,v[0], 0,1,0,v[1], 0,0,1,v[2], 0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::RotationX(T angle) {
	T c = cos(angle); T s = sin(angle);
	return Matrix4<T>(1,0,0,0, 0,c,-s,0, 0,s,c,0, 0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::RotationY(T angle) {
	T c = cos(angle); T s = sin(angle);
	return Matrix4<T>(c,0,s,0, 0,1,0,0, -s,0,c,0, 0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::RotationZ(T angle) {
	T c = cos(angle); T s = sin(angle);
	return Matrix4<T>(c,-s,0,0, s,c,0,0, 0,0,1,0, 0,0,0,1);
}

template<class T> template<class TT> inline Matrix4<T> Matrix4<T>::Rotation(const Direction3<TT>& v, TT angle) {
	// From Real_Time Rendering by Moller and Haines - Chapter 3.2
	T c = cos(angle); T s = sin(angle);
	return Matrix4<T>(c+(1-c)*v[0]*v[0],(1-c)*v[0]*v[1]-v[2]*s,(1-c)*v[0]*v[2]+v[1]*s,0,
		           (1-c)*v[0]*v[1]+v[2]*s,c+(1-c)*v[1]*v[1],(1-c)*v[1]*v[2]-v[0]*s,0,
				   (1-c)*v[0]*v[2]-v[1]*s,(1-c)*v[1]*v[2]+v[0]*s,c+(1-c)*v[2]*v[2],0,
				   0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::Rotation(const Vec3<T>& v, T angle) {
	// From Real_Time Rendering by Moller and Haines - Chapter 3.2
	Vec3<T> vv = v.GetNormalized();
	T c = cos(angle); T s = sin(angle);
	return Matrix4<T>(c+(1-c)*vv[0]*vv[0],(1-c)*vv[0]*vv[1]-vv[2]*s,(1-c)*vv[0]*vv[2]+vv[1]*s,0,
		           (1-c)*vv[0]*vv[1]+vv[2]*s,c+(1-c)*vv[1]*vv[1],(1-c)*vv[1]*vv[2]-vv[0]*s,0,
				   (1-c)*vv[0]*vv[2]-vv[1]*s,(1-c)*vv[1]*vv[2]+vv[0]*s,c+(1-c)*vv[2]*vv[2],0,
				   0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::Rotation(T anglex, T angley, T anglez) {
	return RotationZ(anglez)%RotationX(anglex)%RotationY(angley);
}

template<class T> inline Matrix4<T> Matrix4<T>::Rotation(const Vec3<T>& a, const Vec3<T>& b) {
    Vec3<T> an = a.GetNormalized(); 
    Vec3<T> bn = b.GetNormalized();
    Vec3<T> v = an ^ bn;
    T e = an % bn;
    T h = 1 / (1+e);

    return Matrix4<T>(e+h*v.x*v.x, h*v.x*v.y-v.z, h*v.x*v.z+v.y, 0,
                      h*v.x*v.y+v.z, e+h*v.y*v.y, h*v.y*v.z-v.x, 0,
                      h*v.x*v.z-v.y, h*v.y*v.z+v.x, e+h*v.z*v.z, 0,
                      0, 0, 0, 1);
}

template<class T> inline Matrix4<T> Matrix4<T>::Scale(T v) {
	return Matrix4<T>(Vec4<T>(v,0,0,0), Vec4<T>(0,v,0,0), Vec4<T>(0,0,v,0), Vec4<T>(0,0,0,1));
}

template<class T> inline Matrix4<T> Matrix4<T>::Scale(const Vec3<T>& v) {
	return Matrix4<T>(Vec4<T>(v[0],0,0,0), Vec4<T>(0,v[1],0,0), Vec4<T>(0,0,v[2],0), Vec4<T>(0,0,0,1));
}

template<class T> inline Matrix4<T> Matrix4<T>::OrthographicGL(T l, T r, T b, T t, T n, T f) {
	return Matrix4<T>(2/(r-l),0,0,-(r+l)/(r-l),
				   0,2/(t-b),0,-(t+b)/(t-b),
				   0,0,-2/(f-n),-(f+n)/(f-n),
				   0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::OrthographicGL(T fovy, T aspectRatio, T n, T f) {
	return Matrix4::OrthographicGL(-aspectRatio*fovy, aspectRatio*fovy, -fovy, fovy, n, f);
}

template<class T> inline Matrix4<T> Matrix4<T>::OrthographicDX(T l, T r, T b, T t, T n, T f) {
	return Matrix4<T>(2/(r-l),0,0,-(r+l)/(r-l),
				   0,2/(t-b),0,-(t+b)/(t-b),
				   0,0,-1/(f-n),-n/(f-n),
				   0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::OrthographicDX(T fovy, T aspectRatio, T n, T f) {
	return Matrix4::OrthographicDX(-aspectRatio*fovy, aspectRatio*fovy, -fovy, fovy, n, f);
}

template<class T> inline Matrix4<T> Matrix4<T>::PerspectiveGL(T l, T r, T b, T t, T n, T f) {
	return Matrix4<T>(2*n/(r-l),0,(r+l)/(r-l),0,
				   0,2*n/(t-b),(t+b)/(t-b),0,
				   0,0,-(f+n)/(f-n),-2*f*n/(f-n),
				   0,0,-1,0);
}

template<class T> inline Matrix4<T> Matrix4<T>::PerspectiveGL(T fovy, T aspectRatio, T n, T f) {
	T t = n * tan((fovy*PI/180)/2);
	return Matrix4<T>::PerspectiveGL(-t/aspectRatio,t/aspectRatio,-t,t,n,f);
}

template<class T> inline Matrix4<T> Matrix4<T>::PerspectiveDX(T l, T r, T b, T t, T n, T f) {
	return Matrix4<T>(2*n/(r-l),0,(r+l)/(r-l),0,
				   0,2*n/(t-b),(t+b)/(t-b),0,
				   0,0,-f/(f-n),-f*n/(f-n),
				   0,0,-1,0);
}

template<class T> inline Matrix4<T> Matrix4<T>::PerspectiveDX(T fovy, T aspectRatio, T n, T f) {
	T t = n * tan((fovy*PI/180)/2);
	return Matrix4<T>::PerspectiveDX(-t/aspectRatio,t/aspectRatio,-t,t,n,f);
}

template<class T> template<class TT> inline Matrix4<T> Matrix4<T>::LookAt(const Point3<TT>& eye, const Point3<TT>& center, const Direction3<TT>& up, bool flipz) {
	// From notes by Ramamoorthi
	Direction3<TT> d = (center - eye).GetNormalized();
	Direction3<TT> s = (d ^ up).GetNormalized();
	Direction3<TT> u = (s ^ d).GetNormalized();
    Vector3<T> e = eye - Point3<T>();
    if(flipz) {
	    return Matrix4<T>(s[0],s[1],s[2],-(s%e),
				       u[0],u[1],u[2],-(u%e),
				       -d[0],-d[1],-d[2],(d%e),
				       0,0,0,1);
    } else {
	    return Matrix4<T>(s[0],s[1],s[2],-(s%e),
				       u[0],u[1],u[2],-(u%e),
				       d[0],d[1],d[2],-(d%e),
				       0,0,0,1);
    }
}

template<class T> inline Matrix4<T> Matrix4<T>::LookAt(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up) {
	// From notes by Ramamoorthi
	Vec3<T> upn = up.GetNormalized();
	Vec3<T> d = center - eye; d.Normalize();
	Vec3<T> s = d ^ upn; s.Normalize();
	Vec3<T> u = s ^ d; u.Normalize();
	return Matrix4<T>(s[0],s[1],s[2],-(s%eye),
				   u[0],u[1],u[2],-(u%eye),
				   d[0],d[1],d[2],-(d%eye),
				   0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::LookAtGL(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up) {
	// From notes by Ramamoorthi
	Vec3<T> upn = up.GetNormalized();
	Vec3<T> d = center - eye; d.Normalize();
	Vec3<T> s = d ^ upn; s.Normalize();
	Vec3<T> u = s ^ d; u.Normalize();
	return Matrix4<T>(s[0],s[1],s[2],-(s%eye),
				   u[0],u[1],u[2],-(u%eye),
				   -d[0],-d[1],-d[2],(d%eye),
				   0,0,0,1);
}

template<class T> inline Matrix4<T> Matrix4<T>::LookAtRenderMan(const Vec3<T>& eye, const Vec3<T>& forward, const Vec3<T>& up) {
	//Math from PBRT
	//remember to flip z from far camera as input (far cameras flip z internally)
	Vec3<T> z = forward.GetNormalized();
	Vec3<T> x = z^up.GetNormalized();
	Vec3<T> y = x^z;
	Matrix4<T> ret = Matrix4<T>(x[0],y[0],z[0],eye[0],
								x[1],y[1],z[1],eye[1],
								x[2],y[2],z[2],eye[2],
								0,0,0,1);
	return ret.GetInverse();
}

// from "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"
// by Gil Gribb, Klaus Hartmann
template<class T> inline void Matrix4<T>::ExtractPlanesGL(Plane<T> planes[6], const Matrix4<T>& mvp) {
    // Left clipping plane
    planes[0] = Plane<T>(
        mvp._d[3][0] + mvp._d[0][0],
        mvp._d[3][1] + mvp._d[0][1],
        mvp._d[3][2] + mvp._d[0][2],
        mvp._d[3][3] + mvp._d[0][3]);
    // Right clipping plane
    planes[1] = Plane<T>(
        mvp._d[3][0] - mvp._d[0][0],
        mvp._d[3][1] - mvp._d[0][1],
        mvp._d[3][2] - mvp._d[0][2],
        mvp._d[3][3] - mvp._d[0][3]);
    // Top clipping plane
    planes[2] = Plane<T>(
        mvp._d[3][0] - mvp._d[1][0],
        mvp._d[3][1] - mvp._d[1][1],
        mvp._d[3][2] - mvp._d[1][2],
        mvp._d[3][3] - mvp._d[1][3]);
    // Bottom clipping plane
    planes[3] = Plane<T>(
        mvp._d[3][0] + mvp._d[1][0],
        mvp._d[3][1] + mvp._d[1][1],
        mvp._d[3][2] + mvp._d[1][2],
        mvp._d[3][3] + mvp._d[1][3]);
    // Near clipping plane
    planes[4] = Plane<T>(
        mvp._d[3][0] + mvp._d[2][0],
        mvp._d[3][1] + mvp._d[2][1],
        mvp._d[3][2] + mvp._d[2][2],
        mvp._d[3][3] + mvp._d[2][3]);
    // Far clipping plane
    planes[5] = Plane<T>(
        mvp._d[3][0] - mvp._d[2][0],
        mvp._d[3][1] - mvp._d[2][1],
        mvp._d[3][2] - mvp._d[2][2],
        mvp._d[3][3] - mvp._d[2][3]);
}

// from "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"
// by Gil Gribb, Klaus Hartmann
template<class T> inline void Matrix4<T>::ExtractPlanesDX(Plane<T> planes[6], const Matrix4<T>& mvp) {
    // Left clipping plane
    planes[0] = Plane<T>(
        mvp._d[3][0] + mvp._d[0][0],
        mvp._d[3][1] + mvp._d[0][1],
        mvp._d[3][2] + mvp._d[0][2],
        mvp._d[3][3] + mvp._d[0][3]);
    // Right clipping plane
    planes[1] = Plane<T>(
        mvp._d[3][0] - mvp._d[0][0],
        mvp._d[3][1] - mvp._d[0][1],
        mvp._d[3][2] - mvp._d[0][2],
        mvp._d[3][3] - mvp._d[0][3]);
    // Top clipping plane
    planes[2] = Plane<T>(
        mvp._d[3][0] - mvp._d[1][0],
        mvp._d[3][1] - mvp._d[1][1],
        mvp._d[3][2] - mvp._d[1][2],
        mvp._d[3][3] - mvp._d[1][3]);
    // Bottom clipping plane
    planes[3] = Plane<T>(
        mvp._d[3][0] + mvp._d[1][0],
        mvp._d[3][1] + mvp._d[1][1],
        mvp._d[3][2] + mvp._d[1][2],
        mvp._d[3][3] + mvp._d[1][3]);
    // Near clipping plane
    planes[4] = Plane<T>(
        mvp._d[2][0],
        mvp._d[2][1],
        mvp._d[2][2],
        mvp._d[2][3]);
    // Far clipping plane
    planes[5] = Plane<T>(
        mvp._d[3][0] - mvp._d[2][0],
        mvp._d[3][1] - mvp._d[2][1],
        mvp._d[3][2] - mvp._d[2][2],
        mvp._d[3][3] - mvp._d[2][3]);
}

template<class T> inline Matrix4<T> Matrix4<T>::ComputeXform(const Vec3<T>& translation,
                                       const Vec3<T>& rotationAxis, T rotationAngle,
                                       T scale, const Vec3<T>& center) {
    return
        Matrix4<T>::Translation(translation) %
        Matrix4<T>::Translation(center) %
        Matrix4<T>::Rotation(rotationAxis, rotationAngle) %
        Matrix4<T>::Scale(scale) %
        Matrix4<T>::Translation(-center);
}

template<class T> inline Matrix4<T> Matrix4<T>::ComputeXformInverse(const Vec3<T>& translation,
											  const Vec3<T>& rotationAxis, T rotationAngle,
										      T scale,
                                              const Vec3<T>& center) {
    return
        Matrix4<T>::Translation(center) %
        Matrix4<T>::Scale(1/scale) %
        Matrix4<T>::Rotation(rotationAxis, -rotationAngle) %
        Matrix4<T>::Translation(-center) %
        Matrix4<T>::Translation(-translation);
}

// Comparison ops -----------------------------------
template<class T> inline bool Matrix4<T>::operator  == (const Matrix4<T>& v) const { 	return 		_d[0]  == v._d[0] && 		_d[1]  == v._d[1] && 		_d[2]  == v._d[2] && 		_d[3]  == v._d[3]; };
template<class T> inline bool Matrix4<T>::operator  != (const Matrix4<T>& v) const { 	return 		_d[0]  != v._d[0] || 		_d[1]  != v._d[1] || 		_d[2]  != v._d[2] || 		_d[3]  != v._d[3]; };

// Binary ops ---------------------------------------
template<class T> inline Matrix4<T> Matrix4<T>::operator  + (const Matrix4<T>& v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  + v._d[0];		ret._d[1] = _d[1]  + v._d[1];		ret._d[2] = _d[2]  + v._d[2];		ret._d[3] = _d[3]  + v._d[3];		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  - (const Matrix4<T>& v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  - v._d[0];		ret._d[1] = _d[1]  - v._d[1];		ret._d[2] = _d[2]  - v._d[2];		ret._d[3] = _d[3]  - v._d[3];		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  * (const Matrix4<T>& v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  * v._d[0];		ret._d[1] = _d[1]  * v._d[1];		ret._d[2] = _d[2]  * v._d[2];		ret._d[3] = _d[3]  * v._d[3];		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  / (const Matrix4<T>& v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  / v._d[0];		ret._d[1] = _d[1]  / v._d[1];		ret._d[2] = _d[2]  / v._d[2];		ret._d[3] = _d[3]  / v._d[3];		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  + ( T v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  + v;		ret._d[1] = _d[1]  + v;		ret._d[2] = _d[2]  + v;		ret._d[3] = _d[3]  + v;		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  - ( T v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  - v;		ret._d[1] = _d[1]  - v;		ret._d[2] = _d[2]  - v;		ret._d[3] = _d[3]  - v;		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  * ( T v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  * v;		ret._d[1] = _d[1]  * v;		ret._d[2] = _d[2]  * v;		ret._d[3] = _d[3]  * v;		return ret;			};
template<class T> inline Matrix4<T> Matrix4<T>::operator  / ( T v) const { 	Matrix4<T> ret; 	ret._d[0] = _d[0]  / v;		ret._d[1] = _d[1]  / v;		ret._d[2] = _d[2]  / v;		ret._d[3] = _d[3]  / v;		return ret;			};

// Assignment ops -----------------------------------
template<class T> inline Matrix4<T> & Matrix4<T>::operator  = (const Matrix4<T>& v) { 	_d[0]  = v._d[0];		_d[1]  = v._d[1];		_d[2]  = v._d[2];		_d[3]  = v._d[3];		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  += (const Matrix4<T>& v) { 	_d[0]  += v._d[0];		_d[1]  += v._d[1];		_d[2]  += v._d[2];		_d[3]  += v._d[3];		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  -= (const Matrix4<T>& v) { 	_d[0]  -= v._d[0];		_d[1]  -= v._d[1];		_d[2]  -= v._d[2];		_d[3]  -= v._d[3];		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  *= (const Matrix4<T>& v) { 	_d[0]  *= v._d[0];		_d[1]  *= v._d[1];		_d[2]  *= v._d[2];		_d[3]  *= v._d[3];		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  /= (const Matrix4<T>& v) { 	_d[0]  /= v._d[0];		_d[1]  /= v._d[1];		_d[2]  /= v._d[2];		_d[3]  /= v._d[3];		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  = ( T v) { 	_d[0]  = v;		_d[1]  = v;		_d[2]  = v;		_d[3]  = v;		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  += ( T v) { 	_d[0]  += v;		_d[1]  += v;		_d[2]  += v;		_d[3]  += v;		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  -= ( T v) { 	_d[0]  -= v;		_d[1]  -= v;		_d[2]  -= v;		_d[3]  -= v;		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  *= ( T v) { 	_d[0]  *= v;		_d[1]  *= v;		_d[2]  *= v;		_d[3]  *= v;		return *this;			};
template<class T> inline Matrix4<T> & Matrix4<T>::operator  /= ( T v) { 	_d[0]  /= v;		_d[1]  /= v;		_d[2]  /= v;		_d[3]  /= v;		return *this;			};

typedef Matrix4<double> Matrix4d;
typedef Matrix4<float> Matrix4f;

#endif
