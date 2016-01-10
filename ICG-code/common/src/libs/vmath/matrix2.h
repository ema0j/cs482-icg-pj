#ifndef _MATRIX2_H_
#define _MATRIX2_H_

// INCLUDES ====================================================
#include "vec2.h"

// TYPE DECLARATION ============================================
template<class T>
class Matrix2 {
	// data ---------------------------------------------
protected:
	Vec2<T> _d[3]; // rows of the matrix

public:
	// Constructors -------------------------------------
	Matrix2();
	Matrix2(T* v);
	Matrix2(const Matrix2& v);
	Matrix2(const Vec2<T>& r0, const Vec2<T>& r1);
	Matrix2(T m00, T m01,
			T m10, T m11);

	// Access ops ---------------------------------------
	const Vec2<T>& operator[](int i) const; // Access the row
	Vec2<T>& operator[](int i);
	const T& operator()(int i, int j) const; // Access the element
	T& operator()(int i, int j);
	Vec2<T> Row(int i) const;
	Vec2<T> Col(int i) const;

	// Comparison ops -----------------------------------
	bool operator == (const Matrix2& v) const;
	bool operator != (const Matrix2& v) const;

	// Binary ops ---------------------------------------
	Matrix2 operator+(const Matrix2& v) const;
	Matrix2 operator-(const Matrix2& v) const;
	Matrix2 operator*(const Matrix2& v) const;
	Matrix2 operator/(const Matrix2& v) const;
	Matrix2 operator+(T v) const;
	Matrix2 operator-(T v) const;
	Matrix2 operator*(T v) const;
	Matrix2 operator/(T v) const;

	// Assignment ops -----------------------------------
	Matrix2& operator=(const Matrix2& v);
	Matrix2& operator=(T v);
	Matrix2& operator+=(const Matrix2& v);
	Matrix2& operator-=(const Matrix2& v);
	Matrix2& operator*=(const Matrix2& v);
	Matrix2& operator/=(const Matrix2& v);
	Matrix2& operator+=(T v);
	Matrix2& operator-=(T v);
	Matrix2& operator*=(T v);
	Matrix2& operator/=(T v);

	// Matrix ops ---------------------------------------
	Matrix2 GetTranspose() const;
	T GetDeterminant() const;
	Matrix2 GetAdjoint() const;
	Matrix2 GetInverse() const;
	Matrix2 operator %(const Matrix2& v) const; // matrix multiply
	Vec2<T> operator %(const Vec2<T>& v) const; // transform

	// Transforms ---------------------------------------
	// Transform assume the multiplication are of the type
	// M % v.
	Vec2<T> TransformPoint(const Vec2<T>& v) const;
	Vec2<T> TransformNormal(const Vec2<T>& v) const;
	Vec2<T> TransformVector(const Vec2<T>& v) const;

	// Typical matrices ---------------------------------
	static Matrix2 Identity();
	static Matrix2 Zero();
};

// IMPLEMENTATION ==============================================
// Constructors -------------------------------------
template<class T> inline Matrix2<T>::Matrix2() {
	_d[0] = _d[1] = 0;
}
template<class T> inline Matrix2<T>::Matrix2(const Matrix2<T>& v) {
	_d[0] = v._d[0];
	_d[1] = v._d[1];
}

template<class T> inline Matrix2<T>::Matrix2(const Vec2<T>&  r0, const Vec2<T>&  r1) {
	this->_d[0] = r0;
	this->_d[1] = r1;
}

template<class T> inline Matrix2<T>::Matrix2(T m00, T m01,
						T m10, T m11) {
	this->_d[0] = Vec2<T>(m00,m01);
	this->_d[1] = Vec2<T>(m10,m11);
}

// Access ops ---------------------------------------
template<class T> inline const Vec2<T>& Matrix2<T>::operator[](int i) const {
	return _d[i];
}

template<class T> inline Vec2<T>& Matrix2<T>::operator[](int i) {
	return _d[i];
}

template<class T> inline const T& Matrix2<T>::operator()(int i, int j) const {
	return _d[i][j];
}

template<class T> inline T& Matrix2<T>::operator()(int i, int j) {
	return _d[i][j];
}

template<class T> inline Vec2<T> Matrix2<T>::Row(int i) const {
	return _d[i];
}

template<class T> inline Vec2<T> Matrix2<T>::Col(int i) const {
	return Vec2<T>(_d[0][i], _d[1][i]);
}

// Matrix ops ---------------------------------------
template<class T> inline Matrix2<T> Matrix2<T>::GetTranspose() const {
	return Matrix2<T>(Col(0), Col(1));
}

template<class T> inline T Matrix2<T>::GetDeterminant() const {
    return _d[0][0]*_d[1][1] - _d[0][1]*_d[1][0];
}

