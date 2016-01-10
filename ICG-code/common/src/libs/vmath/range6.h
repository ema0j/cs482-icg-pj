#ifndef _RANGE6T_H_
#define _RANGE6T_H_

// INCLUDES ====================================================
#include "vec6.h"
#include <float.h>

template<class T>
class Range6 {
	// data ---------------------------------------------
protected:
	Vec6<T> _min;
	Vec6<T> _max;

public:
	// Constructors -------------------------------------
	Range6(const Vec6<T>& min = Vec6<T>(1,1,1,1,1,1), const Vec6<T>& max = Vec6<T>(-1,-1,-1,-1,-1,-1));
	Range6(const Range6& v);
	template<class TT>
	Range6(const Range6<TT>& v);

	// Conversions --------------------------------------

	// Access ops ---------------------------------------
	Vec6<T> GetMin() const;
	Vec6<T> GetMax() const;
	Vec6<T>& GetMinRef();
	Vec6<T>& GetMaxRef();
	void SetMin(const Vec6<T>& m);
	void SetMax(const Vec6<T>& M);
	void Set(const Vec6<T>& m, const Vec6<T>& M);
	Vec6<T> GetCenter() const;
	Vec6<T> GetSize() const;
    bool IsValid() const;
    T Diagonal() const;
    void GetCorners(Vec6<T> P[8]) const;

	// Comparison ops -----------------------------------
    int operator == (const Range6& v) const;
    int operator != (const Range6& v) const;

	// Binary ops ---------------------------------------

	// Test ops -----------------------------------------
	bool Contain(const Vec6<T>& v) const;

	// Assignment ops -----------------------------------
	Range6& operator=(const Range6& v);

	// Vector ops ---------------------------------------
	void Grow(const Vec6<T>& v);
    void Grow(const Range6& b);
    void Scale(const Vec6<T>& s);
    Vec6<T> Sample(float u1, float u2, float u3, float u4, float u5, float u6) const;
    static bool Overlap(const Range6& a, const Range6& b);
    static Range6 Union(const Range6& a, const Range6& b);
    static Range6 Intersection(const Range6& a, const Range6& b);
    static float  DistanceSqr(const Range6& a, const Range6 &b);
    static Range6 Empty();
};

template <>
Range6<int> Range6<int>::Empty();
template <>
Range6<float> Range6<float>::Empty();
template <>
Range6<double> Range6<double>::Empty();

// MACROS USEFUL FOR CODE GENERATION ===========================
template <class T>
inline Range6<T>::Range6(const Vec6<T>& min, const Vec6<T>& max) {
	_min = min; _max = max;
}
template <class T>
inline Range6<T>::Range6(const Range6<T>& v) {
	_min = v._min; _max = v._max;
}
template <class T> template<class TT>
inline Range6<T>::Range6(const Range6<TT>& v) {
	_min = Vec6<T>(v.GetMin()); _max = Vec6<T>(v.GetMax());
}
template <class T>
Vec6<T> Range6<T>::Sample(float u1, float u2, float u3, float u4, float u5, float u6) const
{
    Vec6<T> p;
    p.x = u1 * (_max.x - _min.x) + _min.x;
    p.y = u2 * (_max.y - _min.y) + _min.y;
    p.z = u3 * (_max.z - _min.z) + _min.z;
    p.w = u4 * (_max.w - _min.w) + _min.w;
    p.s = u5 * (_max.s - _min.s) + _min.s;
    p.t = u6 * (_max.t - _min.t) + _min.t;
    return p;
}

