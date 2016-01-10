#ifndef _GEOM3T_H_
#define _GEOM3T_H_

#include "tuple3.h"
#include <misc/arrays.h>

template<class T> class Point3;
template<class T> class Vector3;
template<class T> class Direction3;
template<class T> class Normal3;

template<class T>
class Point3 : public Tuple3<T> {
public:
	// Constructors -------------------------------------
    Point3() : Tuple3<T>(0,0,0) { }
    Point3(T x, T y, T z) : Tuple3<T>(x,y,z) { }
    explicit Point3(const Tuple3<T>& v) : Tuple3<T>(v) { }

	// Binary ops ---------------------------------------
	Point3 operator+(const Vector3<T>& v) const;
	Vector3<T> operator-(const Point3& v) const;

	// Functions ----------------------------------------
    static Point3 Min(const Point3& a, const Point3& b);
    static Point3 Max(const Point3& a, const Point3& b);
    static Point3 AffineCombination(const Point3& p1, T w1, const Point3& p2, T w2);
    static Point3 AffineCombination(const Point3& p1, T w1, const Point3& p2, T w2, const Point3& p3, T w3);
};

template<class T>
class Vector3 : public Tuple3<T> {
public:
	// Constructors -------------------------------------
    Vector3() : Tuple3<T>(0,0,0) { }
    Vector3(T x, T y, T z) : Tuple3<T>(x,y,z) { }
    explicit Vector3(const Tuple3<T>& v) : Tuple3<T>(v) { }

    static Vector3 X() { return Vector3(1,0,0); }
    static Vector3 Y() { return Vector3(0,1,0); }
    static Vector3 Z() { return Vector3(0,0,1); }

	// Unary ops ----------------------------------------
    Vector3 operator-() const;

	// Binary ops ---------------------------------------
	Vector3 operator+(const Vector3& v) const;
	Vector3 operator-(const Vector3& v) const;
	Vector3 operator*(T v) const;
	Vector3 operator/(T v) const;

	// Vector ops ---------------------------------------
	T operator %(const Vector3& v) const; // dot
	Vector3 operator ^(const Vector3& v) const; // cross
    void GetXYFromZ(Vector3& outX, Vector3& outY) const;
	T GetLength() const;
	T GetLengthSqr() const;
	Direction3<T> GetNormalized() const;
};

template<class T>
class Direction3 : public Vector3<T> {
public:
	// Constructors -------------------------------------
    Direction3() : Vector3<T>(0,0,1) { }
    Direction3(T x, T y, T z) : Vector3<T>(x,y,z) { }
    explicit Direction3(const Tuple3<T>& v) : Vector3<T>(v) {  }

    static Direction3 FromNormalizedVector(const Vector3<T>& v) { return Direction3(v.x,v.y,v.z); }

    static Direction3 X() { return Direction3(1,0,0); }
    static Direction3 Y() { return Direction3(0,1,0); }
    static Direction3 Z() { return Direction3(0,0,1); }

	// Unary ops ----------------------------------------
    Direction3 operator-() const;
};

template<class T>
class Normal3 : public Direction3<T> {
public:
	// Constructors -------------------------------------
    Normal3() : Direction3<T>() { }
    Normal3(T x, T y, T z) : Direction3<T>(x,y,z) { }
    explicit Normal3(const Direction3<T>& v) : Direction3<T>(v) { }
    explicit Normal3(const Tuple3<T>& v) : Direction3<T>(v) {  }

	// Unary ops ----------------------------------------
    Normal3 operator-() const;
};

// Utils ----------------------------------------
class Geom3Utils {
public:
    template<typename T, typename TT> 
    static carray<T> ConvertArray(const carray<TT>& a) {
        carray<T> ret(a.size());
        for(int i = 0; i < a.size(); i ++) ret[i] = T(a[i]);
        return ret;
    }
};

// Binary ops ---------------------------------------
template<class T> inline Point3<T> Point3<T>::operator  + (const Vector3<T>& v) const { 	Point3<T> ret; 	ret.x = this->x  + v.x;		ret.y = this->y  + v.y;		ret.z = this->z  + v.z;		return ret;			};
template<class T> inline Vector3<T> Point3<T>::operator  - (const Point3& v) const { 	Vector3<T> ret; 	ret.x = this->x  - v.x;		ret.y = this->y  - v.y;		ret.z = this->z  - v.z;		return ret;			};

