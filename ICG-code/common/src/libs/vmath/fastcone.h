#ifndef _FAST_CONET_H_
#define _FAST_CONET_H_

// INCLUDES ====================================================
#include "vec3.h"
#include "range3.h"

template<class T>
class FastCone {
	// data ---------------------------------------------
protected:
	Range3<T> _dirBox;
public:
	// Constructors -------------------------------------
	FastCone(const Vec3<T>& ax = Vec3<T>::Z());
	FastCone(const FastCone<T>& v);

	// Access ops ---------------------------------------
	Vec3<T>		GetAxis() const;
	T		    GetAngleCos() const;
    T		    GetAngleSin() const;
	void		Set(const Vec3<T> *v, const uint32_t count);
	bool		IsValid() const;

	// Comparison ops -----------------------------------
	int operator == (const FastCone& v) const;
	int operator != (const FastCone& v) const;

	// Binary ops ---------------------------------------

	// Test ops -----------------------------------------
	bool Contain(const Vec3<T>& v) const;

	// Assignment ops -----------------------------------
	FastCone& operator=(const FastCone& v);

	// Vector ops ---------------------------------------
	void Grow(const Vec3<T>& v);
	void Grow(const FastCone& b);

	static bool Overlap(const FastCone& a, const FastCone& b);
	static FastCone Union(const FastCone& a, const FastCone& b);
};

// MACROS USEFUL FOR CODE GENERATION ===========================
template <class T>
inline FastCone<T>::FastCone(const Vec3<T>& ax) : _dirBox(ax, ax) {}

template <class T>
inline FastCone<T>::FastCone(const FastCone<T>& v) : _dirBox(v._dirBox) {} 

template <class T>
inline Vec3<T> FastCone<T>::GetAxis() const 
{
    return _dirBox.GetCenter().GetNormalized();
}

template <class T>
inline T FastCone<T>::GetAngleSin() const 
{
    T c = GetAngleCos();
    return sqrt(1 - c * c);
}

template <class T>
inline T FastCone<T>::GetAngleCos() const 
{
    Vec3<T> center = _dirBox.GetCenter();
    T r2 = (_dirBox.GetMax() - _dirBox.GetMin()).GetLengthSqr() * static_cast<T>(0.25);
    T d2 = center.GetLengthSqr();
    if (d2 == static_cast<T>(0))
    {
        return static_cast<T>(0);
    }
    float d = sqrt(d2);
    float a = (d2 - r2 + 1) / (2 * d);
    if (a < static_cast<T>(0))
    {
        a = static_cast<T>(0);
    }
    return clamp(a, 0.0f, 1.0f);
}

template <class T>
inline void FastCone<T>::Set(const Vec3<T> *v, const uint32_t count) 
{
    _dirBox = Range3<T>::Empty();
    for (uint32_t i = 0; i < count; i++)
    {
        _dirBox.Grow(v[i]);
    }
}

template <class T>
inline bool FastCone<T>::IsValid() const 
{ 
    Vec3<T> center = _dirBox.GetCenter();
    T r2 = (_dirBox.GetMax() - _dirBox.GetMin()).GetLengthSqr() * static_cast<T>(0.25);
    T d2 = center.GetLengthSqr();
    if (d2 == static_cast<T>(0))
    {
        return false;
    }
    float d = sqrt(d2);
    float a = (d2 - r2 + 1) / (2 * d);
    return (a >= static_cast<T>(0) && a <= static_cast<T>(1));
}

template <class T>
inline int FastCone<T>::operator == (const FastCone<T>& v) const {
	return _dirBox == v._dirBox;
}
template <class T>
inline int FastCone<T>::operator != (const FastCone<T>& v) const {
	return !operator==(v); 
}

template <class T>
inline bool FastCone<T>::Contain(const Vec3<T>& v) const {
	return _dirBox.Contain(v);
}

template <class T>
inline FastCone<T>& FastCone<T>::operator=(const FastCone<T>& v) {
	_dirBox = v._dirBox;
	return *this;
}

template <class T>
inline void FastCone<T>::Grow(const Vec3<T>& v) {
	_dirBox.Grow(v);
}

template <class T>
inline void FastCone<T>::Grow(const FastCone<T>& b) {
	*this = Union(*this, b);
}


template <class T>
inline FastCone<T> FastCone<T>::Union(const FastCone<T>& a, const FastCone<T>& b) {
    FastCone<T> result;
    result._dirBox = Range3<T>::Union(a._dirBox, b._dirBox);
	return result;
}

template <class T>
inline bool FastCone<T>::Overlap(const FastCone<T>& a, const FastCone<T>& b)
{
    return Range3<T>::Overlap(a._dirBox, b._dirBox);
}

typedef FastCone<float> FastConef;
typedef FastCone<double> FastConed;

#endif
