#ifndef _INTERSECTIONMETHODS_H_
#define _INTERSECTIONMETHODS_H_

#include <scene/smath.h>
#include "intersectionMethods.h"

// RAY-PRIMITIVE HIT TESTS
// -------------------------------------------------------------------------

inline bool IntersectBoundingBox(const Range3f& bbox, const Ray& ray, float& tMin, float& tMax) {
    tMin = ray.tMin; tMax = ray.tMax;

    Vec3f invRayDir = Vec3f(1) / ray.D;
    Vec3f tNear = (bbox.GetMin() - ray.E) * invRayDir;
    Vec3f tFar = (bbox.GetMax() - ray.E) * invRayDir;
    if(tNear[0] > tFar[0]) { float aux = tNear[0]; tNear[0] = tFar[0]; tFar[0] = aux; }
    if(tNear[1] > tFar[1]) { float aux = tNear[1]; tNear[1] = tFar[1]; tFar[1] = aux; }
    if(tNear[2] > tFar[2]) { float aux = tNear[2]; tNear[2] = tFar[2]; tFar[2] = aux; }

    tMin = (tNear[0] > tMin) ? tNear[0] : tMin;
    tMax = (tFar[0] < tMax) ? tFar[0] : tMax;
    if(tMin > tMax) return false;

    tMin = (tNear[1] > tMin) ? tNear[1] : tMin;
    tMax = (tFar[1] < tMax) ? tFar[1] : tMax;
    if(tMin > tMax) return false;

    tMin = (tNear[2] > tMin) ? tNear[2] : tMin;
    tMax = (tFar[2] < tMax) ? tFar[2] : tMax;
    if(tMin > tMax) return false;

    return true;
}

inline bool IntersectSphere(const Vec3f& center, float radius, const Ray& ray, float* t, float* rayEpsilon) {
    float a = ray.D.GetLengthSqr();
    float b = 2 * (ray.D % (ray.E - center));
    float c = (ray.E - center).GetLengthSqr() - radius*radius;

    float d = b*b - 4*a*c;
    if(d <= 0.0f) return false;

    float tm = (-b - sqrt(d)) / (2*a);
    float tM = (-b + sqrt(d)) / (2*a);

    if (tm > ray.tMin && tm < ray.tMax) {
      *t = tm;
    } else if (tM > ray.tMin && tM < ray.tMax) {
      *t = tM;
    } else {
      return false;
    }

    *rayEpsilon = 5e-4f * *t;
    return true;
}

// from pbrt
inline bool IntersectTriangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Ray& ray, float* t, float* b1, float* b2, float* rayEpsilon) {
    // setup
    Vec3f e1 = v1-v0;
    Vec3f e2 = v2-v0;
    Vec3f s1 = ray.D ^ e2;
    float divisor = s1 % e1;
    if(divisor == 0) return false;
    float invDivisor = 1/divisor;

    // first baricentric coord
    Vec3f d = ray.E - v0;
    *b1 = (d % s1) * invDivisor;
    if(*b1 < 0 || *b1 > 1) return false;

    // second baricentric coord
    Vec3f s2 = d ^ e1;
    *b2 = (ray.D % s2) * invDivisor;
    if(*b2 < 0 || *b1 + *b2 > 1) return false;

    // t
    *t = (e2 % s2) * invDivisor;
    if(*t < ray.tMin || *t > ray.tMax) return false;

    *rayEpsilon =  1e-3f * *t;

    return true;
}

// cylinder with no caps
inline bool IntersectHairSegment( const Vec3f& p0, const Vec3f& p1, float radius, const Ray& ray, float* t, float* u, float* rayEpsilon )
{
    bool hitCylinder = false;
    float bestCylT = FLT_MAX;

    Vec3f axis = p1 - p0;
    float height = axis.GetLength();
    axis.Normalize();

    Vec3f RC = ray.E - p0;
    Vec3f n = ray.D ^ axis;
    float ln = n.GetLength();
    n.Normalize();

    float d = fabs(n % RC);
    Vec3f O;
    bool hit = (d <= radius);

    if(!hit) return false;

    O = RC ^ axis;
    float tt = - (O % n) / ln;
    O = n ^ axis;
    O.Normalize();
    float s = (float)fabsf(sqrtf(radius * radius - d * d) / (O % ray.D));

    float t0 = tt - s;
    float t1 = tt + s;

    //check for valid intersections: > tstart, within finite cylinder
    if(t0 > ray.tMin && t0 < ray.tMax) { //point is on ray
        O = ray.Eval(t0) - p0;
        ln = O % axis;
        if(ln > 0 && ln < height) {//point is on finite cylinder
            bestCylT = t0;
            hitCylinder = true;
        }
    }

    if(t1 > ray.tMin && t1 < ray.tMax && !hitCylinder /*&& !ray.cullBackface*/) { //point is on ray
        O = ray.Eval(t1) - p0;
        ln = O % axis;
        if(ln > 0 && ln < height) {//point is on finite cylinder
            bestCylT = t1;
            hitCylinder = true;
        }
    }

    if(!hitCylinder) return false;

    RC = ray.Eval(bestCylT);
    n = axis * (-ln) + O;
    n = n * (1.0f / radius);
    if(n % ray.D > 0) return false;

    float a = ln / height;

    *u = a;
    *t = bestCylT;

    *rayEpsilon = 5e-4f * *t;

    return true;
}

