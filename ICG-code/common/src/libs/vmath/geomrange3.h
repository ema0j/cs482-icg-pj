#ifndef _GEOMRANGE3T_H_
#define _GEOMRANGE3T_H_

// INCLUDES ====================================================
#include "geom3.h"

template<class T>
class PointRange3 {
	// static data --------------------------------------
public:
	static PointRange3 ZERO;

	// data ---------------------------------------------
protected:
	Point3<T> _min;
	Point3<T> _max;

public:
	// Constructors -------------------------------------
	PointRange3(const Point3<T>& min = Point3<T>(1,1,1), const Point3<T>& max = Point3<T>(-1,-1,-1));
	PointRange3(const PointRange3& v);
	template<class TT>
	PointRange3(const PointRange3<TT>& v);

	// Conversions --------------------------------------

	// Access ops ---------------------------------------
	Point3<T> GetMin() const;
	Point3<T> GetMax() const;
	Point3<T>& GetMinRef();
	Point3<T>& GetMaxRef();
	void SetMin(const Point3<T>& m);
	void SetMax(const Point3<T>& M);
	void Set(const Point3<T>& m, const Point3<T>& M);
	Point3<T> GetCenter() const;
	Vector3<T> GetSize() const;
    bool IsValid() const;
    T Diagonal() const;
    void GetCorners(Point3<T> P[8]) const;

	// Comparison ops -----------------------------------
    int operator == (const PointRange3& v) const;
    int operator != (const PointRange3& v) const;

	// Binary ops ---------------------------------------

	// Test ops -----------------------------------------
	bool Contain(const Point3<T>& v) const;

	// Assignment ops -----------------------------------
	PointRange3& operator=(const PointRange3& v);

	// Vector ops ---------------------------------------
	void Grow(const Point3<T>& v);
    void Grow(const PointRange3& b);
    void Scale(const Point3<T>& s);
    static bool Overlap(const PointRange3& a, const PointRange3& b);
    static PointRange3 Union(const PointRange3& a, const PointRange3& b);
    static PointRange3 Intersection(const PointRange3& a, const PointRange3& b);
};

// MACROS USEFUL FOR CODE GENERATION ===========================
template <class T>
inline PointRange3<T>::PointRange3(const Point3<T>& min, const Point3<T>& max) {
	_min = min; _max = max;
}
template <class T>
inline PointRange3<T>::PointRange3(const PointRange3<T>& v) {
	_min = v._min; _max = v._max;
}
template <class T> template<class TT>
inline PointRange3<T>::PointRange3(const PointRange3<TT>& v) {
	_min = Point3<T>(v.GetMin()); _max = Point3<T>(v.GetMax());
}

template <class T>
inline Point3<T> PointRange3<T>::GetMin() const { return _min; }
template <class T>
inline Point3<T> PointRange3<T>::GetMax() const { return _max; }
template <class T>
inline Point3<T>& PointRange3<T>::GetMinRef() { return _min; }
template <class T>
inline Point3<T>& PointRange3<T>::GetMaxRef() { return _max; }
template <class T>
inline void PointRange3<T>::SetMin(const Point3<T>& m) { _min = m; }
template <class T>
inline void PointRange3<T>::SetMax(const Point3<T>& M) { _max = M; }
template <class T>
inline void PointRange3<T>::Set(const Point3<T>& m, const Point3<T>& M) { _min = m; _max = M; }
template <class T>
inline Point3<T> PointRange3<T>::GetCenter() const { return Point3<T>::AffineCombination(_max, T(0.5), _min, T(0.5)); }
template <class T>
inline Vector3<T> PointRange3<T>::GetSize() const { return (_max - _min); }
template <class T>
inline bool PointRange3<T>::IsValid() const { return (_max[0] >= _min[0] && _max[1] >= _min[1] && _max[2] >= _min[2]); }
template <class T>
inline T PointRange3<T>::Diagonal() const { return (_max-_min).GetLength(); }
template <class T>
inline void PointRange3<T>::GetCorners(Point3<T> p[8]) const {
	p[0] = Point3<T>(_min[0], _min[1], _min[2]);
	p[1] = Point3<T>(_min[0], _min[1], _max[2]);
	p[2] = Point3<T>(_min[0], _max[1], _min[2]);
	p[3] = Point3<T>(_min[0], _max[1], _max[2]);
	p[4] = Point3<T>(_max[0], _min[1], _min[2]);
	p[5] = Point3<T>(_max[0], _min[1], _max[2]);
	p[6] = Point3<T>(_max[0], _max[1], _min[2]);
	p[7] = Point3<T>(_max[0], _max[1], _max[2]);
}

template <class T>
inline int PointRange3<T>::operator == (const PointRange3<T>& v) const {
	return _min == v._min && _max == v._max; }
template <class T>
inline int PointRange3<T>::operator != (const PointRange3<T>& v) const {
	return !operator==(v); }

template <class T>
inline bool PointRange3<T>::Contain(const Point3<T>& v) const {
	return v <= _max && v >= _min;
}

template <class T>
inline PointRange3<T>& PointRange3<T>::operator=(const PointRange3<T>& v) {
	_min = v._min;
	_max = v._max;
	return *this;
}

template <class T>
inline void PointRange3<T>::Grow(const Point3<T>& v) {
	if(IsValid()) {
		_min = Point3<T>::Min(_min,v);
		_max = Point3<T>::Max(_max,v);
	} else {
		_min = v;
		_max = v;
	}
}
template <class T>
inline void PointRange3<T>::Grow(const PointRange3<T>& b) {
	if(IsValid()) {
		_min = Point3<T>::Min(_min,b._min);
		_max = Point3<T>::Max(_max,b._max);
	} else {
		_min = b._min;
		_max = b._max;
	}
}
template <class T>
inline void PointRange3<T>::Scale(const Point3<T>& s) {
	Point3<T> center = GetCenter();
	_max[0] = (_max[0]-center[0])*s[0] + center[0];
	_min[0] = (_min[0]-center[0])*s[0] + center[0];
	_max[1] = (_max[1]-center[1])*s[1] + center[1];
	_min[1] = (_min[1]-center[1])*s[1] + center[1];
	_max[2] = (_max[2]-center[2])*s[2] + center[2];
	_min[2] = (_min[2]-center[2])*s[2] + center[2];
}
template <class T>
inline PointRange3<T> PointRange3<T>::Union(const PointRange3<T>& a, const PointRange3<T>& b) {
	PointRange3<T> ret;
	ret._min = Point3<T>::Min(a._min,b._min);
	ret._max = Point3<T>::Max(a._max,b._max);
	return ret;
}
template <class T>
inline PointRange3<T> PointRange3<T>::Intersection(const PointRange3<T>& a, const PointRange3<T>& b) {
	PointRange3<T> ret;
	ret._min = Point3<T>::Max(a._min,b._min);
	ret._max = Point3<T>::Min(a._max,b._max);
	return ret;
}
template <class T>
inline bool PointRange3<T>::Overlap(const PointRange3<T>& a, const PointRange3<T>& b) {
	return Intersection(a, b).IsValid();
}

typedef PointRange3<float> PointRange3f;
typedef PointRange3<double> PointRange3d;

#endif
