#ifndef _RANGE3T_H_
#define _RANGE3T_H_

// INCLUDES ====================================================
#include "vec3.h"
#include <float.h>

template<class T>
class Range3 {
	// data ---------------------------------------------
protected:
	Vec3<T> _min;
	Vec3<T> _max;

public:
	// Constructors -------------------------------------
	Range3(const Vec3<T>& min, const Vec3<T>& max);
	Range3(const Range3& v = Range3::Empty());
	template<class TT>
	Range3(const Range3<TT>& v);

	// Conversions --------------------------------------

	// Access ops ---------------------------------------
	Vec3<T> GetMin() const;
	Vec3<T> GetMax() const;
	Vec3<T>& GetMinRef();
	Vec3<T>& GetMaxRef();
    const Vec3<T>& GetMinRef() const;
    const Vec3<T>& GetMaxRef() const;
	void SetMin(const Vec3<T>& m);
	void SetMax(const Vec3<T>& M);
	void Set(const Vec3<T>& m, const Vec3<T>& M);
	Vec3<T> GetCenter() const;
	Vec3<T> GetSize() const;
    T GetVolumn() const;
    bool IsValid() const;
    T Diagonal() const;
    void GetCorners(Vec3<T> P[8]) const;
    T SurfaceArea() const;
    Vec3f ClosestPoint(const Vec3f &p) const;
	// Comparison ops -----------------------------------
    int operator == (const Range3& v) const;
    int operator != (const Range3& v) const;

    inline const Vec3<T> operator[](int i) const;
    inline Vec3<T> operator[](int i);

	// Binary ops ---------------------------------------

	// Test ops -----------------------------------------
	bool Contain(const Vec3<T>& v) const;

	// Assignment ops -----------------------------------
	Range3& operator=(const Range3& v);

	// Vector ops ---------------------------------------
	void Grow(const Vec3<T>& v);
    void Grow(const Range3& b);
    void Scale(const Vec3<T>& s);
    Vec3<T> Sample(float u1, float u2, float u3) const;
    static bool Overlap(const Range3& a, const Range3& b);
    static Range3 Union(const Range3& a, const Range3& b);
    static Range3 Intersection(const Range3& a, const Range3& b);
    static float  DistanceSqr(const Range3& a, const Range3 &b);
    static Range3 Empty();
};

template<class T>
float Range3<T>::DistanceSqr( const Range3& a, const Range3 &b )
{
    float sqrDist = 0;
    for (uint32_t i = 0; i < 3; i++)
    {
        if(b.GetMaxRef()[i] < a.GetMinRef()[i])
        {
            float d = b.GetMaxRef()[i] - a.GetMinRef()[i];
            sqrDist += d * d;
        }
        else if(b.GetMinRef()[i] > a.GetMaxRef()[i])
        {
            float d = b.GetMinRef()[i] - a.GetMaxRef()[i];
            sqrDist += d * d;
        }
    }
    return sqrDist;
}

template<class T>
Vec3f Range3<T>::ClosestPoint( const Vec3f &p ) const
{
    return p.Clamp(_min, _max);
}

template <>
Range3<int> Range3<int>::Empty();
template <>
Range3<float> Range3<float>::Empty();
template <>
Range3<double> Range3<double>::Empty();

// MACROS USEFUL FOR CODE GENERATION ===========================
template <class T>
inline Range3<T>::Range3(const Vec3<T>& min, const Vec3<T>& max) {
	_min = min; _max = max;
}
template <class T>
inline Range3<T>::Range3(const Range3<T>& v) {
	_min = v._min; _max = v._max;
}
template <class T> template<class TT>
inline Range3<T>::Range3(const Range3<TT>& v) {
	_min = Vec3<T>(v.GetMin()); _max = Vec3<T>(v.GetMax());
}
template <class T>
Vec3<T> Range3<T>::Sample(float u1, float u2, float u3) const
{
    Vec3<T> p;
    p.x = u1 * (_max.x - _min.x) + _min.x;
    p.y = u2 * (_max.y - _min.y) + _min.y;
    p.z = u3 * (_max.z - _min.z) + _min.z;
    return p;
}

