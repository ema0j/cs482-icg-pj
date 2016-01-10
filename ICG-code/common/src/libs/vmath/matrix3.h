#ifndef _MATRIX3_H_
#define _MATRIX3_H_

// INCLUDES ====================================================
#include "vec3.h"
#include "matrix2.h"

// TYPE DECLARATION ============================================
template<class T>
class Matrix3 {
	// data ---------------------------------------------
protected:
	Vec3<T> _d[3]; // rows of the matrix

public:
	// Constructors -------------------------------------
	Matrix3();
	Matrix3(T* v);
	Matrix3(const Matrix3& v);
	Matrix3(const Vec3<T>& r0, const Vec3<T>& r1, const Vec3<T>& r2);
	Matrix3(T m00, T m01, T m02,
			T m10, T m11, T m12,
			T m20, T m21, T m22);

	// Access ops ---------------------------------------
	const Vec3<T>& operator[](int i) const; // Access the row
	Vec3<T>& operator[](int i);
	const T& operator()(int i, int j) const; // Access the element
	T& operator()(int i, int j);
	Vec3<T> Row(int i) const;
	Vec3<T> Col(int i) const;

	// Comparison ops -----------------------------------
	bool operator == (const Matrix3& v) const;
	bool operator != (const Matrix3& v) const;

	// Binary ops ---------------------------------------
	Matrix3 operator+(const Matrix3& v) const;
	Matrix3 operator-(const Matrix3& v) const;
	Matrix3 operator*(const Matrix3& v) const;
	Matrix3 operator/(const Matrix3& v) const;
	Matrix3 operator+(T v) const;
	Matrix3 operator-(T v) const;
	Matrix3 operator*(T v) const;
	Matrix3 operator/(T v) const;

	// Assignment ops -----------------------------------
	Matrix3& operator=(const Matrix3& v);
	Matrix3& operator=(T v);
	Matrix3& operator+=(const Matrix3& v);
	Matrix3& operator-=(const Matrix3& v);
	Matrix3& operator*=(const Matrix3& v);
	Matrix3& operator/=(const Matrix3& v);
	Matrix3& operator+=(T v);
	Matrix3& operator-=(T v);
	Matrix3& operator*=(T v);
	Matrix3& operator/=(T v);

	// Matrix ops ---------------------------------------
	Matrix3 GetTranspose() const;
	T GetDeterminant() const;
	Matrix3 GetAdjoint() const;
	Matrix3 GetInverse() const;
	Matrix3 operator %(const Matrix3& v) const; // matrix multiply
	Vec3<T> operator %(const Vec3<T>& v) const; // transform

	// Transforms ---------------------------------------
	// Transform assume the multiplication are of the type
	// M % v.
	Vec3<T> TransformPoint(const Vec3<T>& v) const;
	Vec3<T> TransformNormal(const Vec3<T>& v) const;
	Vec3<T> TransformVector(const Vec3<T>& v) const;

	// Typical matrices ---------------------------------
	static Matrix3 Identity();
	static Matrix3 Zero();
};

// IMPLEMENTATION ==============================================
// Constructors -------------------------------------
template<class T> inline Matrix3<T>::Matrix3() {
	_d[0] = _d[1] = _d[2] = 0;
}
template<class T> inline Matrix3<T>::Matrix3(const Matrix3<T>& v) {
	_d[0] = v._d[0];
	_d[1] = v._d[1];
	_d[2] = v._d[2];
}

template<class T> inline Matrix3<T>::Matrix3(const Vec3<T>&  r0, const Vec3<T>&  r1, const Vec3<T>&  r2) {
	this->_d[0] = r0;
	this->_d[1] = r1;
	this->_d[2] = r2;
}

template<class T> inline Matrix3<T>::Matrix3(T m00, T m01, T m02,
						T m10, T m11, T m12,
						T m20, T m21, T m22) {
	this->_d[0] = Vec3<T>(m00,m01,m02);
	this->_d[1] = Vec3<T>(m10,m11,m12);
	this->_d[2] = Vec3<T>(m20,m21,m22);
}

