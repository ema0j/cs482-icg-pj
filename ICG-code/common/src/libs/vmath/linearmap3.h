#ifndef _LINEARMAP3_H_
#define _LINEARMAP3_H_

// INCLUDES ====================================================
#include "geom3.h"
#include "matrix4.h"

template<class T>
class LinearMap3 {
	// data ---------------------------------------------
protected:
    Matrix4<T> m, mi;
    bool isIdentity;
    bool isRigid;
    bool isAffine;

public:
	// Constructors -------------------------------------
    LinearMap3();

	// Properties ---------------------------------------
    bool IsIdentity() const { return isIdentity; }
    bool IsRigid() const { return isRigid; }
    bool IsAffine() const { return isAffine; }
    Matrix4<T> GetMatrix() const { return m; }

	// Matrix Ops ---------------------------------------
    LinearMap3 GetInverse() const { return LinearMap3(mi, m, isIdentity, isRigid, isAffine); }
    LinearMap3 operator%(const LinearMap3<T>& v) const { return LinearMap3(m % v.m, v.mi % mi, 
        isIdentity && v.isIdentity, isRigid && v.isRigid, isAffine && v.isAffine); }

	// Transforms ---------------------------------------
	template<class TT> Point3<TT> TransformPoint(const Point3<TT>& v) const;
	template<class TT> Normal3<TT> TransformNormal(const Normal3<TT>& v) const;
	template<class TT> Vector3<TT> TransformVector(const Vector3<TT>& v) const;
	template<class TT> Direction3<TT> TransformDirection(const Direction3<TT>& v) const;
	template<class TT> PointRange3<TT> TransformBBox(const PointRange3<TT>& v) const;

	// Array Transforms ---------------------------------
	template<class TT> void TransformPointArray(carray<Point3<TT> >& v) const;
	template<class TT> void TransformVectorArray(carray<Vector3<TT> >& v) const;
	template<class TT> void TransformNormalArray(carray<Normal3<TT> >& v) const;
	template<class TT> void TransformDirectionArray(carray<Direction3<TT> >& v) const;
	template<class TT> void TransformPointArray(Point3<TT>* v, int l) const;
	template<class TT> void TransformVectorArray(Vector3<TT>* v, int l) const;
	template<class TT> void TransformNormalArray(Normal3<TT>* v, int l) const;
	template<class TT> void TransformDirectionArray(Direction3<TT>* v, int l) const;

	// Typical maps -------------------------------------
	template<class TT> static LinearMap3 Translation(const Vector3<TT>& v);
	template<class TT> static LinearMap3 Rotation(const Direction3<TT>& v, TT angle);
	template<class TT> static LinearMap3 RotationX(TT angle);
	template<class TT> static LinearMap3 RotationY(TT angle);
	template<class TT> static LinearMap3 RotationZ(TT angle);
	template<class TT> static LinearMap3 Scale(TT v);
	template<class TT> static LinearMap3 LookAt(const Point3<TT>& eye, const Point3<TT>& center, const Direction3<TT>& up, bool flipz); // along positive z
    template<class TT> static LinearMap3 Transform(const Matrix4<TT>& m);

private:
	// Constructors -------------------------------------
    LinearMap3(const Matrix4<T>& m, const Matrix4<T>& mi, 
        bool isIdentity, bool isRigid, bool isAffine) : 
            m(m), mi(mi), 
            isIdentity(isIdentity), isRigid(isRigid), isAffine(isAffine) {}
};

template<class T>
LinearMap3<T>::LinearMap3() : m(Matrix4<T>::Identity()), mi(Matrix4<T>::Identity()), isIdentity(true), isRigid(true), isAffine(true) { }

