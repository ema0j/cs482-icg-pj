#ifndef _VEC4T_H_
#define _VEC4T_H_

// INCLUDES ====================================================
#include "functions.h"
#include "vec3.h"

template<class T>
class Vec4 {
	// data ---------------------------------------------
public:
	T x, y, z, w;

public:
	// Constructors -------------------------------------
	Vec4();
	Vec4(T v);
	Vec4(T x, T y, T z, T w);
	Vec4(const Vec4& v);
	template<class TT>
	Vec4(const Vec4<TT>& v);
	Vec4(const Vec3<T>& v, T w);

	static Vec4 Zero();
	static Vec4 One();
	static Vec4 X();
	static Vec4 Y();
	static Vec4 Z();
	static Vec4 W();

	// Conversions --------------------------------------
	operator const T* () const;
	operator T* ();
	template<class TT>
	operator Vec4<TT>() const;
	Vec3<T> Project() const;
	Vec3<T> IgnoreLast() const;
	Vec3<T> MultiplyLast() const;

	// Access ops ---------------------------------------
	const T& operator[](int i) const;
	T& operator[](int i);
    static int8_t NumComponents();

	// Comparison ops -----------------------------------
	bool operator == (const Vec4& v) const;
	bool operator != (const Vec4& v) const;
	bool operator < (const Vec4& v) const;
	bool operator > (const Vec4& v) const;
	bool operator <= (const Vec4& v) const;
	bool operator >= (const Vec4& v) const;
    bool IsZero() const;

	// Unary ops ----------------------------------------
    Vec4 operator-() const;

	// Binary ops ---------------------------------------
	Vec4 operator+(const Vec4& v) const;
	Vec4 operator-(const Vec4& v) const;
	Vec4 operator*(const Vec4& v) const;
	Vec4 operator/(const Vec4& v) const;
	Vec4 operator+(T v) const;
	Vec4 operator-(T v) const;
	Vec4 operator*(T v) const;
	Vec4 operator/(T v) const;

	// Assignment ops -----------------------------------
	Vec4& operator=(const Vec4& v);
	Vec4& operator=(T v);
	Vec4& operator+=(const Vec4& v);
	Vec4& operator-=(const Vec4& v);
	Vec4& operator*=(const Vec4& v);
	Vec4& operator/=(const Vec4& v);
	Vec4& operator+=(T v);
	Vec4& operator-=(T v);
	Vec4& operator*=(T v);
	Vec4& operator/=(T v);

	// Vector ops ---------------------------------------
	T operator %(const Vec4& v) const; // dot
	T GetLength() const;
	T GetLengthSqr() const;
	Vec4 GetNormalized() const;
	void Normalize();
    T Average() const;

	// Functions ----------------------------------------
    static Vec4 Min(const Vec4& a, const Vec4& b);
    static Vec4 Max(const Vec4& a, const Vec4& b);
    Vec4        Abs() const;
};