// Vector ops ---------------------------------------
template<class T>
inline T Vector3<T>::operator %(const Vector3& v) const {
	return this->x * v.x + this->y * v.y + this->z * v.z;
}
template<class T>
inline Vector3<T> Vector3<T>::operator ^(const Vector3& v) const {
	return Vector3(this->y * v.z - this->z * v.y,
				this->z * v.x - this->x * v.z,
				this->x * v.y - this->y * v.x);
}
template<class T>
inline void Vector3<T>::GetXYFromZ(Vector3& outX, Vector3& outY) const {
	outX = Vector3<T>(1,0,0);
	outX = operator ^(outX);
	if(outX.GetLength() == 0) {
		outX = (operator ^(Vector3<T>(0,1,0))).GetNormalized();
	} else {
		outX = (operator ^(outX)).GetNormalized();
	}
	outY = operator ^(outX);
}
template<class T>
inline T Vector3<T>::GetLength() const {
	return (T)sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
}
template<class T>
inline T Vector3<T>::GetLengthSqr() const {
	return this->x*this->x+this->y*this->y+this->z*this->z;
}
template<class T>
inline Direction3<T> Vector3<T>::GetNormalized() const {
	T l = GetLength();
	if(l > 0) {
		return Direction3<T>(this->x/l,this->y/l,this->z/l);
	} else {
		return Direction3<T>(0,0,0); // BUG: this is invalid, should we devide by 0?
	}
}
/*
template<class T>
inline void Vector3<T>::Normalize() {
	T l = GetLength();
	operator /= (l);
}
*/

template<class T> inline Point3<T> Point3<T>::Min( 	const Point3<T>& a, const Point3<T>& b) { 	Point3 ret; 	ret.x = min(a.x, b.x); 	ret.y = min(a.y, b.y); 	ret.z = min(a.z, b.z); 	return ret; };
template<class T> inline Point3<T> Point3<T>::Max( 	const Point3<T>& a, const Point3<T>& b) { 	Point3 ret; 	ret.x = max(a.x, b.x); 	ret.y = max(a.y, b.y); 	ret.z = max(a.z, b.z); 	return ret; };
template<class T> inline Point3<T> Point3<T>::AffineCombination(const Point3<T>& p1, T w1, const Point3<T>& p2, T w2) {
    // HACK: write more efficiently
    return Point3<T>() + ((p1-Point3<T>())*w1 + (p2-Point3<T>())*w2);
}
template<class T> inline Point3<T> Point3<T>::AffineCombination(const Point3<T>& p1, T w1, const Point3<T>& p2, T w2, const Point3<T>& p3, T w3) { 
    // HACK: write more efficiently
    return Point3<T>() + ((p1-Point3<T>())*w1 + (p2-Point3<T>())*w2 + (p3-Point3<T>())*w3);
}

// Unary ops ----------------------------------------
template<class T> inline Vector3<T> Vector3<T>::operator  - () const { 	Vector3<T> ret; 	ret.x =  - (this->x); 	ret.y =  - (this->y); 	ret.z =  - (this->z); 	return ret; };

// Binary ops ---------------------------------------
template<class T> inline Vector3<T> Vector3<T>::operator  + (const Vector3& v) const { 	Vector3<T> ret; 	ret.x = this->x  + v.x;		ret.y = this->y  + v.y;		ret.z = this->z  + v.z;		return ret;			};
template<class T> inline Vector3<T> Vector3<T>::operator  - (const Vector3& v) const { 	Vector3<T> ret; 	ret.x = this->x  - v.x;		ret.y = this->y  - v.y;		ret.z = this->z  - v.z;		return ret;			};
template<class T> inline Vector3<T> Vector3<T>::operator  * ( T v) const { 	Vector3<T> ret; 	ret.x = this->x  * v;		ret.y = this->y  * v;		ret.z = this->z  * v;		return ret;			};
template<class T> inline Vector3<T> Vector3<T>::operator  / ( T v) const { 	Vector3<T> ret; 	ret.x = this->x  / v;		ret.y = this->y  / v;		ret.z = this->z  / v;		return ret;			};

// Unary ops ----------------------------------------
template<class T> inline Direction3<T> Direction3<T>::operator  - () const { 	Direction3<T> ret; 	ret.x =  - (this->x); 	ret.y =  - (this->y); 	ret.z =  - (this->z); 	return ret; };

// Types
typedef Tuple3<float> Tuple3f;
typedef Tuple3<double> Tuple3d;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
typedef Point3<float> Point3f;
typedef Point3<double> Point3d;
typedef Normal3<float> Normal3f;
typedef Normal3<double> Normal3d;
typedef Direction3<float> Direction3f;
typedef Direction3<double> Direction3d;

#endif
