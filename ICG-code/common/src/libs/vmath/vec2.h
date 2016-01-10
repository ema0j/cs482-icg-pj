#ifndef _VEC2T_H_
#define _VEC2T_H_

// INCLUDES ====================================================
#include "functions.h"

// TYPE DECLARATION ============================================
template<class T>
class Vec2 {
    // data ---------------------------------------------
public:
    T x, y;

public:
	// Constructors -------------------------------------
	Vec2();
	Vec2(T v);
	Vec2(T x, T y);
	Vec2(const Vec2& v);
	template<class TT>
	Vec2(const Vec2<TT>& v);

	static Vec2 Zero();
	static Vec2 One();
	static Vec2 X();
	static Vec2 Y();

	// Conversions --------------------------------------
	operator const T* () const;
	operator T* ();
	template<class TT>
	operator Vec2<TT>() const;

	// Access ops ---------------------------------------
	const T& operator[](int i) const;
	T& operator[](int i);
    static int8_t NumComponents();

	// Comparison ops -----------------------------------
	bool operator == (const Vec2& v) const;
	bool operator != (const Vec2& v) const;
	bool operator < (const Vec2& v) const;
	bool operator > (const Vec2& v) const;
	bool operator <= (const Vec2& v) const;
	bool operator >= (const Vec2& v) const;
    bool IsZero() const;

	// Unary ops ----------------------------------------
    Vec2 operator-() const;

	// Binary ops ---------------------------------------
	Vec2 operator+(const Vec2& v) const;
	Vec2 operator-(const Vec2& v) const;
	Vec2 operator*(const Vec2& v) const;
	Vec2 operator/(const Vec2& v) const;
	Vec2 operator |(const Vec2& v) const; // none zero devide
	Vec2 operator+(T v) const;
	Vec2 operator-(T v) const;
	Vec2 operator*(T v) const;
	Vec2 operator/(T v) const;
	Vec2 operator |(T v) const; // none zero devide

	// Assignment ops -----------------------------------
	Vec2& operator=(const Vec2& v);
	Vec2& operator=(T v);
	Vec2& operator+=(const Vec2& v);
	Vec2& operator-=(const Vec2& v);
	Vec2& operator*=(const Vec2& v);
	Vec2& operator/=(const Vec2& v);
	Vec2& operator|=(const Vec2& v);
	Vec2& operator+=(T v);
	Vec2& operator-=(T v);
	Vec2& operator*=(T v);
	Vec2& operator/=(T v);
	Vec2& operator|=(T v);

	// Vector ops ---------------------------------------
	T operator %(const Vec2& v) const; // dot
	T GetLength() const;
	T GetLengthSqr() const;
	Vec2 GetNormalized() const;
	void Normalize();

	// Functions ----------------------------------------
    static Vec2 Min(const Vec2& a, const Vec2& b);
    static Vec2 Max(const Vec2& a, const Vec2& b);
};

// Constructors -------------------------------------
template<class T>
inline Vec2<T>::Vec2() {
	x = y = 0;
}
template<class T>
inline Vec2<T>::Vec2(T v) {
	x = y = v;
}
template<class T>
inline Vec2<T>::Vec2(T x, T y) {
	this->x = x;
	this->y = y;
}
template<class T>
inline Vec2<T>::Vec2(const Vec2<T>& v) {
	x = v.x;
	y = v.y;
}
template<class T> template<class TT>
inline Vec2<T>::Vec2(const Vec2<TT>& v) {
	x = (T)v.x;
	y = (T)v.y;
}
template<class T>
inline Vec2<T> Vec2<T>::Zero() {
	Vec2<T> ret;
	ret.x = 0;
	ret.y = 0;
	return ret;
}
template<class T>
inline Vec2<T> Vec2<T>::One() {
	Vec2<T> ret;
	ret.x = 1;
	ret.y = 1;
	return ret;
}
template<class T>
inline Vec2<T> Vec2<T>::X() {
	Vec2<T> ret;
	ret.x = 1;
	ret.y = 0;
	return ret;
}
template<class T>
inline Vec2<T> Vec2<T>::Y() {
	Vec2<T> ret;
	ret.x = 0;
	ret.y = 1;
	return ret;
}

// Conversions --------------------------------------
template<class T>
inline Vec2<T>::operator const T* () const {
	return &x;
}
template<class T>
inline Vec2<T>::operator T* () {
	return &x;
}
template<class T> template<class TT>
inline Vec2<T>::operator Vec2<TT> () const {
	return otype1(*this);
}

// Access ops ---------------------------------------
template<class T>
inline const T& Vec2<T>::operator[](int i) const {
	return (&x)[i];
}
template<class T>
inline T& Vec2<T>::operator[](int i) {
	return (&x)[i];
}
template<class T>
inline int8_t Vec2<T>::NumComponents() {
    return 2;
}

// Vector ops ---------------------------------------
template<class T>
inline T Vec2<T>::operator %(const Vec2<T>& v) const {
	return x * v.x + y * v.y;
}
template<class T>
inline T Vec2<T>::GetLength() const {
	return (T)sqrt(x*x+y*y);
}
template<class T>
inline T Vec2<T>::GetLengthSqr() const {
	return x*x+y*y;
}
template<class T>
inline Vec2<T> Vec2<T>::GetNormalized() const {
	T l = GetLength();
	if(l > 0) {
		return operator / (l);
	} else {
		return *this;
	}
}
template<class T>
inline void Vec2<T>::Normalize() {
	T l = GetLength();
	operator /= (l);
}