template <class T>
inline Vec6<T> Range6<T>::GetMin() const { return _min; }
template <class T>
inline Vec6<T> Range6<T>::GetMax() const { return _max; }
template <class T>
inline Vec6<T>& Range6<T>::GetMinRef() { return _min; }
template <class T>
inline Vec6<T>& Range6<T>::GetMaxRef() { return _max; }
template <class T>
inline void Range6<T>::SetMin(const Vec6<T>& m) { _min = m; }
template <class T>
inline void Range6<T>::SetMax(const Vec6<T>& M) { _max = M; }
template <class T>
inline void Range6<T>::Set(const Vec6<T>& m, const Vec6<T>& M) { _min = m; _max = M; }
template <class T>
inline Vec6<T> Range6<T>::GetCenter() const { return (_max + _min) / 2; }
template <class T>
inline Vec6<T> Range6<T>::GetSize() const { return (_max - _min); }
template <class T>
inline bool Range6<T>::IsValid() const { return (_max[0] >= _min[0] && _max[1] >= _min[1] && _max[2] >= _min[2] && _max[3] >= _min[3] && _max[4] >= _min[4] && _max[5] >= _min[5]); }
template <class T>
inline T Range6<T>::Diagonal() const { return (_max-_min).GetLength(); }
template <class T>
inline void Range6<T>::GetCorners(Vec6<T> p[8]) const {
	p[0] = Vec6<T>(_min[0], _min[1], _min[2]);
	p[1] = Vec6<T>(_min[0], _min[1], _max[2]);
	p[2] = Vec6<T>(_min[0], _max[1], _min[2]);
	p[3] = Vec6<T>(_min[0], _max[1], _max[2]);
	p[4] = Vec6<T>(_max[0], _min[1], _min[2]);
	p[5] = Vec6<T>(_max[0], _min[1], _max[2]);
	p[6] = Vec6<T>(_max[0], _max[1], _min[2]);
	p[7] = Vec6<T>(_max[0], _max[1], _max[2]);
}

template <class T>
inline int Range6<T>::operator == (const Range6<T>& v) const {
	return _min == v._min && _max == v._max; }
template <class T>
inline int Range6<T>::operator != (const Range6<T>& v) const {
	return !operator==(v); }

template <class T>
inline bool Range6<T>::Contain(const Vec6<T>& v) const {
	return v <= _max && v >= _min;
}

template <class T>
inline Range6<T>& Range6<T>::operator=(const Range6<T>& v) {
	_min = v._min;
	_max = v._max;
	return *this;
}

template <class T>
inline void Range6<T>::Grow(const Vec6<T>& v) {
	if(IsValid()) {
		_min = Vec6<T>::Min(_min,v);
		_max = Vec6<T>::Max(_max,v);
	} else {
		_min = v;
		_max = v;
	}
}
template <class T>
inline void Range6<T>::Grow(const Range6<T>& b) {
	if(IsValid()) {
		_min = Vec6<T>::Min(_min,b._min);
		_max = Vec6<T>::Max(_max,b._max);
	} else {
		_min = b._min;
		_max = b._max;
	}
}
template <class T>
inline void Range6<T>::Scale(const Vec6<T>& s) {
	Vec6<T> center = GetCenter();
	_max[0] = (_max[0]-center[0])*s[0] + center[0];
	_min[0] = (_min[0]-center[0])*s[0] + center[0];
	_max[1] = (_max[1]-center[1])*s[1] + center[1];
	_min[1] = (_min[1]-center[1])*s[1] + center[1];
	_max[2] = (_max[2]-center[2])*s[2] + center[2];
	_min[2] = (_min[2]-center[2])*s[2] + center[2];
    _max[3] = (_max[3]-center[3])*s[3] + center[3];
    _min[3] = (_min[3]-center[3])*s[3] + center[3];
    _max[4] = (_max[4]-center[4])*s[4] + center[4];
    _min[4] = (_min[4]-center[4])*s[4] + center[4];
    _max[5] = (_max[5]-center[5])*s[5] + center[5];
    _min[5] = (_min[5]-center[5])*s[5] + center[5];
}
template <class T>
inline Range6<T> Range6<T>::Union(const Range6<T>& a, const Range6<T>& b) {
	Range6<T> ret;
	ret._min = Vec6<T>::Min(a._min,b._min);
	ret._max = Vec6<T>::Max(a._max,b._max);
	return ret;
}
template <class T>
inline Range6<T> Range6<T>::Intersection(const Range6<T>& a, const Range6<T>& b) {
	Range6<T> ret;
	ret._min = Vec6<T>::Max(a._min,b._min);
	ret._max = Vec6<T>::Min(a._max,b._max);
	return ret;
}
template <class T>
inline bool Range6<T>::Overlap(const Range6<T>& a, const Range6<T>& b) {
	return Intersection(a, b).IsValid();
}

template <class T>
float Range6<T>::DistanceSqr(const Range6& a, const Range6 &b)
{
    float sqrDist = 0;
    for (uint32_t i = 0; i < 6; i++)
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

typedef Range6<float> Range6f;
typedef Range6<double> Range6d;
typedef Range6<int> Range6i;

#endif
