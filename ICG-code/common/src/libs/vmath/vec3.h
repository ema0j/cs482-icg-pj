#ifndef _VEC3T_H_
#define _VEC3T_H_

// INCLUDES ====================================================
#include "functions.h"
#include <assert.h>

template<class T>
class Vec3 {
	// data ---------------------------------------------
public:
	T x, y, z;

public:
	// Constructors -------------------------------------
	Vec3();
	Vec3(T v);
	Vec3(T x, T y, T z);
	Vec3(const Vec3<T>& v);
	template <class TT>
	Vec3(const Vec3<TT>& v);

	static Vec3 Zero();
	static Vec3 One();
	static Vec3 X();
	static Vec3 Y();
	static Vec3 Z();
    static Vec3 Infinity();

	// Conversions --------------------------------------
	operator const T* () const;
	operator T* ();
	template<class TT>
	operator Vec3<TT>() const;

	// Access ops ---------------------------------------
	const T& operator[](int i) const;
	T& operator[](int i);
    T MaxComponent() const;
    T MinComponent() const;
    uint8_t MaxComponentIndex() const;
    uint8_t MinComponentIndex() const;
    T Average() const;
    static uint8_t NumComponents();

	// Comparison ops -----------------------------------
	bool operator == (const Vec3& v) const;
	bool operator != (const Vec3& v) const;
	bool operator < (const Vec3& v) const; // For-all-component
	bool operator > (const Vec3& v) const; // For-all-component
	bool operator <= (const Vec3& v) const; // For-all-component
	bool operator >= (const Vec3& v) const; // For-all-component
    bool IsZero() const;
	bool HasNeg() const;

	// Unary ops ----------------------------------------
    Vec3 operator-() const;

	// Binary ops ---------------------------------------
	Vec3 operator+(const Vec3& v) const;
	Vec3 operator-(const Vec3& v) const;
	Vec3 operator*(const Vec3& v) const;
	Vec3 operator/(const Vec3& v) const;
	Vec3 operator|(const Vec3& v) const;
	Vec3 operator+(T v) const;
	Vec3 operator-(T v) const;
	Vec3 operator*(T v) const;
	Vec3 operator/(T v) const;
	Vec3 operator|(T v) const;

	// Assignment ops -----------------------------------
	Vec3& operator=(const Vec3& v);
	Vec3& operator=(T v);
	Vec3& operator+=(const Vec3& v);
	Vec3& operator-=(const Vec3& v);
	Vec3& operator*=(const Vec3& v);
	Vec3& operator/=(const Vec3& v);
	Vec3& operator|=(const Vec3& v);
	Vec3& operator+=(T v);
	Vec3& operator-=(T v);
	Vec3& operator*=(T v);
	Vec3& operator/=(T v);
	Vec3& operator|=(T v);

	// Vector ops ---------------------------------------
	T operator %(const Vec3& v) const; // dot
	Vec3 operator ^(const Vec3& v) const; // cross
    void GetXYFromZ(Vec3& outX, Vec3& outY) const;
	T GetLength() const;
	T GetLengthSqr() const;
	Vec3 GetNormalized() const;
	void Normalize();
	T Luminance() const;

	// Functions ----------------------------------------
	Vec3 Pow(T e) const;
	Vec3 Log(T b) const;
	Vec3 Abs() const;
	Vec3 Sqrt() const;
	Vec3 Sign() const;
    Vec3 Clamp(const Vec3& a, const Vec3& b) const;
	Vec3 Clamp01() const;
    Vec3 ClampMin(const Vec3& a) const;

	// Functions ----------------------------------------
    static Vec3 Min(const Vec3& a, const Vec3& b);
    static Vec3 Max(const Vec3& a, const Vec3& b);
};