template<class T> inline Matrix2<T> Matrix2<T>::GetAdjoint() const {
    return Matrix2<T>(_d[1][1],-_d[0][1],-_d[1][0],_d[0][0]);}

template<class T> inline Matrix2<T> Matrix2<T>::GetInverse() const {
    return GetAdjoint() * (1 / GetDeterminant());
}

template<class T> inline Matrix2<T> Matrix2<T>::operator %(const Matrix2<T>& v) const {
	Matrix2<T> t = v.GetTranspose();
	return Matrix2<T>(_d[0] % t._d[0], _d[0] % t._d[1],
				   _d[1] % t._d[0], _d[1] % t._d[1]);
}

template<class T> inline Vec2<T> Matrix2<T>::operator %(const Vec2<T>& v) const {
	return Vec2<T>(_d[0] % v, _d[1] % v);
}

// Transform ops ---------------------------------------
template<class T> inline Vec2<T> Matrix2<T>::TransformPoint(const Vec2<T>& v) const {
	return Vec2<T>(_d[0] % v, _d[1] % v);
}

template<class T> inline Vec2<T> Matrix2<T>::TransformVector(const Vec2<T>& v) const {
	return Vec2<T>(_d[0] % v, _d[1] % v);
}

template<class T> inline Vec2<T> Matrix2<T>::TransformNormal(const Vec2<T>& v) const {
	Matrix2<T> adj = GetAdjoint();
	return adj.TransformVector(v).GetNormalized();
}

// Typical matrices ---------------------------------
template<class T> inline Matrix2<T> Matrix2<T>::Identity() {
	return Matrix2<T>(Vec2<T>(1,0), Vec2<T>(0,1));
}

template<class T> inline Matrix2<T> Matrix2<T>::Zero() {
	return Matrix2<T>(Vec2<T>(0,0), Vec2<T>(0,0));
}

// Comparison ops -----------------------------------
template<class T> inline bool Matrix2<T>::operator  == (const Matrix2<T>& v) const { 	return 		_d[0]  == v._d[0] && 		_d[1]  == v._d[1]; };
template<class T> inline bool Matrix2<T>::operator  != (const Matrix2<T>& v) const { 	return 		_d[0]  != v._d[0] || 		_d[1]  != v._d[1]; };

// Binary ops ---------------------------------------
template<class T> inline Matrix2<T> Matrix2<T>::operator  + (const Matrix2<T>& v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  + v._d[0];		ret._d[1] = _d[1]  + v._d[1];		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  - (const Matrix2<T>& v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  - v._d[0];		ret._d[1] = _d[1]  - v._d[1];		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  * (const Matrix2<T>& v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  * v._d[0];		ret._d[1] = _d[1]  * v._d[1];		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  / (const Matrix2<T>& v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  / v._d[0];		ret._d[1] = _d[1]  / v._d[1];		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  + ( T v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  + v;		ret._d[1] = _d[1]  + v;		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  - ( T v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  - v;		ret._d[1] = _d[1]  - v;		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  * ( T v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  * v;		ret._d[1] = _d[1]  * v;		return ret;			};
template<class T> inline Matrix2<T> Matrix2<T>::operator  / ( T v) const { 	Matrix2<T> ret; 	ret._d[0] = _d[0]  / v;		ret._d[1] = _d[1]  / v;		return ret;			};

// Assignment ops -----------------------------------
template<class T> inline Matrix2<T> & Matrix2<T>::operator  = (const Matrix2<T>& v) { 	_d[0]  = v._d[0];		_d[1]  = v._d[1];		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  += (const Matrix2<T>& v) { 	_d[0]  += v._d[0];		_d[1]  += v._d[1];		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  -= (const Matrix2<T>& v) { 	_d[0]  -= v._d[0];		_d[1]  -= v._d[1];		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  *= (const Matrix2<T>& v) { 	_d[0]  *= v._d[0];		_d[1]  *= v._d[1];		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  /= (const Matrix2<T>& v) { 	_d[0]  /= v._d[0];		_d[1]  /= v._d[1];		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  = ( T v) { 	_d[0]  = v;		_d[1]  = v;		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  += ( T v) { 	_d[0]  += v;		_d[1]  += v;		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  -= ( T v) { 	_d[0]  -= v;		_d[1]  -= v;		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  *= ( T v) { 	_d[0]  *= v;		_d[1]  *= v;		return *this;			};
template<class T> inline Matrix2<T> & Matrix2<T>::operator  /= ( T v) { 	_d[0]  /= v;		_d[1]  /= v;		return *this;			};

typedef Matrix2<float> Matrix2f;
typedef Matrix2<double> Matrix2d;

#endif
