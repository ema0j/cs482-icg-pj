#ifndef _VEC6T_H_
#define _VEC6T_H_

// INCLUDES ====================================================
#include "functions.h"
#include "vec3.h"

template<class _T>
class Vec6 {
    // data ---------------------------------------------
public:
    _T x, y, z, w, s, t;

public:
    // Constructors -------------------------------------
    Vec6();
    Vec6(_T v);
    Vec6(_T x, _T y = 0, _T z = 0, _T w = 0, _T s = 0, _T t = 0);
    Vec6(const Vec6& v);
    template<class TT>
    Vec6(const Vec6<TT>& v);
    Vec6(const Vec3<_T>& v, const Vec3<_T>& s);

    static Vec6 Zero();
    static Vec6 One();
    static Vec6 X();
    static Vec6 Y();
    static Vec6 Z();
    static Vec6 W();
    static Vec6 S();
    static Vec6 T();

    // Conversions --------------------------------------
    operator const _T* () const;
    operator _T* ();
    template<class TT>
    operator Vec6<TT>() const;

    // Access ops ---------------------------------------
    const _T& operator[](int i) const;
    _T& operator[](int i);
    _T MaxComponent() const;
    _T MinComponent() const;
    uint8_t MaxComponentIndex() const;
    uint8_t MinComponentIndex() const;
    static uint8_t NumComponents();

    // Comparison ops -----------------------------------
    bool operator == (const Vec6& v) const;
    bool operator != (const Vec6& v) const;
    bool operator < (const Vec6& v) const;
    bool operator > (const Vec6& v) const;
    bool operator <= (const Vec6& v) const;
    bool operator >= (const Vec6& v) const;
    bool IsZero() const;

    // Unary ops ----------------------------------------
    Vec6 operator-() const;

    // Binary ops ---------------------------------------
    Vec6 operator+(const Vec6& v) const;
    Vec6 operator-(const Vec6& v) const;
    Vec6 operator*(const Vec6& v) const;
    Vec6 operator/(const Vec6& v) const;
    Vec6 operator+(_T v) const;
    Vec6 operator-(_T v) const;
    Vec6 operator*(_T v) const;
    Vec6 operator/(_T v) const;

    // Assignment ops -----------------------------------
    Vec6& operator=(const Vec6& v);
    Vec6& operator=(_T v);
    Vec6& operator+=(const Vec6& v);
    Vec6& operator-=(const Vec6& v);
    Vec6& operator*=(const Vec6& v);
    Vec6& operator/=(const Vec6& v);
    Vec6& operator+=(_T v);
    Vec6& operator-=(_T v);
    Vec6& operator*=(_T v);
    Vec6& operator/=(_T v);

    // Vector ops ---------------------------------------
    _T operator %(const Vec6& v) const; // dot
    _T GetLength() const;
    _T GetLengthSqr() const;
    Vec6 GetNormalized() const;
    void Normalize();
    _T Average() const;

    // Functions ----------------------------------------
    static Vec6 Min(const Vec6& a, const Vec6& b);
    static Vec6 Max(const Vec6& a, const Vec6& b);
    Vec6        Clamp(const Vec6& a, const Vec6& b) const;
    Vec6        Abs() const;
    Vec3<_T>    GetXyz() const;
};

