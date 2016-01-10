#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include <scene/smath.h>

class Material;

struct Intersection {
    Intersection() : t(0.0f), m(NULL), rayEpsilon(0.0f) {} 
    float t;
    DifferentialGeometry dp;
    Material* m;
    float rayEpsilon;

    void Transform(const Matrix4d& m) {
        dp.Transform(m);
    }
};

#endif