// Transforms ---------------------------------------
template<class T> template<class TT> 
inline Point3<TT> LinearMap3<T>::TransformPoint(const Point3<TT>& v) const { return m.TransformPoint(v); }
template<class T> template<class TT> 
inline Normal3<TT> LinearMap3<T>::TransformNormal(const Normal3<TT>& v) const { return m.GetTranspose().TransformNormalNoAdjoint(v); }
template<class T> template<class TT> 
inline Vector3<TT> LinearMap3<T>::TransformVector(const Vector3<TT>& v) const { return m.TransformVector(v); }
template<class T> template<class TT> 
inline Direction3<TT> LinearMap3<T>::TransformDirection(const Direction3<TT>& v) const { return m.TransformNormalizedVector(v); }
template<class T> template<class TT> 
inline PointRange3<TT> LinearMap3<T>::TransformBBox(const PointRange3<TT>& v) const { return m.TransformBBox(v); }

// Array Transforms ---------------------------------
template<class T> template<class TT> 
inline void LinearMap3<T>::TransformPointArray(carray<Point3<TT> >& v) const {
    TransformPointArray(v.data(), v.size());
}
template<class T> template<class TT> 
inline void LinearMap3<T>::TransformVectorArray(carray<Vector3<TT> >& v) const {
    TransformVectorArray(v.data(), v.size());
}
template<class T> template<class TT> inline void LinearMap3<T>::TransformDirectionArray(carray<Direction3<TT> >& v) const {
    TransformDirectionArray(v.data(), v.size());
}

template<class T> template<class TT> inline void LinearMap3<T>::TransformNormalArray(carray<Normal3<TT> >& v) const {
    TransformNormalArray(v.data(), v.size());
}

template<class T> template<class TT> inline void LinearMap3<T>::TransformPointArray(Point3<TT>* v, int l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformPoint(v[i]);
}

template<class T> template<class TT> inline void LinearMap3<T>::TransformVectorArray(Vector3<TT>* v, int l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformVector(v[i]);
}

template<class T> template<class TT> inline void LinearMap3<T>::TransformDirectionArray(Direction3<TT>* v, int l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformDirection(v[i]);
}

template<class T> template<class TT> inline void LinearMap3<T>::TransformNormalArray(Normal3<TT>* v, int l) const {
    for(int i = 0; i < l; i ++) v[i] = TransformNormal(v[i]);
}

// Typical maps -------------------------------------
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::Translation(const Vector3<TT>& v) {
    return LinearMap3<T>(Matrix4<T>::Translation(v), Matrix4<T>::Translation(-v), v.IsZero(), true, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::Rotation(const Direction3<TT>& v, TT a) {
    return LinearMap3<T>(Matrix4<T>::Rotation(v,a), Matrix4<T>::Rotation(v,-a), a == 0, true, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::RotationX(TT a) {
    return LinearMap3<T>(Matrix4<T>::RotationX(a), Matrix4<T>::RotationX(-a), a == 0, true, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::RotationY(TT a) {
    return LinearMap3<T>(Matrix4<T>::RotationY(a), Matrix4<T>::RotationY(-a), a == 0, true, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::RotationZ(TT a) {
    return LinearMap3<T>(Matrix4<T>::RotationZ(a), Matrix4<T>::RotationZ(-a), a == 0, true, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::Scale(TT v) {
    return LinearMap3<T>(Matrix4<T>::Scale(v), Matrix4<T>::Scale(1/v), v == 1, v == 1, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::LookAt(const Point3<TT>& eye, const Point3<TT>& center, const Direction3<TT>& up, bool flipz) {
    Matrix4<T> mm = Matrix4<T>::LookAt(eye, center, up, flipz);
    return LinearMap3<T>(mm, mm.GetInverse(), false, true, true);
}
template<class T> template<class TT> 
inline LinearMap3<T> LinearMap3<T>::Transform(const Matrix4<TT>& m) {
    return LinearMap3<T>(m, m.GetInverse(), m.IsIdentity(), m.IsRigid(), m.IsAffine());
}

// Types -------------------------------------
typedef LinearMap3<float> LinearMap3f;
typedef LinearMap3<double> LinearMap3d;

#endif