// Constructors -------------------------------------
template <class _T>
inline Vec6<_T>::Vec6() {
    x = y = z = w = s = t = 0;
}
template <class _T>
inline Vec6<_T>::Vec6(_T v) {
    x = y = z = w = s = t = v;
}
template <class _T>
inline Vec6<_T>::Vec6(_T x, _T y, _T z, _T w, _T s, _T t) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    this->s = s;
    this->t = t;
}
template <class _T>
inline Vec6<_T>::Vec6(const Vec6<_T>& v) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    s = v.s;
    t = v.t;
}
template <class _T>
inline Vec6<_T>::Vec6(const Vec3<_T>& v, const Vec3<_T>& vv) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = vv.x;
    s = vv.y;
    t = vv.z;
}
template <class _T> template<class _TT>
inline Vec6<_T>::Vec6(const Vec6<_TT>& v) {
    x = (_T)v.x;
    y = (_T)v.y;
    z = (_T)v.z;
    w = (_T)v.w;
    s = (_T)v.s;
    t = (_T)v.t;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::Zero() {
    Vec6<_T> ret;
    ret.x = 0;
    ret.y = 0;
    ret.z = 0;
    ret.w = 0;
    ret.s = 0;
    ret.t = 0;
    return ret;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::One() {
    Vec6<_T> ret;
    ret.x = 1;
    ret.y = 1;
    ret.z = 1;
    ret.w = 1;
    ret.s = 1;
    ret.t = 1;
    return ret;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::X() {
    Vec6<_T> ret;
    ret.x = 1;
    ret.y = 0;
    ret.z = 0;
    ret.w = 0;
    ret.s = 0;
    ret.t = 0;
    return ret;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::Y() {
    Vec6<_T> ret;
    ret.x = 0;
    ret.y = 1;
    ret.z = 0;
    ret.w = 0;
    ret.s = 0;
    ret.t = 0;
    return ret;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::Z() {
    Vec6<_T> ret;
    ret.x = 0;
    ret.y = 0;
    ret.z = 1;
    ret.w = 0;
    ret.s = 0;
    ret.t = 0;
    return ret;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::W() {
    Vec6<_T> ret;
    ret.x = 0;
    ret.y = 0;
    ret.z = 0;
    ret.w = 1;
    ret.s = 0;
    ret.t = 0;
    return ret;
}

template <class _T>
inline Vec6<_T> Vec6<_T>::S() {
    Vec6<_T> ret;
    ret.x = 0;
    ret.y = 0;
    ret.z = 0;
    ret.w = 0;
    ret.s = 1;
    ret.t = 0;
    return ret;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::T() {
    Vec6<_T> ret;
    ret.x = 0;
    ret.y = 0;
    ret.z = 0;
    ret.w = 0;
    ret.s = 0;
    ret.t = 1;
    return ret;
}

// Conversions --------------------------------------
template <class _T>
inline Vec6<_T>::operator const _T* () const {
    return &x;
}
template <class _T>
inline Vec6<_T>::operator _T* () {
    return &x;
}
template <class _T> template<class TT>
inline Vec6<_T>::operator Vec6<TT> () const {
    return Vec6<TT>(*this);
}

// Access ops ---------------------------------------
template <class _T>
inline const _T& Vec6<_T>::operator[](int i) const {
    return (&x)[i];
}
template <class _T>
inline _T& Vec6<_T>::operator[](int i) {
    return (&x)[i];
}
template<class T>
inline T Vec6<T>::MaxComponent() const {
    return max(max(x, max(y, z)), max(w, max(s, t)));
}
template<class T>
inline T Vec6<T>::MinComponent() const {
    return min(min(x, min(y,z)), min(w, min(s, t)));
}
template<class T>
inline uint8_t Vec6<T>::MaxComponentIndex() const {
    if(x >= y && x >= z && x >= w && x >= s && x >= t) return 0;
    if(y >= x && y >= z && y >= w && y >= s && y >= t) return 1;
    if(z >= x && z >= y && z >= w && z >= s && z >= t) return 2;
    if(w >= x && w >= y && w >= z && w >= s && w >= t) return 3;
    if(s >= x && s >= y && s >= z && s >= w && s >= t) return 4;
    if(t >= x && t >= y && t >= z && t >= w && t >= s) return 5;
    return -1; // error
}
template<class T>
inline uint8_t Vec6<T>::MinComponentIndex() const {
    if(x <= y && x <= z && x <= w && x <= s && x <= t) return 0;
    if(y <= x && y <= z && y <= w && y <= s && y <= t) return 1;
    if(z <= x && z <= y && z <= w && z <= s && z <= t) return 2;
    if(w <= x && w <= y && w <= z && w <= s && w <= t) return 3;
    if(s <= x && s <= y && s <= z && s <= w && s <= t) return 4;
    if(t <= x && t <= y && t <= z && t <= w && t <= s) return 5;
    return -1; // error
}
template<class _T>
inline uint8_t Vec6<_T>::NumComponents() {
    return 6;
}
template<class _T>
inline _T Vec6<_T>::Average() const {
    return (x + y + z + w + s + t) / 6;
}

// Vector ops ---------------------------------------
template <class _T>
inline _T Vec6<_T>::operator %(const Vec6<_T>& v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w + s * v.s + t * v.t;
}
template <class _T>
inline _T Vec6<_T>::GetLength() const {
    return (_T)sqrt(x*x+y*y+z*z+w*w+s*s+t*t);
}
template <class _T>
inline _T Vec6<_T>::GetLengthSqr() const {
    return x*x+y*y+z*z+w*w+s*s+t*t;
}
template <class _T>
inline Vec6<_T> Vec6<_T>::GetNormalized() const {
    _T l = GetLength();
    if(l > 0) {
        return operator / (l);
    } else {
        return *this;
    }
}
template <class _T>
inline void Vec6<_T>::Normalize() {
    _T l = GetLength();
    operator /= (l);
}
template <class _T>
inline Vec3<_T> Vec6<_T>::GetXyz() const {
    return Vec3<_T>(x, y, z);
}

// Comparison ops -----------------------------------
template <class _T> inline bool Vec6<_T>::operator == (const Vec6<_T>& v) const { return x == v.x && y == v.y && z == v.z && w == v.w && s == v.s && t == v.t; };
template <class _T> inline bool Vec6<_T>::operator != (const Vec6<_T>& v) const { return x != v.x || y != v.y || z != v.z || w != v.w || s != v.s || t != v.t; };
template <class _T> inline bool Vec6<_T>::operator < (const Vec6<_T>& v) const { return x < v.x && y < v.y && z < v.z && w < v.w && s < v.s && t < v.t; };
template <class _T> inline bool Vec6<_T>::operator > (const Vec6<_T>& v) const { return x > v.x && y > v.y && z > v.z && w > v.w && s > v.s && t > v.t; };
template <class _T> inline bool Vec6<_T>::operator <= (const Vec6<_T>& v) const { return x <= v.x && y <= v.y && z <= v.z && w <= v.w && s <= v.s && t <= v.t; };
template <class _T> inline bool Vec6<_T>::operator >= (const Vec6<_T>& v) const { return x >= v.x && y >= v.y && z >= v.z && w >= v.w && s >= v.s && t >= v.t; };
template <class _T> inline bool Vec6<_T>::IsZero () const { return x == 0 && y == 0 && z == 0 && w == 0 && s == 0 && t == 0; };

// Unary ops ----------------------------------------
template <class _T> inline Vec6<_T> Vec6<_T>::operator - () const { Vec6<_T> ret; ret.x = -(x); ret.y = -(y); ret.z = -(z); ret.w = -(w); ret.s = -(s); ret.t = -(t); return ret; };

// Binary ops ---------------------------------------
template <class _T> inline Vec6<_T> Vec6<_T>::operator + (const Vec6<_T>& v) const { Vec6<_T> ret; ret.x = x + v.x; ret.y = y + v.y; ret.z = z + v.z; ret.w = w + v.w; ret.s = s + v.s; ret.t = t + v.t; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator - (const Vec6<_T>& v) const { Vec6<_T> ret; ret.x = x - v.x; ret.y = y - v.y; ret.z = z - v.z; ret.w = w - v.w; ret.s = s - v.s; ret.t = t - v.t; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator * (const Vec6<_T>& v) const { Vec6<_T> ret; ret.x = x * v.x; ret.y = y * v.y; ret.z = z * v.z; ret.w = w * v.w; ret.s = s * v.s; ret.t = t * v.t; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator / (const Vec6<_T>& v) const { Vec6<_T> ret; ret.x = x / v.x; ret.y = y / v.y; ret.z = z / v.z; ret.w = w / v.w; ret.s = s / v.s; ret.t = t / v.t; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator + ( _T v) const { Vec6<_T> ret; ret.x = x + v; ret.y = y + v; ret.z = z + v; ret.w = w + v; ret.s = s + v; ret.t = t + v; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator - ( _T v) const { Vec6<_T> ret; ret.x = x - v; ret.y = y - v; ret.z = z - v; ret.w = w - v; ret.s = s - v; ret.t = t - v; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator * ( _T v) const { Vec6<_T> ret; ret.x = x * v; ret.y = y * v; ret.z = z * v; ret.w = w * v; ret.s = s * v; ret.t = t * v; return ret; };
template <class _T> inline Vec6<_T> Vec6<_T>::operator / ( _T v) const { Vec6<_T> ret; ret.x = x / v; ret.y = y / v; ret.z = z / v; ret.w = w / v; ret.s = s / v; ret.t = t / v; return ret; };

// Assignment ops -----------------------------------
template <class _T> inline Vec6<_T> & Vec6<_T>::operator = (const Vec6<_T>& v) { x = v.x; y = v.y; z = v.z; w = v.w; s = v.s; t = v.t; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator += (const Vec6<_T>& v) { x += v.x; y += v.y; z += v.z; w += v.w; s += v.s; t += v.t; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator -= (const Vec6<_T>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; s -= v.s; t -= v.t; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator *= (const Vec6<_T>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; s *= v.s; t *= v.t; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator /= (const Vec6<_T>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; s /= v.s; t /= v.t; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator = ( _T v) { x = v; y = v; z = v; w = v; s = v; t = v; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator += ( _T v) { x += v; y += v; z += v; w += v; s += v; t += v; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator -= ( _T v) { x -= v; y -= v; z -= v; w -= v; s -= v; t -= v; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator *= ( _T v) { x *= v; y *= v; z *= v; w *= v; s *= v; t *= v; return *this; };
template <class _T> inline Vec6<_T> & Vec6<_T>::operator /= ( _T v) { x /= v; y /= v; z /= v; w /= v; s /= v; t /= v; return *this; };

// Functions ----------------------------------------
template<class _T> inline Vec6<_T> Vec6<_T>::Min( const Vec6<_T>& a, const Vec6<_T>& b) { Vec6<_T> ret; ret.x = min(a.x, b.x); ret.y = min(a.y, b.y); ret.z = min(a.z, b.z); ret.w = min(a.w, b.w); ret.s = min(a.s, b.s); ret.t = min(a.t, b.t); return ret; };
template<class _T> inline Vec6<_T> Vec6<_T>::Max( const Vec6<_T>& a, const Vec6<_T>& b) { Vec6<_T> ret; ret.x = max(a.x, b.x); ret.y = max(a.y, b.y); ret.z = max(a.z, b.z); ret.w = max(a.w, b.w); ret.s = max(a.s, b.s); ret.t = max(a.t, b.t); return ret; };
template<class _T> inline Vec6<_T> Vec6<_T>::Abs() const { Vec6<_T> ret; ret.x = abs(x); ret.y = abs(y); ret.z = abs(z); ret.w = abs(w); ret.s = abs(s); ret.t = abs(t); return ret; }
template<class _T> inline Vec6<_T> Vec6<_T>::Clamp(const Vec6<_T>& a, const Vec6<_T>& b) const { Vec6<_T> ret; ret.x = clamp(x, a.x, b.x); ret.y = clamp(y, a.y, b.y); ret.z = clamp(z, a.z, b.z); ret.w = clamp(w, a.w, b.w); ret.s = clamp(s, a.s, b.s); ret.t = clamp(t, a.t, b.t); return ret; };

typedef Vec6<float> Vec6f;
typedef Vec6<double> Vec6d;
typedef Vec6<int> Vec6i;


#endif
