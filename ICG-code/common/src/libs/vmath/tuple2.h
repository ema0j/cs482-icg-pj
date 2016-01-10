#ifndef _TUPLE2T_H_
#define _TUPLE2T_H_

template<class T>
class Tuple2 {
	// data ---------------------------------------------
public:
	T x, y;

public:
	// Constructors -------------------------------------
    Tuple2() : x(0), y(0) { }
    Tuple2(T x, T y) : x(x), y(y) { }

    // Access ops ---------------------------------------
    const T& operator[](int i) const { return (&x)[i]; }
	T& operator[](int i)  { return (&x)[i]; }
    T Average() { return (x+y)/2; }

	// Comparison ops -----------------------------------
    bool IsZero() const { return x == 0 && y == 0; }
    bool operator==(const Tuple2& v) const { return x == v.x && y == v.y; }

	// Binary ops ---------------------------------------
	Tuple2 operator+(const Tuple2& v) const;
	Tuple2 operator-(const Tuple2& v) const;
	Tuple2 operator*(const Tuple2& v) const;
	Tuple2 operator/(const Tuple2& v) const;
	Tuple2 operator*(T v) const;
	Tuple2 operator/(T v) const;
};

// Binary ops ---------------------------------------
template<class T> inline Tuple2<T> Tuple2<T>::operator  + (const Tuple2& v) const { 	Tuple2<T> ret; 	ret.x = x  + v.x;		ret.y = y  + v.y;		return ret;			};
template<class T> inline Tuple2<T> Tuple2<T>::operator  - (const Tuple2& v) const { 	Tuple2<T> ret; 	ret.x = x  - v.x;		ret.y = y  - v.y;		return ret;			};
template<class T> inline Tuple2<T> Tuple2<T>::operator  * (const Tuple2& v) const { 	Tuple2<T> ret; 	ret.x = x  * v.x;		ret.y = y  * v.y;		return ret;			};
template<class T> inline Tuple2<T> Tuple2<T>::operator  / (const Tuple2& v) const { 	Tuple2<T> ret; 	ret.x = x  / v.x;		ret.y = y  / v.y;		return ret;			};
template<class T> inline Tuple2<T> Tuple2<T>::operator  * ( T v) const { 	Tuple2<T> ret; 	ret.x = x  * v;		ret.y = y  * v;		return ret;			};
template<class T> inline Tuple2<T> Tuple2<T>::operator  / ( T v) const { 	Tuple2<T> ret; 	ret.x = x  / v;		ret.y = y  / v;		return ret;			};

// Types
typedef Tuple2<int> Tuple2i;
typedef Tuple2<float> Tuple2f;
typedef Tuple2<double> Tuple2d;

#endif