// Comparison ops -----------------------------------
template<class T> inline bool Vec2<T>::operator  == (const Vec2<T>& v) const { 	return 		x  == v.x && 		y  == v.y; };
template<class T> inline bool Vec2<T>::operator  != (const Vec2<T>& v) const { 	return 		x  != v.x || 		y  != v.y; };
template<class T> inline bool Vec2<T>::operator  < (const Vec2<T>& v) const { 	return 		x  < v.x && 		y  < v.y; };
template<class T> inline bool Vec2<T>::operator  > (const Vec2<T>& v) const { 	return 		x  > v.x && 		y  > v.y; };
template<class T> inline bool Vec2<T>::operator  <= (const Vec2<T>& v) const { 	return 		x  <= v.x && 		y  <= v.y; };
template<class T> inline bool Vec2<T>::operator  >= (const Vec2<T>& v) const { 	return 		x  >= v.x && 		y  >= v.y; };
template<class T> inline bool Vec2<T>::IsZero () const { 	return 		x  == 0 && 		y  == 0; };

// Unary ops ----------------------------------------
template<class T> inline Vec2<T> Vec2<T>::operator  - () const { 	Vec2<T> ret; 	ret.x =  - (x); 	ret.y =  - (y); return ret; };

// Binary ops ---------------------------------------
template<class T> inline Vec2<T> Vec2<T>::operator  + (const Vec2<T>& v) const { 	Vec2<T> ret; 	ret.x = x  + v.x;		ret.y = y  + v.y;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  - (const Vec2<T>& v) const { 	Vec2<T> ret; 	ret.x = x  - v.x;		ret.y = y  - v.y;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  * (const Vec2<T>& v) const { 	Vec2<T> ret; 	ret.x = x  * v.x;		ret.y = y  * v.y;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  / (const Vec2<T>& v) const { 	Vec2<T> ret; 	ret.x = x  / v.x;		ret.y = y  / v.y;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  | (const Vec2<T>& v) const 
{ 	
	Vec2<T> ret; 	
	ret.x = (v.x == 0) ? 0 : (x  / v.x);
	ret.y = (v.y == 0) ? 0 : (y  / v.y);
	return ret;			
};
template<class T> inline Vec2<T> Vec2<T>::operator  + ( T v) const { 	Vec2<T> ret; 	ret.x = x  + v;		ret.y = y  + v;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  - ( T v) const { 	Vec2<T> ret; 	ret.x = x  - v;		ret.y = y  - v;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  * ( T v) const { 	Vec2<T> ret; 	ret.x = x  * v;		ret.y = y  * v;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  / ( T v) const { 	Vec2<T> ret; 	ret.x = x  / v;		ret.y = y  / v;		return ret;			};
template<class T> inline Vec2<T> Vec2<T>::operator  | ( T v) const 
{
	Vec2<T> ret; 	
	ret.x = (v == 0) ? 0 : (x  / v);		
	ret.y = (v == 0) ? 0 : (y  / v);		
	return ret;			
};

// Assignment ops -----------------------------------
template<class T> inline Vec2<T> & Vec2<T>::operator  = (const Vec2<T>& v) { 	x  = v.x;		y  = v.y;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  += (const Vec2<T>& v) { 	x  += v.x;		y  += v.y;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  -= (const Vec2<T>& v) { 	x  -= v.x;		y  -= v.y;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  *= (const Vec2<T>& v) { 	x  *= v.x;		y  *= v.y;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  /= (const Vec2<T>& v) { 	x  /= v.x;		y  /= v.y;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  |= (const Vec2<T>& v) 
{ 	
	(v.x == 0) ? x = 0 : x  /= v.x;		
	(v.y == 0) ? y = 0 : y  /= v.y;		
	return *this;			
};
template<class T> inline Vec2<T> & Vec2<T>::operator  = ( T v) { 	x  = v;		y  = v;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  += ( T v) { 	x  += v;		y  += v;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  -= ( T v) { 	x  -= v;		y  -= v;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  *= ( T v) { 	x  *= v;		y  *= v;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  /= ( T v) { 	x  /= v;		y  /= v;		return *this;			};
template<class T> inline Vec2<T> & Vec2<T>::operator  |= ( T v) 
{ 	
	(v == 0) ? x = 0 : x  /= v;		
	(v == 0) ? y = 0 : y  /= v;		
	return *this;			
};

// Functions ----------------------------------------
template<class T> inline Vec2<T> Vec2<T>::Min( 	const Vec2<T>& a, const Vec2<T>& b) { 	Vec2<T> ret; 	ret.x = min(a.x, b.x); 	ret.y = min(a.y, b.y); 	return ret; };
template<class T> inline Vec2<T> Vec2<T>::Max( 	const Vec2<T>& a, const Vec2<T>& b) { 	Vec2<T> ret; 	ret.x = max(a.x, b.x); 	ret.y = max(a.y, b.y); 	return ret; };

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int32_t> Vec2i;
typedef Vec2<int64_t> Vec2l;
typedef Vec2<uint32_t> Vec2ui;
typedef Vec2<uint64_t> Vec2ul;
#endif
