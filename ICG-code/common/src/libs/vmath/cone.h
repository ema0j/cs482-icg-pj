#ifndef _CONET_H_
#define _CONET_H_

// INCLUDES ====================================================
#include "vec3.h"

template<class T>
class Cone {
    // data ---------------------------------------------
protected:
    Vec3<T> _axis;
    float	_angle;
public:
    // Constructors -------------------------------------
    Cone(const Vec3<T>& ax = Vec3<T>::Z(), T angle = 0.0);
    Cone(const Cone<T>& v);

    // Access ops ---------------------------------------
    Vec3<T>		GetAxis() const;
    T		    GetAngle() const;
    Vec3<T>&	GetAxisRef();
    T&		    GetAngleRef();
    void		SetAxis(const Vec3<T>& ax);
    void		SetAngle(const T ca);
    void		Set(const Vec3<T>& ax, const T ca);
    bool		IsValid() const;

    // Comparison ops -----------------------------------
    int operator == (const Cone& v) const;
    int operator != (const Cone& v) const;

    // Binary ops ---------------------------------------

    // Test ops -----------------------------------------
    bool Contain(const Vec3<T>& v) const;

    // Assignment ops -----------------------------------
    Cone& operator=(const Cone& v);

    // Vector ops ---------------------------------------
    void Grow(const Vec3<T>& v);
    void Grow(const Cone& b);
    void ApproxGrow(const Cone& b);
    //Vec3<T> Sample(float u1, float u2) const;
    static bool Overlap(const Cone& a, const Cone& b);
    static Cone Union(const Cone& a, const Cone& b);
    static Cone ApproxUnion(const Cone& a, const Cone& b);

    bool IsValid() { return (!_axis.IsZero()) && _angle < HALF_PIf; }
};

// MACROS USEFUL FOR CODE GENERATION ===========================
template <class T>
inline Cone<T>::Cone(const Vec3<T>& ax, T angle) {
    _axis = ax.GetNormalized(); _angle = angle;
}

template <class T>
inline Cone<T>::Cone(const Cone<T>& v) {
    _axis = v._axis; _angle = v._angle;
}

//template <class T>
//Vec3<T> Cone<T>::Sample(float u1, float u2) const
//{
//	Vec3<T> p;
//	return p;
//}

template <class T>
inline Vec3<T> Cone<T>::GetAxis() const { return _axis; }
template <class T>
inline T Cone<T>::GetAngle() const { return _angle; }
template <class T>
inline Vec3<T>& Cone<T>::GetAxisRef() { return _axis; }
template <class T>
inline T& Cone<T>::GetAngleRef() { return _angle; }
template <class T>
inline void Cone<T>::SetAxis(const Vec3<T>& ax) { _axis = ax; }
template <class T>
inline void Cone<T>::SetAngle(const T ca) { _angle = ca; }
template <class T>
inline void Cone<T>::Set(const Vec3<T>& ax, const T ca) { _axis = ax; _angle = ca; }
template <class T>
inline bool Cone<T>::IsValid() const { return (!_axis.IsZero()) &&  (_angle >= 0.0f) && (_angle < HALF_PIf); }

template <class T>
inline int Cone<T>::operator == (const Cone<T>& v) const {
    return _axis == v._axis && _angle == v._angle; 
}
template <class T>
inline int Cone<T>::operator != (const Cone<T>& v) const {
    return !operator==(v); 
}

template <class T>
inline bool Cone<T>::Contain(const Vec3<T>& v) const {
    return (v % _axis >= _angle);
}

template <class T>
inline Cone<T>& Cone<T>::operator=(const Cone<T>& v) {
    _axis = v._axis;
    _angle = v._angle;
    return *this;
}

template <class T>
inline void Cone<T>::Grow(const Vec3<T>& v) {
    Grow(Cone<T>(v));
}

template <class T>
inline void Cone<T>::Grow(const Cone<T>& b) {
    *this = Union(*this, b);
}

template <class T>
inline void Cone<T>::ApproxGrow(const Cone<T>& b) {
    *this = ApproxUnion(*this, b);
}

template <class T>
inline Cone<T> Cone<T>::ApproxUnion(const Cone<T>& a, const Cone<T>& b) {
    if (!a.IsValid() || !b.IsValid())
    {
        // invalid 
        return Cone<T>(Vec3<T>::Zero(), HALF_PIf);
    }
    // define variables to store the results
    T angle;
    Vec3<T> axis;
    Cone<T> result;

    // define min and max - Angle and Axis, assuming this is the bigger cone
    T maxAngle = a.GetAngle();
    T minAngle = b.GetAngle();
    Vec3<T> maxAxis = a.GetAxis();
    Vec3<T> minAxis = b.GetAxis();
    // swap min and max - Angle and Axis if they are not correct
    if (maxAngle < minAngle) {
        maxAngle = minAngle;
        minAngle = a.GetAngle();
        maxAxis = minAxis;
        minAxis = a.GetAxis();
    }

    // calculate the angle between the axes
    T gapAngle = acos(a.GetAxis() % b.GetAxis());

    // check if the bigger normal cone completely contains the smaller cone
    if (maxAngle >= gapAngle + minAngle) {
        // simply adopt the bigger normal cone
        angle = maxAngle;
        axis = maxAxis;
    } else {
        // approximate the new axis by the one in the middle of the old axes
        axis = maxAxis + minAxis;
        if (!axis.IsZero())
        {
            axis.Normalize();
        }
        // calculate an upper bound for the angle
        angle = gapAngle/2 + maxAngle;
    }

    result.SetAxis( axis );
    result.SetAngle( angle );

    return result;
}


