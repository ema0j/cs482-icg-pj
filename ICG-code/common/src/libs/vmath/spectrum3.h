#ifndef _SPECTRUM3T_H_
#define _SPECTRUM3T_H_

#include "tuple3.h"

template<class T>
class Spectrum3 : public Tuple3<T> {
public:
	// Constructors -------------------------------------
    Spectrum3() : Tuple3<T>(0,0,0) { }
    Spectrum3(T x, T y, T z) : Tuple3<T>(x,y,z) { }

    static Spectrum3<T> One() { return Spectrum3<T>(1,1,1); }
    static Spectrum3<T> Zero() { return Spectrum3<T>(0,0,0); }

    T ToLuminance() const { return ToLuminance(this->x,this->y,this->z); }
    static T ToLuminance(T r, T g, T b) { return T(r * 0.212671 + g * 0.715160 + b * 0.072169); }

	// Binary ops ---------------------------------------
	Spectrum3 operator+(const Spectrum3& v) const;
	Spectrum3 operator-(const Spectrum3& v) const;
	Spectrum3 operator*(const Spectrum3& v) const;
	Spectrum3 operator/(const Spectrum3& v) const;
	Spectrum3 operator*(T v) const;
	Spectrum3 operator/(T v) const;

	// Assignment ops -----------------------------------
	Spectrum3& operator+=(const Spectrum3& v);
	Spectrum3& operator*=(const Spectrum3& v);
	Spectrum3& operator*=(T v);
	Spectrum3& operator/=(T v);
};

template<class T>
class SpectrumAlpha3 {
	// data ---------------------------------------------
public:
    Spectrum3<T> s;
    T a;

public:
	// Constructors -------------------------------------
    SpectrumAlpha3() : s(), a(0) { }
    SpectrumAlpha3(const Spectrum3<T>& s, T a) : s(s), a(a) { }
};

// Binary ops ---------------------------------------
template<class T> inline Spectrum3<T> Spectrum3<T>::operator  + (const Spectrum3& v) const { 	Spectrum3<T> ret; 	ret.x = this->x  + v.x;		ret.y = this->y  + v.y;		ret.z = this->z  + v.z;		return ret;			};
template<class T> inline Spectrum3<T> Spectrum3<T>::operator  - (const Spectrum3& v) const { 	Spectrum3<T> ret; 	ret.x = this->x  - v.x;		ret.y = this->y  - v.y;		ret.z = this->z  - v.z;		return ret;			};
template<class T> inline Spectrum3<T> Spectrum3<T>::operator  * (const Spectrum3& v) const { 	Spectrum3<T> ret; 	ret.x = this->x  * v.x;		ret.y = this->y  * v.y;		ret.z = this->z  * v.z;		return ret;			};
template<class T> inline Spectrum3<T> Spectrum3<T>::operator  / (const Spectrum3& v) const { 	Spectrum3<T> ret; 	ret.x = this->x  / v.x;		ret.y = this->y  / v.y;		ret.z = this->z  / v.z;		return ret;			};
template<class T> inline Spectrum3<T> Spectrum3<T>::operator  * ( T v) const { 	Spectrum3<T> ret; 	ret.x = this->x  * v;		ret.y = this->y  * v;		ret.z = this->z  * v;		return ret;			};
template<class T> inline Spectrum3<T> Spectrum3<T>::operator  / ( T v) const { 	Spectrum3<T> ret; 	ret.x = this->x  / v;		ret.y = this->y  / v;		ret.z = this->z  / v;		return ret;			};

// Assignment ops -----------------------------------
template<class T> inline Spectrum3<T>& Spectrum3<T>::operator  += (const Spectrum3& v) { 	this->x  += v.x;		this->y  += v.y;		this->z  += v.z;		return *this;			};
template<class T> inline Spectrum3<T>& Spectrum3<T>::operator  *= (const Spectrum3& v) { 	this->x  *= v.x;		this->y  *= v.y;		this->z  *= v.z;		return *this;			};
template<class T> inline Spectrum3<T>& Spectrum3<T>::operator  *= ( T v) { 	this->x  *= v;		this->y  *= v;		this->z  *= v;		return *this;			};
template<class T> inline Spectrum3<T>& Spectrum3<T>::operator  /= ( T v) { 	this->x  /= v;		this->y  /= v;		this->z  /= v;		return *this;			};

// Types
typedef Spectrum3<float> Spectrum3f;
typedef Spectrum3<double> Spectrum3d;
typedef SpectrumAlpha3<float> SpectrumAlpha3f;
typedef SpectrumAlpha3<double> SpectrumAlpha3d;

#endif