// Constructors -------------------------------------
template<class T>
inline Vec3<T>::Vec3() {
	x = y = z = 0;
}
template<class T>
inline Vec3<T>::Vec3(T v) {
	x = y = z = v;
}
template<class T>
inline Vec3<T>::Vec3(T x, T y, T z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
template<class T>
inline Vec3<T>::Vec3(const Vec3& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}
template<class T> template<class TT>
inline Vec3<T>::Vec3(const Vec3<TT>& v) {
	x = (T)v.x;
	y = (T)v.y;
	z = (T)v.z;
}
template<class T>
inline Vec3<T> Vec3<T>::Zero() {
	Vec3<T> ret;
	ret.x = 0;
	ret.y = 0;
	ret.z = 0;
	return ret;
}
template<class T>
inline Vec3<T> Vec3<T>::One() {
	Vec3<T> ret;
	ret.x = 1;
	ret.y = 1;
	ret.z = 1;
	return ret;
}
template<class T>
inline Vec3<T> Vec3<T>::X() {
	Vec3<T> ret;
	ret.x = 1;
	ret.y = 0;
	ret.z = 0;
	return ret;
}
template<class T>
inline Vec3<T> Vec3<T>::Y() {
	Vec3<T> ret;
	ret.x = 0;
	ret.y = 1;
	ret.z = 0;
	return ret;
}
template<class T>
inline Vec3<T> Vec3<T>::Z() {
	Vec3 ret;
	ret.x = 0;
	ret.y = 0;
	ret.z = 1;
	return ret;
}
template<class T>
inline Vec3<T> Vec3<T>::Infinity() {
	Vec3<T> ret;
    ret.x = std::numeric_limits<T>::infinity();
	ret.y = std::numeric_limits<T>::infinity();
	ret.z = std::numeric_limits<T>::infinity();
	return ret;
}

// Conversions --------------------------------------
template<class T>
inline Vec3<T>::operator const T* () const {
	return &x;
}
template<class T>
inline Vec3<T>::operator T* () {
	return &x;
}
template<class T> template<class TT>
inline Vec3<T>::operator Vec3<TT> () const {
	return Vec3<TT>(*this);
}

// Access ops ---------------------------------------
template<class T>
inline const T& Vec3<T>::operator[](int i) const {
	return (&x)[i];
}
template<class T>
inline T& Vec3<T>::operator[](int i) {
	return (&x)[i];
}
template<class T>
inline T Vec3<T>::MaxComponent() const {
	return max(x, max(y,z));
}
template<class T>
inline T Vec3<T>::MinComponent() const {
	return min(x, min(y,z));
}
template<class T>
inline uint8_t Vec3<T>::MaxComponentIndex() const {
    if(x >= y && x >= z) return 0;
    if(y >= x && y >= z) return 1;
    if(z >= x && z >= y) return 2;
    return -1; // error
}
template<class T>
inline uint8_t Vec3<T>::MinComponentIndex() const {
    if(x <= y && x <= z) return 0;
    if(y <= x && y <= z) return 1;
    if(z <= x && z <= y) return 2;
    return -1; // error
}
template<class T>
inline T Vec3<T>::Average() const {
    return (x + y + z) / 3;
}
template<class T>
inline uint8_t Vec3<T>::NumComponents() {
    return 3;
}

template<class T>
inline T Vec3<T>::Luminance() const
{
    const T YWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
    return YWeight[0] * x + YWeight[1] * y + YWeight[2] * z;
}

// Vector ops ---------------------------------------
template<class T>
inline T Vec3<T>::operator %(const Vec3& v) const {
	return x * v.x + y * v.y + z * v.z;
}
template<class T>
inline Vec3<T> Vec3<T>::operator ^(const Vec3& v) const {
	return Vec3(y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x);
}
template<class T>
inline void Vec3<T>::GetXYFromZ(Vec3& outX, Vec3& outY) const {
	outX = X();
	outX = operator ^(outX);
	if(outX.GetLength() == 0) {
		outX = (operator ^(Y())).GetNormalized();
	} else {
		outX = (operator ^(outX)).GetNormalized();
	}
	outY = operator ^(outX);
}
template<class T>
inline T Vec3<T>::GetLength() const {
	return (T)sqrt(x*x+y*y+z*z);
}
template<class T>
inline T Vec3<T>::GetLengthSqr() const {
	return x*x+y*y+z*z;
}
template<class T>
inline Vec3<T> Vec3<T>::GetNormalized() const {
	T l = GetLength();
	if(l > 0) {
		return operator / (l);
	} else {
		return *this;
	}
}
template<class T>
inline void Vec3<T>::Normalize() {
	T l = GetLength();
	operator /= (l);
}

// Comparison ops -----------------------------------
template<class T> inline bool Vec3<T>::operator  == (const Vec3<T>& v) const { 	return 		x  == v.x && 		y  == v.y && 		z  == v.z; };
template<class T> inline bool Vec3<T>::operator  != (const Vec3<T>& v) const { 	return 		x  != v.x || 		y  != v.y || 		z  != v.z; };
template<class T> inline bool Vec3<T>::operator  < (const Vec3<T>& v) const { 	return 		x  < v.x && 		y  < v.y && 		z  < v.z; };
template<class T> inline bool Vec3<T>::operator  > (const Vec3<T>& v) const { 	return 		x  > v.x && 		y  > v.y && 		z  > v.z; };
template<class T> inline bool Vec3<T>::operator  <= (const Vec3<T>& v) const { 	return 		x  <= v.x && 		y  <= v.y && 		z  <= v.z; };
template<class T> inline bool Vec3<T>::operator  >= (const Vec3<T>& v) const { 	return 		x  >= v.x && 		y  >= v.y && 		z  >= v.z; };
template<class T> inline bool Vec3<T>::IsZero() const { 	return 		x  == 0 && 		y  == 0 && 		z  == 0; };
template<class T> inline bool Vec3<T>::HasNeg() const { 	return 		x  <  0 || 		y  < 0  && 		z  <  0; };

// Unary ops ----------------------------------------
template<class T> inline Vec3<T> Vec3<T>::operator  - () const { 	Vec3<T> ret; 	ret.x =  - (x); 	ret.y =  - (y); 	ret.z =  - (z); 	return ret; };

// Binary ops ---------------------------------------
template<class T> inline Vec3<T> Vec3<T>::operator  + (const Vec3& v) const { 	Vec3<T> ret; 	ret.x = x  + v.x;		ret.y = y  + v.y;		ret.z = z  + v.z;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  - (const Vec3& v) const { 	Vec3<T> ret; 	ret.x = x  - v.x;		ret.y = y  - v.y;		ret.z = z  - v.z;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  * (const Vec3& v) const { 	Vec3<T> ret; 	ret.x = x  * v.x;		ret.y = y  * v.y;		ret.z = z  * v.z;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  / (const Vec3& v) const { 	Vec3<T> ret; 	ret.x = x  / v.x;		ret.y = y  / v.y;		ret.z = z  / v.z;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  | (const Vec3& v) const 
{ 	
	Vec3 ret; 	
	ret.x = (v.x == 0) ? 0 : (x  / v.x);
	ret.y = (v.y == 0) ? 0 : (y  / v.y);
	ret.z = (v.z == 0) ? 0 : (z  / v.z);
	return ret;			
};
template<class T> inline Vec3<T> Vec3<T>::operator  + ( T v) const { 	Vec3<T> ret; 	ret.x = x  + v;		ret.y = y  + v;		ret.z = z  + v;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  - ( T v) const { 	Vec3<T> ret; 	ret.x = x  - v;		ret.y = y  - v;		ret.z = z  - v;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  * ( T v) const { 	Vec3<T> ret; 	ret.x = x  * v;		ret.y = y  * v;		ret.z = z  * v;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  / ( T v) const { 	Vec3<T> ret; 	ret.x = x  / v;		ret.y = y  / v;		ret.z = z  / v;		return ret;			};
template<class T> inline Vec3<T> Vec3<T>::operator  | ( T v) const 
{
	Vec3 ret; 	
	ret.x = (v == 0) ? 0 : (x  / v);		
	ret.y = (v == 0) ? 0 : (y  / v);
	ret.z = (v == 0) ? 0 : (z  / v);
	return ret;			
};


// Assignment ops -----------------------------------
template<class T> inline Vec3<T> & Vec3<T>::operator  = (const Vec3<T>& v) { 	x  = v.x;		y  = v.y;		z  = v.z;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  += (const Vec3<T>& v) { 	x  += v.x;		y  += v.y;		z  += v.z;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  -= (const Vec3<T>& v) { 	x  -= v.x;		y  -= v.y;		z  -= v.z;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  *= (const Vec3<T>& v) { 	x  *= v.x;		y  *= v.y;		z  *= v.z;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  /= (const Vec3<T>& v) { 	x  /= v.x;		y  /= v.y;		z  /= v.z;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  |= (const Vec3& v) 
{ 	
	(v.x == 0) ? x = 0 : x  /= v.x;		
	(v.y == 0) ? y = 0 : y  /= v.y;	
	(v.z == 0) ? z = 0 : z  /= v.z;	
	return *this;			
};
template<class T> inline Vec3<T> & Vec3<T>::operator  = ( T v) { 	x  = v;		y  = v;		z  = v;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  += ( T v) { 	x  += v;		y  += v;		z  += v;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  -= ( T v) { 	x  -= v;		y  -= v;		z  -= v;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  *= ( T v) { 	x  *= v;		y  *= v;		z  *= v;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  /= ( T v) { 	x  /= v;		y  /= v;		z  /= v;		return *this;			};
template<class T> inline Vec3<T> & Vec3<T>::operator  |= ( T v) 
{ 	
	(v == 0) ? x = 0 : x  /= v;		
	(v == 0) ? y = 0 : y  /= v;	
	(v == 0) ? z = 0 : z  /= v;		
	return *this;			
};

// Functions ----------------------------------------
template<class T> inline Vec3<T> Vec3<T>::Pow( T e ) const { Vec3<T> ret; ret.x = pow(x,e); ret.y = pow(y,e); ret.z = pow(z,e); return ret; }
template<class T> inline Vec3<T> Vec3<T>::Log( T b ) const { Vec3<T> ret; ret.x = log(x) / log(b); ret.y = log(y) / log(b); ret.z = log(z) / log(b); return ret; }
template<class T> inline Vec3<T> Vec3<T>::Sqrt() const { Vec3<T> ret; ret.x = sqrt(x); ret.y = sqrt(y); ret.z = sqrt(z); return ret; }

template<class T> inline Vec3<T> Vec3<T>::Abs() const { Vec3<T> ret; ret.x = abs(x); ret.y = abs(y); ret.z = abs(z); return ret; }
template<> inline Vec3<float> Vec3<float>::Abs() const { Vec3<float> ret; ret.x = fabs(x); ret.y = fabs(y); ret.z = fabs(z); return ret; }
template<> inline Vec3<double> Vec3<double>::Abs() const { Vec3<double> ret; ret.x = fabs(x); ret.y = fabs(y); ret.z = fabs(z); return ret; }

template<class T> inline Vec3<T> Vec3<T>::Sign() const { Vec3<T> ret; ret.x = x >= 0 ? One().x : -One().x; ret.y = y >= 0 ? One().y : -One().y; ret.z = z >= 0 ? One().z : -One().z; return ret; };
template<class T> inline Vec3<T> Vec3<T>::Clamp(const Vec3<T>& a, const Vec3<T>& b) const { Vec3<T> ret; ret.x = clamp(x, a.x, b.x); 	ret.y = clamp(y, a.y, b.y); 	ret.z = clamp(z, a.z, b.z); return ret; };
template<class T> inline Vec3<T> Vec3<T>::Clamp01() const { Vec3<T> ret; ret.x = clamp(x, 0.0f, 1.0f); 	ret.y = clamp(y, 0.0f, 1.0f);	ret.z = clamp(z, 0.0f, 1.0f); return ret; };
template<class T> inline Vec3<T> Vec3<T>::ClampMin(const Vec3<T>& a) const { Vec3<T> ret; ret.x = clampmin(x, a.x); 	ret.y = clampmin(y, a.y); 	ret.z = clampmin(z, a.z); return ret; };
template<class T> inline Vec3<T> Vec3<T>::Min( 	const Vec3<T>& a, const Vec3<T>& b) { 	Vec3 ret; 	ret.x = min(a.x, b.x); 	ret.y = min(a.y, b.y); 	ret.z = min(a.z, b.z); 	return ret; };
template<class T> inline Vec3<T> Vec3<T>::Max( 	const Vec3<T>& a, const Vec3<T>& b) { 	Vec3 ret; 	ret.x = max(a.x, b.x); 	ret.y = max(a.y, b.y); 	ret.z = max(a.z, b.z); 	return ret; };

typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int> Vec3i;

#endif
