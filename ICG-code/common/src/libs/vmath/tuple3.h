#ifndef _TUPLE3T_H_
#define _TUPLE3T_H_

template<class T>
class Tuple3 {
	// data ---------------------------------------------
public:
	T x, y, z;

public:
	// Constructors -------------------------------------
    Tuple3() : x(0), y(0), z(0) { }
    Tuple3(T x, T y, T z) : x(x), y(y), z(z) { }

    // Access ops ---------------------------------------
    const T& operator[](int i) const { return (&x)[i]; }
	T& operator[](int i)  { return (&x)[i]; }
    T Average() { return (x+y+z)/3; }
    T MaxComponent() const { return max(x, max(y,z)); }

	// Comparison ops -----------------------------------
    bool IsZero() const { return x == 0 && y == 0 && z == 0; }
    bool operator==(const Tuple3& v) const { return x == v.x && y == v.y && z == v.z; }

	// Binary ops ---------------------------------------
	Tuple3 operator+(const Tuple3& v) const;
	Tuple3 operator-(const Tuple3& v) const;
	Tuple3 operator*(const Tuple3& v) const;
	Tuple3 operator/(const Tuple3& v) const;
	Tuple3 operator*(T v) const;
	Tuple3 operator/(T v) const;
};

// Binary ops ---------------------------------------
template<class T> inline Tuple3<T> Tuple3<T>::operator  + (const Tuple3& v) const { 	Tuple3<T> ret; 	ret.x = x  + v.x;		ret.y = y  + v.y;		ret.z = z  + v.z;		return ret;			};
template<class T> inline Tuple3<T> Tuple3<T>::operator  - (const Tuple3& v) const { 	Tuple3<T> ret; 	ret.x = x  - v.x;		ret.y = y  - v.y;		ret.z = z  - v.z;		return ret;			};
template<class T> inline Tuple3<T> Tuple3<T>::operator  * (const Tuple3& v) const { 	Tuple3<T> ret; 	ret.x = x  * v.x;		ret.y = y  * v.y;		ret.z = z  * v.z;		return ret;			};
template<class T> inline Tuple3<T> Tuple3<T>::operator  / (const Tuple3& v) const { 	Tuple3<T> ret; 	ret.x = x  / v.x;		ret.y = y  / v.y;		ret.z = z  / v.z;		return ret;			};
template<class T> inline Tuple3<T> Tuple3<T>::operator  * ( T v) const { 	Tuple3<T> ret; 	ret.x = x  * v;		ret.y = y  * v;		ret.z = z  * v;		return ret;			};
template<class T> inline Tuple3<T> Tuple3<T>::operator  / ( T v) const { 	Tuple3<T> ret; 	ret.x = x  / v;		ret.y = y  / v;		ret.z = z  / v;		return ret;			};

// Types
typedef Tuple3<int> Tuple3i;
typedef Tuple3<float> Tuple3f;
typedef Tuple3<double> Tuple3d;

#endif
