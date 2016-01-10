#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <cmath>
#include <algorithm>
#include <limits>
#include "consts.h"
#include <stdint.h>

using std::min;
using std::max;

inline float toRadians(float degree) {
    return degree * PIf / 180.0f;
}

inline double toRadians(double degree) {
    return degree * PI / 180.0;
}

inline float toDegrees(float radians) {
    return radians * 180.0f / PIf;
}

inline double toDegrees(double radians) {
    return radians * 180.0 / PI;
}

template<class T>
inline T clamp(T x, T m, T M) {
    return max( min (x,M), m);
}

template<class T>
inline T clampmin(T x, T m) {
    return max(x, m);
}

template<class T>
inline T smoothstep(T x, T m, T M) {
	T v = clamp((x - m) / (M - m), (T)0, (T)1);
	return v * v * ((T)-2 * v  + (T)3);
}


inline bool isPow2(uint64_t v) {
    return (v & (v-1)) == 0;
}

inline int roundUpPow2(uint32_t v) {
    v --;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	return v+1;    
}

inline int pow2(int n) { return 1 << n; }

// equal with tolerance
template<typename T>
inline bool eq(T a, T b, T t) {
    return fabs(a-b) < t;
}

#endif