template <class T>
T Range3<T>::SurfaceArea() const
{
    Vec3f d = _max - _min;
    return 2.f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

template <class T>
const Vec3<T> Range3<T>::operator[](int i) const 
{
    assert(i == 0 || i == 1);
    return i == 0 ? _min : _max;
}

template <class T>
Vec3<T> Range3<T>::operator[](int i) 
{
    assert(i == 0 || i == 1);
    return i == 0 ? _min : _max;
}

template <class T>
inline Vec3<T> Range3<T>::GetMin() const { return _min; }
template <class T>
inline Vec3<T> Range3<T>::GetMax() const { return _max; }
template <class T>
inline Vec3<T>& Range3<T>::GetMinRef() { return _min; }
template <class T>
inline Vec3<T>& Range3<T>::GetMaxRef() { return _max; }
template <class T>
inline const Vec3<T>& Range3<T>::GetMinRef() const { return _min; }
template <class T>
inline const Vec3<T>& Range3<T>::GetMaxRef() const { return _max; }
template <class T>
inline void Range3<T>::SetMin(const Vec3<T>& m) { _min = m; }
template <class T>
inline void Range3<T>::SetMax(const Vec3<T>& M) { _max = M; }
template <class T>
inline void Range3<T>::Set(const Vec3<T>& m, const Vec3<T>& M) { _min = m; _max = M; }
template <class T>
inline Vec3<T> Range3<T>::GetCenter() const { return (_max + _min) / 2; }
template <class T>
inline Vec3<T> Range3<T>::GetSize() const { return (_max - _min); }
template <class T>
inline T Range3<T>::GetVolumn() const { Vec3<T> &size = GetSize(); return (size.x * size.y * size.z); }
template <class T>
inline bool Range3<T>::IsValid() const { return (_max[0] >= _min[0] && _max[1] >= _min[1] && _max[2] >= _min[2]); }
template <class T>
inline T Range3<T>::Diagonal() const { return (_max-_min).GetLength(); }
template <class T>
inline void Range3<T>::GetCorners(Vec3<T> p[8]) const {
	p[0] = Vec3<T>(_min[0], _min[1], _min[2]);
	p[1] = Vec3<T>(_min[0], _min[1], _max[2]);
	p[2] = Vec3<T>(_min[0], _max[1], _min[2]);
	p[3] = Vec3<T>(_min[0], _max[1], _max[2]);
	p[4] = Vec3<T>(_max[0], _min[1], _min[2]);
	p[5] = Vec3<T>(_max[0], _min[1], _max[2]);
	p[6] = Vec3<T>(_max[0], _max[1], _min[2]);
	p[7] = Vec3<T>(_max[0], _max[1], _max[2]);
}

template <class T>
inline int Range3<T>::operator == (const Range3<T>& v) const {
	return _min == v._min && _max == v._max; }
template <class T>
inline int Range3<T>::operator != (const Range3<T>& v) const {
	return !operator==(v); }

template <class T>
inline bool Range3<T>::Contain(const Vec3<T>& v) const {
	return v <= _max && v >= _min;
}

template <class T>
inline Range3<T>& Range3<T>::operator=(const Range3<T>& v) {
	_min = v._min;
	_max = v._max;
	return *this;
}

template <class T>
inline void Range3<T>::Grow(const Vec3<T>& v) {
	if(IsValid()) {
		_min = Vec3<T>::Min(_min,v);
		_max = Vec3<T>::Max(_max,v);
	} else {
		_min = v;
		_max = v;
	}
}
template <class T>
inline void Range3<T>::Grow(const Range3<T>& b) {
	if(IsValid()) {
		_min = Vec3<T>::Min(_min,b._min);
		_max = Vec3<T>::Max(_max,b._max);
	} else {
		_min = b._min;
		_max = b._max;
	}
}
template <class T>
inline void Range3<T>::Scale(const Vec3<T>& s) {
	Vec3<T> center = GetCenter();
	_max[0] = (_max[0]-center[0])*s[0] + center[0];
	_min[0] = (_min[0]-center[0])*s[0] + center[0];
	_max[1] = (_max[1]-center[1])*s[1] + center[1];
	_min[1] = (_min[1]-center[1])*s[1] + center[1];
	_max[2] = (_max[2]-center[2])*s[2] + center[2];
	_min[2] = (_min[2]-center[2])*s[2] + center[2];
}
template <class T>
inline Range3<T> Range3<T>::Union(const Range3<T>& a, const Range3<T>& b) {
	Range3<T> ret;
	ret._min = Vec3<T>::Min(a._min,b._min);
	ret._max = Vec3<T>::Max(a._max,b._max);
	return ret;
}
template <class T>
inline Range3<T> Range3<T>::Intersection(const Range3<T>& a, const Range3<T>& b) {
	Range3<T> ret;
	ret._min = Vec3<T>::Max(a._min,b._min);
	ret._max = Vec3<T>::Min(a._max,b._max);
	return ret;
}
template <class T>
inline bool Range3<T>::Overlap(const Range3<T>& a, const Range3<T>& b) {
	return Intersection(a, b).IsValid();
}

typedef Range3<float> Range3f;
typedef Range3<double> Range3d;
typedef Range3<int> Range3i;

#endif
