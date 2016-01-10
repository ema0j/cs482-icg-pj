#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

// INCLUDES ====================================================
#include "frame.h"
#include "projection.h"
#include "consts.h"

// The frustum represnt a typical frustum of vision useful to render
// from a perspective or orthographic camera.
// The data are stored using the camera frame of reference and a projection.

// TYPE DECLARATION ============================================
template<class T>
class Frustum {
	// static data --------------------------------------
public:

	// data ---------------------------------------------
protected:
	Frame<T>				_frame;			// Frame of refernce
	Projection<T>			_proj;			// Projection

public:
	// Constructors -------------------------------------
	Frustum(const Frame<T>& frame = Frame<T>(),
			const Projection<T>& proj = Projection<T>());
    Frustum(const Frustum& f);

	// Setters ------------------------------------------
	void SetFrame(const Frame<T>& frame);
	void SetProjection(const Projection<T>& proj);

	// Getters ------------------------------------------
	Projection<T> GetProjection() const;
	Frame<T> GetFrame() const;

	// Matrix computation -------------------------------
	Matrix4<T> ComputeViewMatrixGL() const;
	Matrix4<T> ComputeProjectionMatrixGL() const;

	// Conversions --------------------------------------

	// Access ops ---------------------------------------

	// Comparison ops -----------------------------------
    bool operator==(const Frustum& f);

	// Assignment ops -----------------------------------
    Frustum& operator=(const Frustum& f);
};

// IMPLEMENTATION ==============================================
// Constructors -------------------------------------
template<class T> inline Frustum<T>::Frustum(const Frame<T>& frame,
						const Projection<T>& proj) {
    _frame = frame;
	_proj = proj;
}

template<class T> inline Frustum<T>::Frustum(const Frustum<T>& f) {
    _frame = f._frame;
    _proj = f._proj;
}

// Setters ------------------------------------------
template<class T> inline void Frustum<T>::SetFrame(const Frame<T>& frame) {
	_frame = frame;
}

template<class T> inline void Frustum<T>::SetProjection(const Projection<T>& proj) {
	_proj = proj;
}

// Getters ------------------------------------------
template<class T> inline Projection<T> Frustum<T>::GetProjection() const {
	return _proj;
}

template<class T> inline Frame<T> Frustum<T>::GetFrame() const {
	return _frame;
}

// Matrix computation -------------------------------
template<class T> inline Matrix4<T> Frustum<T>::ComputeViewMatrixGL() const {
	return (Matrix4d)_frame;
}

template<class T> inline Matrix4<T> Frustum<T>::ComputeProjectionMatrixGL() const {
	return (Matrix4d)_proj;
}

// Comparison ops -----------------------------------
template<class T> inline bool Frustum<T>::operator==(const Frustum<T>& f) {
    return _frame == f._frame && _proj == f._proj;
}

// Assignment ops -----------------------------------
template<class T> inline Frustum<T>& Frustum<T>::operator=(const Frustum<T>& f) {
    _frame = f._frame;
    _proj = f._proj;
    return *this;
}

#endif
