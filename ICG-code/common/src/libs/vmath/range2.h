#ifndef _RANGE2T_H_
#define _RANGE2T_H_

// INCLUDES ====================================================
#include "vec2.h"

template<class T>
class Range2 {
	// static data --------------------------------------
public:
	static Range2 ZERO;
	// data ---------------------------------------------
protected:
	Vec2<T> _min;
	Vec2<T> _max;

public:
	// Constructors -------------------------------------
	Range2(const Vec2<T>& min = Vec2<T>(1,1), const Vec2<T>& max = Vec2<T>(-1,-1));
	Range2(const Range2& v);
	template<class TT>
	Range2(const Range2<TT>& v);

	// Conversions --------------------------------------

	// Access ops ---------------------------------------
	Vec2<T> GetMin() const;
	Vec2<T> GetMax() const;
	Vec2<T>& GetMinRef();
	Vec2<T>& GetMaxRef();
	void SetMin(const Vec2<T>& m);
	void SetMax(const Vec2<T>& M);
	void Set(const Vec2<T>& m, const Vec2<T>& M);
	Vec2<T> GetCenter() const;
	Vec2<T> GetSize() const;
    bool IsValid() const;
    T Diagonal() const;
    void GetCorners(Vec2<T> P[4]) const;

	// Comparison ops -----------------------------------
    int operator == (const Range2& v) const;
    int operator != (const Range2& v) const;

	// Binary ops ---------------------------------------

	// Test ops -----------------------------------------
	bool Contain(const Vec2<T>& v) const;

	// Assignment ops -----------------------------------
	Range2& operator=(const Range2& v);

	// Vector ops ---------------------------------------
	void Grow(const Vec2<T>& v);
    void Grow(const Range2& b);
    void Scale(const Vec2<T>& s);
    static bool Overlap(const Range2& a, const Range2& b);
    static Range2 Union(const Range2& a, const Range2& b);
    static Range2 Intersection(const Range2& a, const Range2& b);
    static Range2 Empty();
};

template <>
Range2<int> Range2<int>::Empty();
template <>
Range2<float> Range2<float>::Empty();
template <>
Range2<double> Range2<double>::Empty();

// MACROS USEFUL FOR CODE GENERATION ===========================
template <class T>
inline Range2<T>::Range2(const Vec2<T>& min, const Vec2<T>& max) {
	_min = min; _max = max;
}
template <class T>
inline Range2<T>::Range2(const Range2<T>& v) {
	_min = v._min; _max = v._max;
}
template <class T> template<class TT>
inline Range2<T>::Range2(const Range2<TT>& v) {
	_min = Vec2<T>(v.GetMin()); _max = Vec2<T>(v.GetMax());
}

template <class T>
inline Vec2<T> Range2<T>::GetMin() const { return _min; }
template <class T>
inline Vec2<T> Range2<T>::GetMax() const { return _max; }
template <class T>
inline Vec2<T>& Range2<T>::GetMinRef() { return _min; }
template <class T>
inline Vec2<T>& Range2<T>::GetMaxRef() { return _max; }
template <class T>
inline void Range2<T>::SetMin(const Vec2<T>& m) { _min = m; }
template <class T>
inline void Range2<T>::SetMax(const Vec2<T>& M) { _max = M; }
template <class T>
inline void Range2<T>::Set(const Vec2<T>& m, const Vec2<T>& M) { _min = m; _max = M; }
template <class T>
inline Vec2<T> Range2<T>::GetCenter() const { return (_max + _min) / 2; }
template <class T>
inline Vec2<T> Range2<T>::GetSize() const { return (_max - _min); }
template <class T>
inline bool Range2<T>::IsValid() const { return (_max[0] >= _min[0] && _max[1] >= _min[1]); }
template <class T>
inline T Range2<T>::Diagonal() const { return (_max-_min).GetLength(); }
template <class T>
inline void Range2<T>::GetCorners(Vec2<T> p[4]) const {
	p[0] = Vec2<T>(_min[0], _min[1]);
	p[1] = Vec2<T>(_min[0], _max[1]);
	p[2] = Vec2<T>(_max[0], _min[1]);
	p[3] = Vec2<T>(_max[0], _max[1]);
}

template <class T>
inline int Range2<T>::operator == (const Range2<T>& v) const {
	return _min == v._min && _max == v._max; }
template <class T>
inline int Range2<T>::operator != (const Range2<T>& v) const {
	return !operator==(v); }

template <class T>
inline bool Range2<T>::Contain(const Vec2<T>& v) const {
	return v <= _max && v >= _min;
}

template <class T>
inline Range2<T>& Range2<T>::operator=(const Range2<T>& v) {
	_min = v._min;
	_max = v._max;
	return *this;
}

template <class T>
inline void Range2<T>::Grow(const Vec2<T>& v) {
	if(IsValid()) {
		_min = Vec2<T>::Min(_min,v);
		_max = Vec2<T>::Max(_max,v);
	} else {
		_min = v;
		_max = v;
	}
}
template <class T>
inline void Range2<T>::Grow(const Range2<T>& b) {
	if(IsValid()) {
		_min = Vec2<T>::Min(_min,b._min);
		_max = Vec2<T>::Max(_max,b._max);
	} else {
		_min = b._min;
		_max = b._max;
	}
}
template <class T>
inline void Range2<T>::Scale(const Vec2<T>& s) {
	Vec2<T> center = GetCenter();
	_max[0] = (_max[0]-center[0])*s[0] + center[0];
	_min[0] = (_min[0]-center[0])*s[0] + center[0];
	_max[1] = (_max[1]-center[1])*s[1] + center[1];
	_min[1] = (_min[1]-center[1])*s[1] + center[1];
}
template <class T>
inline Range2<T> Range2<T>::Union(const Range2<T>& a, const Range2<T>& b) {
	Range2<T> ret;
	ret._min = Vec2<T>::Min(a._min,b._min);
	ret._max = Vec2<T>::Max(a._max,b._max);
	return ret;
}
template <class T>
inline Range2<T> Range2<T>::Intersection(const Range2<T>& a, const Range2<T>& b) {
	Range2<T> ret;
	ret._min = Vec2<T>::Max(a._min,b._min);
	ret._max = Vec2<T>::Min(a._max,b._max);
	return ret;
}
template <class T>
inline bool Range2<T>::Overlap(const Range2<T>& a, const Range2<T>& b) {
	return Intersection(a, b).IsValid();
}

typedef Range2<float> Range2f;
typedef Range2<double> Range2d;
typedef Range2<int> Range2i;

#endif
