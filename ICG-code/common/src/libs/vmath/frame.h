#ifndef _FRAME_H_
#define _FRAME_H_

// INCLUDES ====================================================
#include "vec3.h"
#include "matrix4.h"

// TYPE DECLARATION ============================================
template<class T>
class Frame {
	// data ---------------------------------------------
protected:
	Vec3<T> _o;			// origin
	Vec3<T> _x, _y, _z;	// xyz axis

public:
	// Constructors -------------------------------------
	Frame(const Vec3<T>& o = Vec3<T>::Zero(),
		  const Vec3<T>& x = Vec3<T>::X(),
		  const Vec3<T>& y = Vec3<T>::Y(),
		  const Vec3<T>& z = Vec3<T>::Z());

	// Setters ------------------------------------------
	void SetOrigin(const Vec3<T>& o);
	void SetX(const Vec3<T>& x);
	void SetY(const Vec3<T>& y);
	void SetZ(const Vec3<T>& z);

	// Getters ------------------------------------------
	Vec3<T> GetOrigin() const;
	Vec3<T> GetX() const;
	Vec3<T> GetY() const;
	Vec3<T> GetZ() const;

	// Conversions --------------------------------------
	Matrix4d ComputeMatrix() const;
	Matrix4d ComputeMatrixGL() const;
	operator Matrix4d() const;

	// Transforms ---------------------------------------
    // These function assume that the matrix is affine
    // with no shearing and has uniform scaling
    void Transform(const Matrix4d& m);
    Frame GetTransformed(const Matrix4d& m) const;

	// Access ops ---------------------------------------

	// Comparison ops -----------------------------------
	int operator == (const Frame& v) const;
	int operator != (const Frame& v) const;

	// Assignment ops -----------------------------------
	Frame& operator=(const Frame& v);

    // Other construction
    void RenormalizeFromZY();
    void RenormalizeFromZ();
};

// IMPLEMENTATION ==============================================
// Constructors -------------------------------------
template<class T> inline Frame<T>::Frame(const Vec3<T>& o, const Vec3<T>& x,
					const Vec3<T>& y, const Vec3<T>& z) {
    _o = o;
    _x = x;
    _y = y;
    _z = z;
}

// Setters ------------------------------------------
template<class T> inline void Frame<T>::SetOrigin(const Vec3<T>& o) {
	_o = o;
}

template<class T> inline void Frame<T>::SetX(const Vec3<T>& x) {
	_x = x;
}

template<class T> inline void Frame<T>::SetY(const Vec3<T>& y) {
	_y = y;
}

template<class T> inline void Frame<T>::SetZ(const Vec3<T>& z) {
	_z = z;
}

// Getters ------------------------------------------
template<class T> inline Vec3<T> Frame<T>::GetOrigin() const {
	return _o;
}

template<class T> inline Vec3<T> Frame<T>::GetX() const {
	return _x;
}

template<class T> inline Vec3<T> Frame<T>::GetY() const {
	return _y;
}

template<class T> inline Vec3<T> Frame<T>::GetZ() const {
	return _z;
}

// Conversions --------------------------------------
template<class T> inline Matrix4d Frame<T>::ComputeMatrix() const {
	return Matrix4d::LookAtGL(_o, _o + _z, _y);
}

template<class T> inline Matrix4d Frame<T>::ComputeMatrixGL() const {
	return Matrix4d::LookAtGL(_o, _o - _z, _y);
}

template<class T> inline Frame<T>::operator Matrix4d() const {
	return ComputeMatrix();
}

// Transforms ---------------------------------------
template<class T> inline void Frame<T>::Transform(const Matrix4d& m) {
    Vec3<T> o = m.TransformPoint(_o);
    Vec3<T> x = (m.TransformPoint(_o + _x) - o).GetNormalized();
    Vec3<T> y = (m.TransformPoint(_o + _y) - o).GetNormalized();
    Vec3<T> z = (m.TransformPoint(_o + _z) - o).GetNormalized();
    _o = o;
    _x = x;
    _y = y;
    _z = z;
}

template<class T> inline Frame<T> Frame<T>::GetTransformed(const Matrix4d& m) const {
    Frame<T> ret = *this;
    ret.Transform(m);
    return ret;
}

// Comparison ops -----------------------------------
template<class T> inline int Frame<T>::operator == (const Frame<T>& v) const {
	return _o == v._o &&
		   _x == v._x &&
		   _y == v._y &&
		   _z == v._z;
}

template<class T> inline int Frame<T>::operator != (const Frame<T>& v) const {
	return !operator==(v);
}

// Assignment ops -----------------------------------
template<class T> inline Frame<T>& Frame<T>::operator=(const Frame<T>& v) {
	_o = v._o;
	_x = v._x;
	_y = v._y;
	_z = v._z;

	return *this;
}

// Other construction -------------------------------
template<class T> inline void Frame<T>::RenormalizeFromZY() {
    _z.Normalize();
    _x = (_y ^ _z).GetNormalized();
    _y = _z ^ _x;
}

template<class T> inline void Frame<T>::RenormalizeFromZ() {
    _z.Normalize();
    double zDotSX = _z % Vec3<T>::X();
    zDotSX = ((zDotSX < 0) ? (-zDotSX) : (zDotSX))-1;
    if (fabs(zDotSX) < 1e-3) {
      _x = Vec3<T>::Y();
    } else {
      _x = Vec3<T>::X();
    }
    _y = (_z ^ _x).GetNormalized();
    _x = _y ^ _z;
}

#endif
