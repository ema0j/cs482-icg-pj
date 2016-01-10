#ifndef _PLANE_H_
#define _PLANE_H_

// INCLUDES ====================================================
#include "vec3.h"
#include "range3.h"
#include <assert.h>
#include "functions.h"

enum PlaneCullResult {
    CULL_INSIDE,
    CULL_OUTSIDE,
    CULL_INTERSECT,
};

enum PlaneHalfSpace {
    HALFSPACE_POSITIVE = 1,
    HALFSPACE_ONPLANE = 0,
    HALFSPACE_NEGATIVE = -1,
};

// TYPE DECLARATION ============================================
template<class T>
class Plane {
	// static data --------------------------------------
public:

	// data ---------------------------------------------
protected:
    Vec3<T> _Nn; T _d;

public:
	// Constructors -------------------------------------
	Plane(T a = 0, T b = 0, T c = 0, T d = 0);
	Plane(const Vec3<T>& P, const Vec3<T>& N);

	// Access ops ---------------------------------------

	// Comparison ops -----------------------------------
	int operator == (const Plane& v) const;
	int operator != (const Plane& v) const;

	// Binary ops ---------------------------------------
    T DistanceToPoint(const Vec3<T>& P) const;
    float DistanceToPoint(const Vec3f& P) const;
    PlaneHalfSpace HalfSpace(const Vec3<T>& P) const;
    PlaneHalfSpace HalfSpace(const Vec3f& P) const;


	// Assignment ops -----------------------------------
    void Set(const Vec3<T>& P, const Vec3<T>& N);
    void Set(const Vec3<T>& N, T d);

	// Matrix ops ---------------------------------------
    static PlaneCullResult CullSphere(const Plane plane[6], const Vec3<T>& c, T radius);
    static PlaneCullResult CullBox(const Plane plane[6], const Range3d& bbox);
};

template<class T> inline Plane<T>::Plane(T a , T b, T c, T d) {
    Vec3<T> N(a,b,c);
    Set(N,d);
}

template<class T> inline Plane<T>::Plane(const Vec3<T>& P, const Vec3<T>& N) {
    Set(P,N);
}

template<class T> inline int Plane<T>::operator == (const Plane<T>& v) const {
    return _Nn == v._Nn && _d == v._d;
}

template<class T> inline int Plane<T>::operator != (const Plane<T>& v) const {
    return ! operator ==(v);
}

template<class T> inline void Plane<T>::Set(const Vec3<T> &P, const Vec3<T> &N) {
    _Nn = N.GetNormalized();
    _d = (P % N) / N.GetLength();
}

template<class T> inline void Plane<T>::Set(const Vec3<T> &N, T d) {
    _Nn = N.GetNormalized();
    _d = d / N.GetLength();
}

template<class T> inline T Plane<T>::DistanceToPoint(const Vec3<T>& P) const {
    return _Nn % P + _d;
}

template<class T> inline float Plane<T>::DistanceToPoint(const Vec3f& P) const {
    return (float)DistanceToPoint(Vec3<T>(P));
}

template<class T> inline PlaneHalfSpace Plane<T>::HalfSpace(const Vec3<T>& P) const {
    T d = DistanceToPoint(P);
    if(d < 0) return HALFSPACE_NEGATIVE;
    else if(d > 0) return HALFSPACE_POSITIVE;
    else return HALFSPACE_ONPLANE;
}

template<class T> inline PlaneHalfSpace Plane<T>::HalfSpace(const Vec3f& P) const {
    return HalfSpace(Vec3<T>(P));
}

// from http://www.flipcode.com/articles/article_frustumculling.shtml
template<class T> inline PlaneCullResult Plane<T>::CullSphere(const Plane<T> plane[6], const Vec3<T>& c, T radius) {
	// calculate our distances to each of the planes
	for(int i = 0; i < 6; ++i) {

		// find the distance to this plane
		T dist = plane[i].DistanceToPoint(c);

		// if this distance is < -sphere.radius, we are outside
		if(dist < -radius)
			return CULL_OUTSIDE;

		// else if the distance is between +- radius, then we intersect
		if(fabs(dist) < radius)
            return CULL_INTERSECT;
	}

	// otherwise we are fully in view
    return CULL_INSIDE;
}

// from http://www.flipcode.com/articles/article_frustumculling.shtml
template<class T> inline PlaneCullResult Plane<T>::CullBox(const Plane<T> planes[6], const Range3d& bbox) {
    Vec3<T> corners[8];
	int totalIn = 0;

	// get the corners of the box into the vCorner array
    bbox.GetCorners(corners);

	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
    for(int p = 0; p < 6; ++p) {
		int inCount = 8;
		int ptIn = 1;

		for(int i = 0; i < 8; ++i) {

			// test this point against the planes
			if(planes[p].HalfSpace(corners[i]) == HALFSPACE_NEGATIVE) {
			    ptIn = 0;
				--inCount;
			}
		}

		// were all the points outside of plane p?
		if(inCount == 0)
			return CULL_OUTSIDE;

		// check if they were all on the right side of the plane
		totalIn += ptIn;
	}

	// so if iTotalIn is 6, then all are inside the view
	if(totalIn == 6)
        return CULL_INSIDE;

	// we must be partly in then otherwise
	return CULL_INTERSECT;
}

typedef Plane<double> Planed;
typedef Plane<float> Planef;

#endif