template <class T>
inline Cone<T> Cone<T>::Union(const Cone<T>& a, const Cone<T>& b) {
    if (!a.IsValid() || !b.IsValid())
    {
        // invalid 
        return Cone<T>(Vec3<T>::Zero(), HALF_PIf);
    }
    // define variables to store the results
    T angle;
    Vec3<T> axis;
    Cone<T> result;

    // define min and max - Angle and Axis, assuming this is the bigger cone
    T maxAngle = a.GetAngle();
    T minAngle = b.GetAngle();
    Vec3<T> maxAxis = a.GetAxis();
    Vec3<T> minAxis = b.GetAxis();
    // swap min and max - Angle and Axis if they are not correct
    if (maxAngle < minAngle) {
        maxAngle = minAngle;
        minAngle = a.GetAngle();
        maxAxis = minAxis;
        minAxis = a.GetAxis();
    }

    // calculate the angle between the axes
    T gapAngle = acos(a.GetAxis() % b.GetAxis());

    // check if the bigger normal cone completely contains the smaller cone
    if (maxAngle >= gapAngle + minAngle) {
        // simply adopt the bigger normal cone
        angle = maxAngle;
        axis = maxAxis;
    } else {

        // calculate a new normal cones' angle enclosing both original cones
        angle = (minAngle + maxAngle + gapAngle) / 2;
        if (angle >= HALF_PIf)
        {
            axis = Vec3f::Zero();
            angle = HALF_PIf;
        }
        else
        {
            // use a presolved equation based on:
            // cos(angle - maxAngle) = axis . maxAxis / |axis|
            // with:    axis = maxAxis + x minAxis
            // determine "some" temporary values for "simplification"
            T cosg = cos(angle - maxAngle);
            T cosg_2 = cosg * cosg;
            T b_1 = maxAxis.x;
            T b_2 = maxAxis.y;
            T b_3 = maxAxis.z;
            T b_1_2 = b_1 * b_1;
            T b_2_2 = b_2 * b_2;
            T b_3_2 = b_3 * b_3;
            T b_1_3 = b_1_2 * b_1;
            T b_2_3 = b_2_2 * b_2;
            T b_3_3 = b_3_2 * b_3;
            T b_1_4 = b_1_3 * b_1;
            T b_2_4 = b_2_3 * b_2;
            T b_3_4 = b_3_3 * b_3;
            T d_1 = minAxis.x;
            T d_2 = minAxis.y;
            T d_3 = minAxis.z;
            T d_1_2 = d_1 * d_1;
            T d_2_2 = d_2 * d_2;
            T d_3_2 = d_3 * d_3;
            // calculate x
            T z = ((-b_2_2 -b_1_2)*d_3_2 +
                (2*b_2*b_3*d_2 + 2*b_1*b_3*d_1)*d_3 +
                (-b_3_2 -b_1_2)*d_2_2 +
                2*b_1*b_2*d_1*d_2 +
                (-b_3_2 -b_2_2)*d_1_2)*cosg_2 +
                ((b_2_2 + b_1_2)*b_3_2 +
                b_2_4 + 2*b_1_2*b_2_2 + b_1_4)*d_3_2 +
                (((-2*b_2_3 -2*b_1_2*b_2)*b_3 -
                2*b_2*b_3_3)*d_2 +
                ((-2*b_1*b_2_2 - 2*b_1_3)*b_3 -
                2*b_1*b_3_3)*d_1)*d_3 +
                (b_3_4 + (b_2_2 + 2*b_1_2)*b_3_2 +
                b_1_2*b_2_2 + b_1_4)*d_2_2 +
                (-2*b_1*b_2*b_3_2 - 2*b_1*b_2_3 -
                2*b_1_3*b_2)*d_1*d_2 +
                (b_3_4 +(2*b_2_2 + b_1_2)*b_3_2 +
                b_2_4 + b_1_2*b_2_2)*d_1_2;
            z = max((T)0.0, z);
            T x = (cosg * sqrt(z) +
                (-b_3*d_3 - b_2*d_2 - b_1*d_1)*cosg_2 +
                (b_3_3 + (b_2_2 + b_1_2)*b_3)*d_3 +
                (b_2*b_3_2 + b_2_3 + b_1_2*b_2)*d_2 +
                (b_1*b_3_2 + b_1*b_2_2 + b_1_3)*d_1) /
                ((d_3_2 + d_2_2 + d_1_2)*cosg_2 - b_3_2*d_3_2 +
                (-2*b_2*b_3*d_2 - 2*b_1*b_3*d_1)*d_3 -
                b_2_2*d_2_2 -2*b_1*b_2*d_1*d_2 -
                b_1_2*d_1_2);
            assert(x <= FLT_MAX && x >= -FLT_MAX);
            // calculate the new axis
            axis = maxAxis + (minAxis * x);
            if (!axis.IsZero())
            {
                axis.Normalize();
            }
        }
    }

    result.SetAxis( axis );
    result.SetAngle( angle );

    return result;
}

//template <class T>
//inline bool Cone<T>::Overlap(const Cone<T>& a, const Cone<T>& b) {
//
//}

typedef Cone<float> Conef;
typedef Cone<double> Coned;

#endif