inline bool IntersectHairSegmentThinned(const Vec3f& p0, const Vec3f& p1,
                                        float radius0, float radius1,
                                        const Ray& ray, float* t, float* u) {
    bool hitCylinder = false;
    float bestCylT = FLT_MAX;

    Vec3f axis = p1 - p0;
    float height = axis.GetLength();
    axis.Normalize();

    Vec3f RC = ray.E - p0;
    Vec3f n = ray.D ^ axis;
    float ln = n.GetLength();
    n.Normalize();

    float d = fabs(n % RC);
    Vec3f O;
    float radius = (1 - ln/height) * radius0 + (ln/height)*radius1;
    bool hit = (d <= radius);

    if(!hit) return false;

    O = RC ^ axis;
    float tt = - (O % n) / ln;
    O = n ^ axis;
    O.Normalize();
    float s = (float)fabsf(sqrtf(radius * radius - d * d) / (O % ray.D));

    float t0 = tt - s;
    float t1 = tt + s;

    //check for valid intersections: > tstart, within finite cylinder
    if(t0 > ray.tMin && t0 < ray.tMax) { //point is on ray
        O = ray.Eval(t0) - p0;
        ln = O % axis;
        if(ln > 0 && ln < height) {//point is on finite cylinder
            bestCylT = t0;
            hitCylinder = true;
        }
    }

    if(t1 > ray.tMin && t1 < ray.tMax && !hitCylinder /*&& !ray.cullBackface*/) { //point is on ray
        O = ray.Eval(t1) - p0;
        ln = O % axis;
        if(ln > 0 && ln < height) {//point is on finite cylinder
            bestCylT = t1;
            hitCylinder = true;
        }
    }

    if(!hitCylinder) return false;

    RC = ray.Eval(bestCylT);
    n = axis * (-ln) + O;
    n = n * (1.0f / radius);
    if(n % ray.D > 0) return false;

    float a = ln / height;

    *u = a;
    *t = bestCylT;

    return true;
}

// DEAD CODE
// -------------------------------------------------------------------------

/*
        // simplified cylinder intersection from skar intersection
inline bool IntersectHairSegmentSimpl(Ray ray, ref float tt, Vec3 p0, Vec3 p1, float radius) {
    bool hitCylinder = false;
    float bestCylT = Single.MaxValue;

    Vec3 axis = p1 - p0;
    float height = axis.GetLength();
    axis.Normalize();

    Vec3 RC = ray.origin - p0;
    Vec3 n = ray.direction ^ axis;
    float ln = n.GetLength();
    n.Normalize();

    float d = Math.Abs(n % RC);
    Vec3 O = new Vec3();
    bool hit = (d <= radius);

    if(!hit) return false;

    O = RC ^ axis;
    float t = -(O % n) / ln;
    O = n ^ axis;
    O.Normalize();
    float s = (float)Math.Abs(Math.Sqrt(radius * radius - d * d) / (O % ray.direction));

    float t0 = t - s;
    float t1 = t + s;

    //check for valid intersections: > tstart, within finite cylinder
    if(t0 > ray.tMin && t0 < ray.tMax) { //point is on ray
        O = ray.Evaluate(t0) - p0;
        ln = O % axis;
        if(ln > 0 && ln < height) {//point is on finite cylinder
            bestCylT = t0;
            hitCylinder = true;
        }
    }

    if(t1 > ray.tMin && t1 < ray.tMax && !hitCylinder && !ray.cullBackface) { //point is on ray
        O = ray.Evaluate(t1) - p0;
        ln = O % axis;
        if(ln > 0 && ln < height) {//point is on finite cylinder
            bestCylT = t1;
            hitCylinder = true;
        }
    }

    if(!hitCylinder) return false;

    RC = ray.Evaluate(bestCylT);
    n = (-ln) * axis + O;
    n = n * (1.0f / radius);
    if(n % ray.direction > 0) return false;

    tt = bestCylT;

    return true;
}
*/

#endif
