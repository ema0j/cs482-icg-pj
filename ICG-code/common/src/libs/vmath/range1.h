#ifndef _RANGE1T_H_
#define _RANGE1T_H_

// INCLUDES ====================================================
template<class T>
class Range1 {
	// data ---------------------------------------------
protected:
	T _min;
	T _max;

public:
	// Constructors -------------------------------------
	Range1(const T& min = 1, const T& max = -1);
	Range1(const Range1& v);
	template<class TT>
	Range1(const Range1<TT>& v);

	// Conversions --------------------------------------

	// Access ops ---------------------------------------
	T GetMin() const;
	T GetMax() const;
	T& GetMinRef();
	T& GetMaxRef();
	void SetMin(const T& m);
	void SetMax(const T& M);
	void Set(const T& m, const T& M);
	T GetCenter() const;
	T GetSize() const;
    bool IsValid() const;

	// Comparison ops -----------------------------------
    int operator == (const Range1& v) const;
    int operator != (const Range1& v) const;

	// Binary ops ---------------------------------------

	// Test ops -----------------------------------------
	bool Contain(const T& v) const;

	// Assignment ops -----------------------------------
	Range1& operator=(const Range1& v);

	// Vector ops ---------------------------------------
	void Grow(const T& v);
    void Grow(const Range1& b);
    void Scale(const T& s);
    static bool Overlap(const Range1& a, const Range1& b);
    static Range1 Union(const Range1& a, const Range1& b);
    static Range1 Intersection(const Range1& a, const Range1& b);
    static Range1 Empty();
};

template <>
Range1<int> Range1<int>::Empty();
template <>
Range1<float> Range1<float>::Empty();
template <>
Range1<double> Range1<double>::Empty();

template <class T>
inline Range1<T>::Range1(const T& min, const T& max) {
	_min = min; _max = max;
}
template <class T>
inline Range1<T>::Range1(const Range1<T>& v) {
	_min = v._min; _max = v._max;
}
template <class T> template<class TT>
inline Range1<T>::Range1(const Range1<TT>& v) {
	_min = T(v.GetMin()); _max = T(v.GetMax());
}

template <class T>
inline T Range1<T>::GetMin() const { return _min; }
template <class T>
inline T Range1<T>::GetMax() const { return _max; }
template <class T>
inline T& Range1<T>::GetMinRef() { return _min; }
template <class T>
inline T& Range1<T>::GetMaxRef() { return _max; }
template <class T>
inline void Range1<T>::SetMin(const T& m) { _min = m; }
template <class T>
inline void Range1<T>::SetMax(const T& M) { _max = M; }
template <class T>
inline void Range1<T>::Set(const T& m, const T& M) { _min = m; _max = M; }
template <class T>
inline T Range1<T>::GetCenter() const { return (_max + _min) / 2; }
template <class T>
inline T Range1<T>::GetSize() const { return (_max - _min); }
template <class T>
inline bool Range1<T>::IsValid() const { return (_max >= _min); }

template <class T>
inline int Range1<T>::operator == (const Range1<T>& v) const {
	return _min == v._min && _max == v._max; }
template <class T>
inline int Range1<T>::operator != (const Range1<T>& v) const {
	return !operator==(v); }

template <class T>
inline bool Range1<T>::Contain(const T& v) const {
	return v <= _max && v >= _min;
}

template <class T>
inline Range1<T>& Range1<T>::operator=(const Range1<T>& v) {
	_min = v._min;
	_max = v._max;
	return *this;
}

template <class T>
inline void Range1<T>::Grow(const T& v) {
	if(IsValid()) {
		_min = min(_min,v);
		_max = max(_max,v);
	} else {
		_min = v;
		_max = v;
	}
}
template <class T>
inline void Range1<T>::Grow(const Range1<T>& b) {
	if(IsValid()) {
		_min = min(_min,b._min);
		_max = max(_max,b._max);
	} else {
		_min = b._min;
		_max = b._max;
	}
}
template <class T>
inline void Range1<T>::Scale(const T& s) {
	T center = GetCenter();
	_max = (_max-center)*s + center;
	_min = (_min-center)*s + center;
}
template <class T>
inline Range1<T> Range1<T>::Union(const Range1<T>& a, const Range1<T>& b) {
	Range1<T> ret;
	ret._min = min(a._min,b._min);
	ret._max = max(a._max,b._max);
	return ret;
}
template <class T>
inline Range1<T> Range1<T>::Intersection(const Range1<T>& a, const Range1<T>& b) {
	Range1<T> ret;
	ret._min = min(a._min,b._min);
	ret._max = max(a._max,b._max);
	return ret;
}
template <class T>
inline bool Range1<T>::Overlap(const Range1<T>& a, const Range1<T>& b) {
	return Intersection(a, b).IsValid();
}

typedef Range1<float> Range1f;
typedef Range1<double> Range1d;
typedef Range1<int32_t> Range1i;
typedef Range1<uint32_t> Range1u;

#endif