// Constructors -------------------------------------
template <class T>
inline Vec4<T>::Vec4() {
	x = y = z = w = 0;
}
template <class T>
inline Vec4<T>::Vec4(T v) {
	x = y = z = w = v;
}
template <class T>
inline Vec4<T>::Vec4(T x, T y, T z, T w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
template <class T>
inline Vec4<T>::Vec4(const Vec4<T>& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}
template <class T>
inline Vec4<T>::Vec4(const Vec3<T>& v, T w) {
	x = v.x;
	y = v.y;
	z = v.z;
	this->w = w;
}
template <class T> template<class TT>
inline Vec4<T>::Vec4(const Vec4<TT>& v) {
	x = (T)v.x;
	y = (T)v.y;
	z = (T)v.z;
	w = (T)v.w;
}
template <class T>
inline Vec4<T> Vec4<T>::Zero() {
	Vec4<T> ret;
	ret.x = 0;
	ret.y = 0;
	ret.z = 0;
	ret.w = 0;
	return ret;
}
template <class T>
inline Vec4<T> Vec4<T>::One() {
	Vec4<T> ret;
	ret.x = 1;
	ret.y = 1;
	ret.z = 1;
	ret.w = 1;
	return ret;
}
template <class T>
inline Vec4<T> Vec4<T>::X() {
	Vec4<T> ret;
	ret.x = 1;
	ret.y = 0;
	ret.z = 0;
	ret.w = 0;
	return ret;
}
template <class T>
inline Vec4<T> Vec4<T>::Y() {
	Vec4<T> ret;
	ret.x = 0;
	ret.y = 1;
	ret.z = 0;
	ret.w = 0;
	return ret;
}
template <class T>
inline Vec4<T> Vec4<T>::Z() {
	Vec4<T> ret;
	ret.x = 0;
	ret.y = 0;
	ret.z = 1;
	ret.w = 0;
	return ret;
}
template <class T>
inline Vec4<T> Vec4<T>::W() {
	Vec4<T> ret;
	ret.x = 0;
	ret.y = 0;
	ret.z = 0;
	ret.w = 1;
	return ret;
}

// Conversions --------------------------------------
template <class T>
inline Vec4<T>::operator const T* () const {
	return &x;
}
template <class T>
inline Vec4<T>::operator T* () {
	return &x;
}
template <class T> template<class TT>
inline Vec4<T>::operator Vec4<TT> () const {
	return Vec4<TT>(*this);
}
template <class T>
inline Vec3<T> Vec4<T>::Project() const {
	return Vec3<T>(x/w, y/w, z/w);
}
template <class T>
inline Vec3<T> Vec4<T>::IgnoreLast() const {
	return Vec3<T>(x, y, z);
}
template <class T>
inline Vec3<T> Vec4<T>::MultiplyLast() const {
	return Vec3<T>(x*w, y*w, z*w);
}

// Access ops ---------------------------------------
template <class T>
inline const T& Vec4<T>::operator[](int i) const {
	return (&x)[i];
}
template <class T>
inline T& Vec4<T>::operator[](int i) {
	return (&x)[i];
}
template<class T>
inline int8_t Vec4<T>::NumComponents() {
    return 4;
}
template<class T>
inline T Vec4<T>::Average() const {
    return (x + y + z + w) / 4;
}

// Vector ops ---------------------------------------
template <class T>
inline T Vec4<T>::operator %(const Vec4<T>& v) const {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}
template <class T>
inline T Vec4<T>::GetLength() const {
	return (T)sqrt(x*x+y*y+z*z+w*w);
}
template <class T>
inline T Vec4<T>::GetLengthSqr() const {
	return x*x+y*y+z*z+w*w;
}
template <class T>
inline Vec4<T> Vec4<T>::GetNormalized() const {
	T l = GetLength();
	if(l > 0) {
		return operator / (l);
	} else {
		return *this;
	}
}
template <class T>
inline void Vec4<T>::Normalize() {
	T l = GetLength();
	operator /= (l);
}

// Comparison ops -----------------------------------
template <class T> inline bool Vec4<T>::operator  == (const Vec4<T>& v) const { 	return 		x  == v.x && 		y  == v.y && 		z  == v.z && 		w  == v.w; };
template <class T> inline bool Vec4<T>::operator  != (const Vec4<T>& v) const { 	return 		x  != v.x || 		y  != v.y || 		z  != v.z || 		w  != v.w; };
template <class T> inline bool Vec4<T>::operator  < (const Vec4<T>& v) const { 	return 		x  < v.x && 		y  < v.y && 		z  < v.z && 		w  < v.w; };
template <class T> inline bool Vec4<T>::operator  > (const Vec4<T>& v) const { 	return 		x  > v.x && 		y  > v.y && 		z  > v.z && 		w  > v.w; };
template <class T> inline bool Vec4<T>::operator  <= (const Vec4<T>& v) const { 	return 		x  <= v.x && 		y  <= v.y && 		z  <= v.z && 		w  <= v.w; };
template <class T> inline bool Vec4<T>::operator  >= (const Vec4<T>& v) const { 	return 		x  >= v.x && 		y  >= v.y && 		z  >= v.z && 		w  >= v.w; };
template <class T> inline bool Vec4<T>::IsZero () const { 	return 		x  == 0 && 		y  == 0 && 		z  == 0 && 		w  == 0; };

// Unary ops ----------------------------------------
template <class T> inline Vec4<T> Vec4<T>::operator  - () const { 	Vec4<T> ret; 	ret.x =  - (x); 	ret.y =  - (y); 	ret.z =  - (z); 	ret.w =  - (w); 	return ret; };

// Binary ops ---------------------------------------
template <class T> inline Vec4<T> Vec4<T>::operator  + (const Vec4<T>& v) const { 	Vec4<T> ret; 	ret.x = x  + v.x;		ret.y = y  + v.y;		ret.z = z  + v.z;		ret.w = w  + v.w;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  - (const Vec4<T>& v) const { 	Vec4<T> ret; 	ret.x = x  - v.x;		ret.y = y  - v.y;		ret.z = z  - v.z;		ret.w = w  - v.w;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  * (const Vec4<T>& v) const { 	Vec4<T> ret; 	ret.x = x  * v.x;		ret.y = y  * v.y;		ret.z = z  * v.z;		ret.w = w  * v.w;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  / (const Vec4<T>& v) const { 	Vec4<T> ret; 	ret.x = x  / v.x;		ret.y = y  / v.y;		ret.z = z  / v.z;		ret.w = w  / v.w;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  + ( T v) const { 	Vec4<T> ret; 	ret.x = x  + v;		ret.y = y  + v;		ret.z = z  + v;		ret.w = w  + v;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  - ( T v) const { 	Vec4<T> ret; 	ret.x = x  - v;		ret.y = y  - v;		ret.z = z  - v;		ret.w = w  - v;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  * ( T v) const { 	Vec4<T> ret; 	ret.x = x  * v;		ret.y = y  * v;		ret.z = z  * v;		ret.w = w  * v;		return ret;			};
template <class T> inline Vec4<T> Vec4<T>::operator  / ( T v) const { 	Vec4<T> ret; 	ret.x = x  / v;		ret.y = y  / v;		ret.z = z  / v;		ret.w = w  / v;		return ret;			};

// Assignment ops -----------------------------------
template <class T> inline Vec4<T> & Vec4<T>::operator  = (const Vec4<T>& v) { 	x  = v.x;		y  = v.y;		z  = v.z;		w  = v.w;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  += (const Vec4<T>& v) { 	x  += v.x;		y  += v.y;		z  += v.z;		w  += v.w;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  -= (const Vec4<T>& v) { 	x  -= v.x;		y  -= v.y;		z  -= v.z;		w  -= v.w;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  *= (const Vec4<T>& v) { 	x  *= v.x;		y  *= v.y;		z  *= v.z;		w  *= v.w;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  /= (const Vec4<T>& v) { 	x  /= v.x;		y  /= v.y;		z  /= v.z;		w  /= v.w;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  = ( T v) { 	x  = v;		y  = v;		z  = v;		w  = v;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  += ( T v) { 	x  += v;		y  += v;		z  += v;		w  += v;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  -= ( T v) { 	x  -= v;		y  -= v;		z  -= v;		w  -= v;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  *= ( T v) { 	x  *= v;		y  *= v;		z  *= v;		w  *= v;		return *this;			};
template <class T> inline Vec4<T> & Vec4<T>::operator  /= ( T v) { 	x  /= v;		y  /= v;		z  /= v;		w  /= v;		return *this;			};

// Functions ----------------------------------------
template<class T> inline Vec4<T> Vec4<T>::Min( 	const Vec4<T>& a, const Vec4<T>& b) { 	Vec4<T> ret; 	ret.x = min(a.x, b.x); 	ret.y = min(a.y, b.y); 	ret.z = min(a.z, b.z); 	ret.w = min(a.w, b.w); 	return ret; };
template<class T> inline Vec4<T> Vec4<T>::Max( 	const Vec4<T>& a, const Vec4<T>& b) { 	Vec4<T> ret; 	ret.x = max(a.x, b.x); 	ret.y = max(a.y, b.y); 	ret.z = max(a.z, b.z); 	ret.w = max(a.w, b.w); 	return ret; };
template<class T> inline Vec4<T> Vec4<T>::Abs() const { Vec4<T> ret; ret.x = abs(x); ret.y = abs(y); ret.z = abs(z); ret.w = abs(w); return ret; }

typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;
typedef Vec4<int> Vec4i;


#endif