// Access ops ---------------------------------------
template<class T> inline const Vec3<T>& Matrix3<T>::operator[](int i) const {
	return _d[i];
}

template<class T> inline Vec3<T>& Matrix3<T>::operator[](int i) {
	return _d[i];
}

template<class T> inline const T& Matrix3<T>::operator()(int i, int j) const {
	return _d[i][j];
}

template<class T> inline T& Matrix3<T>::operator()(int i, int j) {
	return _d[i][j];
}

template<class T> inline Vec3<T> Matrix3<T>::Row(int i) const {
	return _d[i];
}

template<class T> inline Vec3<T> Matrix3<T>::Col(int i) const {
	return Vec3<T>(_d[0][i], _d[1][i], _d[2][i]);
}

// Matrix ops ---------------------------------------
template<class T> inline Matrix3<T> Matrix3<T>::GetTranspose() const {
	return Matrix3<T>(Col(0), Col(1), Col(2));
}

template<class T> inline T Matrix3<T>::GetDeterminant() const {
    return -_d[0][2]*_d[1][1]*_d[2][0] +
            _d[0][1]*_d[1][2]*_d[2][0] +
            _d[0][2]*_d[1][0]*_d[2][1] -
            _d[0][0]*_d[1][2]*_d[2][1] -
            _d[0][1]*_d[1][0]*_d[2][2] +
            _d[0][0]*_d[1][1]*_d[2][2];
}

template<class T> inline Matrix3<T> Matrix3<T>::GetAdjoint() const {
    return Matrix3<T>( -_d[1][2]*_d[2][1] + _d[1][1]*_d[2][2],
                      _d[0][2]*_d[2][1] - _d[0][1]*_d[2][2],
                     -_d[0][2]*_d[1][1] + _d[0][1]*_d[1][2],
                      _d[1][2]*_d[2][0] - _d[1][0]*_d[2][2],
                     -_d[0][2]*_d[2][0] + _d[0][0]*_d[2][2],
                      _d[0][2]*_d[1][0] - _d[0][0]*_d[1][2],
                     -_d[1][1]*_d[2][0] + _d[1][0]*_d[2][1],
                      _d[0][1]*_d[2][0] - _d[0][0]*_d[2][1],
                     -_d[0][1]*_d[1][0] + _d[0][0]*_d[1][1]);
}

template<class T> inline Matrix3<T> Matrix3<T>::GetInverse() const {
    return GetAdjoint() * (1 / GetDeterminant());
}

template<class T> inline Matrix3<T> Matrix3<T>::operator %(const Matrix3<T>& v) const {
	Matrix3<T> t = v.GetTranspose();
	return Matrix3<T>(_d[0] % t._d[0], _d[0] % t._d[1], _d[0] % t._d[2],
				   _d[1] % t._d[0], _d[1] % t._d[1], _d[1] % t._d[2],
				   _d[2] % t._d[0], _d[2] % t._d[1], _d[2] % t._d[2]);
}

template<class T> inline Vec3<T> Matrix3<T>::operator %(const Vec3<T>& v) const {
	return Vec3<T>(_d[0] % v, _d[1] % v, _d[2] % v);
}

// Transform ops ---------------------------------------
template<class T> inline Vec3<T> Matrix3<T>::TransformPoint(const Vec3<T>& v) const {
	return Vec3<T>(_d[0] % v, _d[1] % v, _d[2] % v);
}

template<class T> inline Vec3<T> Matrix3<T>::TransformVector(const Vec3<T>& v) const {
	return Vec3<T>(_d[0] % v, _d[1] % v, _d[2] % v);
}

template<class T> inline Vec3<T> Matrix3<T>::TransformNormal(const Vec3<T>& v) const {
	Matrix3<T> adj = GetAdjoint();
	return adj.TransformVector(v).GetNormalized();
}

