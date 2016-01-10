#ifndef _PROJECTION_H_
#define _PROJECTION_H_

// INCLUDES ====================================================
#include "functions.h"
#include "consts.h"
#include "matrix4.h"

// TYPE DECLARATION ============================================
template<class T>
class Projection {
	// static data --------------------------------------
public:

	// data ---------------------------------------------
protected:
	bool _isPerspective;
	T _fov; // in degrees
	T _aspectRatio;
	T _n;  // near (I am using this name since there is a bug in the MS compiler)
	T _f;

public:
	// Constructors -------------------------------------
	Projection(bool isPerspective = true,
			   T fov = 45,
			   T aspectRatio = 1,
			   T n = 0.1,
			   T f = 10000);

	// Conversions --------------------------------------
	Matrix4<T> ComputeMatrix() const;
	operator Matrix4<T>() const;

	// Access ops ---------------------------------------
	bool IsPerspective() const;
	T GetFOV() const;
	T GetAspectRatio() const;
	T GetNear() const;
	T GetFar() const;

	// Setters ------------------------------------------
	void SetIsPerspective(bool isPerspective);
	void SetFOV(T fov);
	void SetAspectRatio(T aspectRatio);
	void SetNear(T n);
	void SetFar(T f);

	// Comparison ops -----------------------------------
    bool operator==(const Projection& f);

	// Assignment ops -----------------------------------
};

// IMPLEMENTATION ==============================================
// Constructors -------------------------------------
template<class T> inline Projection<T>::Projection(bool isPerspective,
							  T fov,
							  T aspectRatio,
							  T n,
							  T f) {
    _isPerspective = isPerspective;
	_fov = fov;
	_aspectRatio = aspectRatio;
	_n = n;
	_f = f;
}

// Conversions --------------------------------------
template<class T> inline Matrix4<T> Projection<T>::ComputeMatrix() const {
	if(_isPerspective) {
		return Matrix4<T>::PerspectiveGL(_fov, _aspectRatio, _n, _f);
	} else {
		return Matrix4<T>::OrthographicGL(_fov, _aspectRatio, _n, _f);
	}
}

template<class T> inline Projection<T>::operator Matrix4<T>() const {
	return ComputeMatrix();
}

// Access ops ---------------------------------------
template<class T> inline bool Projection<T>::IsPerspective() const {
	return _isPerspective;
}

template<class T> inline T Projection<T>::GetFOV() const {
	return _fov;
}

template<class T> inline T Projection<T>::GetAspectRatio() const {
	return _aspectRatio;
}

template<class T> inline T Projection<T>::GetNear() const {
	return _n;
}

template<class T> inline T Projection<T>::GetFar() const {
	return _f;
}

// Setters ------------------------------------------
template<class T> inline void Projection<T>::SetIsPerspective(bool isPerspective) {
	_isPerspective = isPerspective;
}

template<class T> inline void Projection<T>::SetFOV(T fov) {
	_fov = fov;
}

template<class T> inline void Projection<T>::SetAspectRatio(T aspectRatio) {
	_aspectRatio = aspectRatio;
}

template<class T> inline void Projection<T>::SetNear(T n) {
	_n = n;
}

template<class T> inline void Projection<T>::SetFar(T f) {
	_f = f;
}

template<class T> inline bool Projection<T>::operator==(const Projection<T>& f) {
    return  _isPerspective = f._isPerspective &&
            _fov == f._fov &&
            _aspectRatio == f._aspectRatio &&
            _n == f._n &&
	        _f == f._f;
}

#endif