// Typical matrices ---------------------------------
template<class T> inline Matrix3<T> Matrix3<T>::Identity() {
	return Matrix3<T>(Vec3<T>(1,0,0), Vec3<T>(0,1,0), Vec3<T>(0,0,1));
}

template<class T> inline Matrix3<T> Matrix3<T>::Zero() {
	return Matrix3<T>(Vec3<T>(0,0,0), Vec3<T>(0,0,0), Vec3<T>(0,0,0));
}

// Comparison ops -----------------------------------
template<class T> inline bool Matrix3<T>::operator  == (const Matrix3<T>& v) const { 	return 		_d[0]  == v._d[0] && 		_d[1]  == v._d[1] && 		_d[2]  == v._d[2]; };
template<class T> inline bool Matrix3<T>::operator  != (const Matrix3<T>& v) const { 	return 		_d[0]  != v._d[0] || 		_d[1]  != v._d[1] || 		_d[2]  != v._d[2]; };

// Binary ops ---------------------------------------
template<class T> inline Matrix3<T> Matrix3<T>::operator  + (const Matrix3<T>& v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  + v._d[0];		ret._d[1] = _d[1]  + v._d[1];		ret._d[2] = _d[2]  + v._d[2];		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  - (const Matrix3<T>& v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  - v._d[0];		ret._d[1] = _d[1]  - v._d[1];		ret._d[2] = _d[2]  - v._d[2];		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  * (const Matrix3<T>& v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  * v._d[0];		ret._d[1] = _d[1]  * v._d[1];		ret._d[2] = _d[2]  * v._d[2];		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  / (const Matrix3<T>& v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  / v._d[0];		ret._d[1] = _d[1]  / v._d[1];		ret._d[2] = _d[2]  / v._d[2];		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  + ( T v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  + v;		ret._d[1] = _d[1]  + v;		ret._d[2] = _d[2]  + v;		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  - ( T v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  - v;		ret._d[1] = _d[1]  - v;		ret._d[2] = _d[2]  - v;		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  * ( T v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  * v;		ret._d[1] = _d[1]  * v;		ret._d[2] = _d[2]  * v;		return ret;			};
template<class T> inline Matrix3<T> Matrix3<T>::operator  / ( T v) const { 	Matrix3<T> ret; 	ret._d[0] = _d[0]  / v;		ret._d[1] = _d[1]  / v;		ret._d[2] = _d[2]  / v;		return ret;			};

// Assignment ops -----------------------------------
template<class T> inline Matrix3<T> & Matrix3<T>::operator  = (const Matrix3<T>& v) { 	_d[0]  = v._d[0];		_d[1]  = v._d[1];		_d[2]  = v._d[2];		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  += (const Matrix3<T>& v) { 	_d[0]  += v._d[0];		_d[1]  += v._d[1];		_d[2]  += v._d[2];		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  -= (const Matrix3<T>& v) { 	_d[0]  -= v._d[0];		_d[1]  -= v._d[1];		_d[2]  -= v._d[2];		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  *= (const Matrix3<T>& v) { 	_d[0]  *= v._d[0];		_d[1]  *= v._d[1];		_d[2]  *= v._d[2];		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  /= (const Matrix3<T>& v) { 	_d[0]  /= v._d[0];		_d[1]  /= v._d[1];		_d[2]  /= v._d[2];		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  = ( T v) { 	_d[0]  = v;		_d[1]  = v;		_d[2]  = v;		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  += ( T v) { 	_d[0]  += v;		_d[1]  += v;		_d[2]  += v;		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  -= ( T v) { 	_d[0]  -= v;		_d[1]  -= v;		_d[2]  -= v;		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  *= ( T v) { 	_d[0]  *= v;		_d[1]  *= v;		_d[2]  *= v;		return *this;			};
template<class T> inline Matrix3<T> & Matrix3<T>::operator  /= ( T v) { 	_d[0]  /= v;		_d[1]  /= v;		_d[2]  /= v;		return *this;			};

typedef Matrix3<float> Matrix3f;
typedef Matrix3<double> Matrix3d;

#endif
