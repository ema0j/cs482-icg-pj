/*
 * Copyright (c) 1993 - 2009 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*
    This file implements common mathematical operations on vector types
    (float3, float4 etc.) since these are not provided as standard by CUDA.

    The syntax is modelled on the Cg standard library.

    This file has also been modified from the original cutil_math.h file.
    cutil_math.h is a subset of this file, and you should use this file in place
    of any cutil_math.h file you wish to use.
*/

#ifndef CUTIL_MATH_H
#define CUTIL_MATH_H

#include "internal/optix_declarations.h"  // For RT_HOSTDEVICE
#include <vector_functions.h> // from cuda toolkit

// #define these constants such that we are sure
// 32b floats are emitted in ptx
#ifndef M_Ef
#define M_Ef        2.71828182845904523536f
#endif
#ifndef M_LOG2Ef
#define M_LOG2Ef    1.44269504088896340736f
#endif
#ifndef M_LOG10Ef
#define M_LOG10Ef   0.434294481903251827651f
#endif
#ifndef M_LN2f
#define M_LN2f      0.693147180559945309417f
#endif
#ifndef M_LN10f
#define M_LN10f     2.30258509299404568402f
#endif
#ifndef M_PIf
#define M_PIf       3.14159265358979323846f
#endif
#ifndef M_PI_2f
#define M_PI_2f     1.57079632679489661923f
#endif
#ifndef M_PI_4f
#define M_PI_4f     0.785398163397448309616f
#endif
#ifndef M_1_PIf
#define M_1_PIf     0.318309886183790671538f
#endif
#ifndef M_2_PIf
#define M_2_PIf     0.636619772367581343076f
#endif
#ifndef M_2_SQRTPIf
#define M_2_SQRTPIf 1.12837916709551257390f
#endif
#ifndef M_SQRT2f
#define M_SQRT2f    1.41421356237309504880f
#endif
#ifndef M_SQRT1_2f
#define M_SQRT1_2f  0.707106781186547524401f
#endif

/******************************************************************************/
typedef unsigned int uint;
typedef unsigned short ushort;

#ifndef __CUDACC__
#include <math.h>

inline float fminf(float a, float b)
{
  return a < b ? a : b;
}

inline float fmaxf(float a, float b)
{
  return a > b ? a : b;
}

inline int max(int a, int b)
{
  return a > b ? a : b;
}

inline int min(int a, int b)
{
  return a < b ? a : b;
}
#endif

/* float functions */
/******************************************************************************/

/* copy sign-bit from src value to dst value */
static __inline__ RT_HOSTDEVICE float copy_sign(float dst, float src)
{
  union {
    float f;
    unsigned int i;
  } v1, v2, v3;
  v1.f = src;
  v2.f = dst;
  v3.i = (v2.i & 0x7fffffff) | (v1.i & 0x80000000);

  return v3.f;
}

/* lerp */
inline RT_HOSTDEVICE float lerp(float a, float b, float t)
{
    return a + t*(b-a);
}

/* clamp */
inline RT_HOSTDEVICE float clamp(float f, float a, float b)
{
    return fmaxf(a, fminf(f, b));
}

/* int2 functions */
/******************************************************************************/

/* negate */
inline RT_HOSTDEVICE int2 operator-(int2 &a)
{
    return make_int2(-a.x, -a.y);
}

/* addition */
inline RT_HOSTDEVICE int2 operator+(int2 a, int2 b)
{
    return make_int2(a.x + b.x, a.y + b.y);
}
inline RT_HOSTDEVICE void operator+=(int2 &a, int2 b)
{
    a.x += b.x; a.y += b.y;
}

/* subtract */
inline RT_HOSTDEVICE int2 operator-(int2 a, int2 b)
{
    return make_int2(a.x - b.x, a.y - b.y);
}
inline RT_HOSTDEVICE int2 operator-(int2 a, int b)
{
    return make_int2(a.x - b, a.y - b);
}
inline RT_HOSTDEVICE void operator-=(int2 &a, int2 b)
{
    a.x -= b.x; a.y -= b.y;
}

/* multiply */
inline RT_HOSTDEVICE int2 operator*(int2 a, int2 b)
{
    return make_int2(a.x * b.x, a.y * b.y);
}
inline RT_HOSTDEVICE int2 operator*(int2 a, int s)
{
    return make_int2(a.x * s, a.y * s);
}
inline RT_HOSTDEVICE int2 operator*(int s, int2 a)
{
    return make_int2(a.x * s, a.y * s);
}
inline RT_HOSTDEVICE void operator*=(int2 &a, int s)
{
    a.x *= s; a.y *= s;
}

/* float2 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE float2 make_float2(float s)
{
    return make_float2(s, s);
}
inline RT_HOSTDEVICE float2 make_float2(int2 a)
{
    return make_float2(float(a.x), float(a.y));
}

/* negate */
inline RT_HOSTDEVICE float2 operator-(float2 &a)
{
    return make_float2(-a.x, -a.y);
}

/* addition */
inline RT_HOSTDEVICE float2 operator+(float2 a, float2 b)
{
    return make_float2(a.x + b.x, a.y + b.y);
}
inline RT_HOSTDEVICE float2 operator+(float2 a, float b)
{
    return make_float2(a.x + b, a.y + b);
}
inline RT_HOSTDEVICE float2 operator+(float a, float2 b)
{
    return make_float2(a + b.x, a + b.y);
}
inline RT_HOSTDEVICE void operator+=(float2 &a, float2 b)
{
    a.x += b.x; a.y += b.y;
}

/* subtract */
inline RT_HOSTDEVICE float2 operator-(float2 a, float2 b)
{
    return make_float2(a.x - b.x, a.y - b.y);
}
inline RT_HOSTDEVICE float2 operator-(float2 a, float b)
{
    return make_float2(a.x - b, a.y - b);
}
inline RT_HOSTDEVICE float2 operator-(float a, float2 b)
{
    return make_float2(a - b.x, a - b.y);
}
inline RT_HOSTDEVICE void operator-=(float2 &a, float2 b)
{
    a.x -= b.x; a.y -= b.y;
}

/* multiply */
inline RT_HOSTDEVICE float2 operator*(float2 a, float2 b)
{
    return make_float2(a.x * b.x, a.y * b.y);
}
inline RT_HOSTDEVICE float2 operator*(float2 a, float s)
{
    return make_float2(a.x * s, a.y * s);
}
inline RT_HOSTDEVICE float2 operator*(float s, float2 a)
{
    return make_float2(a.x * s, a.y * s);
}
inline RT_HOSTDEVICE void operator*=(float2 &a, float2 s)
{
    a.x *= s.x; a.y *= s.y;
}
inline RT_HOSTDEVICE void operator*=(float2 &a, float s)
{
    a.x *= s; a.y *= s;
}

/* divide */
inline RT_HOSTDEVICE float2 operator/(float2 a, float2 b)
{
    return make_float2(a.x / b.x, a.y / b.y);
}
inline RT_HOSTDEVICE float2 operator/(float2 a, float s)
{
    float inv = 1.0f / s;
    return a * inv;
}
inline RT_HOSTDEVICE float2 operator/(float s, float2 a)
{
    return make_float2( s/a.x, s/a.y );
}
inline RT_HOSTDEVICE void operator/=(float2 &a, float s)
{
    float inv = 1.0f / s;
    a *= inv;
}

/* lerp */
inline RT_HOSTDEVICE float2 lerp(float2 a, float2 b, float t)
{
    return a + t*(b-a);
}

/* clamp */
inline RT_HOSTDEVICE float2 clamp(float2 v, float a, float b)
{
    return make_float2(clamp(v.x, a, b), clamp(v.y, a, b));
}

inline RT_HOSTDEVICE float2 clamp(float2 v, float2 a, float2 b)
{
    return make_float2(clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y));
}

/* dot product */
inline RT_HOSTDEVICE float dot(float2 a, float2 b)
{
    return a.x * b.x + a.y * b.y;
}

/* length */
inline RT_HOSTDEVICE float length(float2 v)
{
    return sqrtf(dot(v, v));
}

/* normalize */
inline RT_HOSTDEVICE float2 normalize(float2 v)
{
    float invLen = 1.0f / sqrtf(dot(v, v));
    return v * invLen;
}

/* floor */
inline RT_HOSTDEVICE float2 floor(const float2 v)
{
    return make_float2(floor(v.x), floor(v.y));
}

/* reflect */
inline RT_HOSTDEVICE float2 reflect(float2 i, float2 n)
{
	return i - 2.0f * n * dot(n,i);
}

/* faceforward */
inline RT_HOSTDEVICE float2 faceforward(float2 n, float2 i, float2 nref)
{
  return n * copy_sign( 1.0f, dot(i, nref) );
}

/* exp */
inline RT_HOSTDEVICE float2 expf(float2 v)
{
  return make_float2(expf(v.x), expf(v.y));
}


/* float3 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE float3 make_float3(float s)
{
    return make_float3(s, s, s);
}
inline RT_HOSTDEVICE float3 make_float3(float2 a)
{
    return make_float3(a.x, a.y, 0.0f);
}
inline RT_HOSTDEVICE float3 make_float3(float2 a, float s)
{
    return make_float3(a.x, a.y, s);
}
inline RT_HOSTDEVICE float3 make_float3(float4 a)
{
   return make_float3(a.x, a.y, a.z);  /* discards w */
}
inline RT_HOSTDEVICE float3 make_float3(int3 a)
{
    return make_float3(float(a.x), float(a.y), float(a.z));
}

/* negate */
inline RT_HOSTDEVICE float3 operator-(const float3 &a)
{
    return make_float3(-a.x, -a.y, -a.z);
}

/* min */
static __inline__ RT_HOSTDEVICE float3 fminf(float3 a, float3 b)
{
	return make_float3(fminf(a.x,b.x), fminf(a.y,b.y), fminf(a.z,b.z));
}

/* max */
static __inline__ RT_HOSTDEVICE float3 fmaxf(float3 a, float3 b)
{
	return make_float3(fmaxf(a.x,b.x), fmaxf(a.y,b.y), fmaxf(a.z,b.z));
}

/* addition */
inline RT_HOSTDEVICE float3 operator+(float3 a, float3 b)
{
    return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline RT_HOSTDEVICE float3 operator+(float3 a, float b)
{
    return make_float3(a.x + b, a.y + b, a.z + b);
}
inline RT_HOSTDEVICE float3 operator+(float a, float3 b)
{
    return make_float3(a + b.x, a + b.y, a + b.z);
}
inline RT_HOSTDEVICE void operator+=(float3 &a, float3 b)
{
    a.x += b.x; a.y += b.y; a.z += b.z;
}

/* subtract */
inline RT_HOSTDEVICE float3 operator-(float3 a, float3 b)
{
    return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline RT_HOSTDEVICE float3 operator-(float3 a, float b)
{
    return make_float3(a.x - b, a.y - b, a.z - b);
}
inline RT_HOSTDEVICE float3 operator-(float a, float3 b)
{
    return make_float3(a - b.x, a - b.y, a - b.z);
}
inline RT_HOSTDEVICE void operator-=(float3 &a, float3 b)
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z;
}

/* multiply */
inline RT_HOSTDEVICE float3 operator*(float3 a, float3 b)
{
    return make_float3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline RT_HOSTDEVICE float3 operator*(float3 a, float s)
{
    return make_float3(a.x * s, a.y * s, a.z * s);
}
inline RT_HOSTDEVICE float3 operator*(float s, float3 a)
{
    return make_float3(a.x * s, a.y * s, a.z * s);
}
inline RT_HOSTDEVICE void operator*=(float3 &a, float3 s)
{
    a.x *= s.x; a.y *= s.y; a.z *= s.z;
}
inline RT_HOSTDEVICE void operator*=(float3 &a, float s)
{
    a.x *= s; a.y *= s; a.z *= s;
}

/* divide */
inline RT_HOSTDEVICE float3 operator/(float3 a, float3 b)
{
    return make_float3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline RT_HOSTDEVICE float3 operator/(float3 a, float s)
{
    float inv = 1.0f / s;
    return a * inv;
}
inline RT_HOSTDEVICE float3 operator/(float s, float3 a)
{
    return make_float3( s/a.x, s/a.y, s/a.z );
}
inline RT_HOSTDEVICE void operator/=(float3 &a, float s)
{
    float inv = 1.0f / s;
    a *= inv;
}

/* lerp */
inline RT_HOSTDEVICE float3 lerp(float3 a, float3 b, float t)
{
    return a + t*(b-a);
}

/* clamp */
inline RT_HOSTDEVICE float3 clamp(float3 v, float a, float b)
{
    return make_float3(clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b));
}

inline RT_HOSTDEVICE float3 clamp(float3 v, float3 a, float3 b)
{
    return make_float3(clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y), clamp(v.z, a.z, b.z));
}

/* dot product */
inline RT_HOSTDEVICE float dot(float3 a, float3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/* cross product */
inline RT_HOSTDEVICE float3 cross(float3 a, float3 b)
{
    return make_float3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

/* length */
inline RT_HOSTDEVICE float length(float3 v)
{
    return sqrtf(dot(v, v));
}

/* normalize */
inline RT_HOSTDEVICE float3 normalize(float3 v)
{
    float invLen = 1.0f / sqrtf(dot(v, v));
    return v * invLen;
}

/* floor */
inline RT_HOSTDEVICE float3 floor(const float3 v)
{
    return make_float3(floor(v.x), floor(v.y), floor(v.z));
}

/* reflect */
inline RT_HOSTDEVICE float3 reflect(float3 i, float3 n)
{
	return i - 2.0f * n * dot(n,i);
}

/* faceforward */
inline RT_HOSTDEVICE float3 faceforward(float3 n, float3 i, float3 nref)
{
  return n * copy_sign( 1.0f, dot(i, nref) );
}

/* exp */
inline RT_HOSTDEVICE float3 expf(float3 v)
{
  return make_float3(expf(v.x), expf(v.y), expf(v.z));
}


/* float4 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE float4 make_float4(float s)
{
    return make_float4(s, s, s, s);
}
inline RT_HOSTDEVICE float4 make_float4(float3 a)
{
    return make_float4(a.x, a.y, a.z, 0.0f);
}
inline RT_HOSTDEVICE float4 make_float4(float3 a, float w)
{
    return make_float4(a.x, a.y, a.z, w);
}
inline RT_HOSTDEVICE float4 make_float4(int4 a)
{
    return make_float4(float(a.x), float(a.y), float(a.z), float(a.w));
}

/* negate */
inline RT_HOSTDEVICE float4 operator-(float4 &a)
{
    return make_float4(-a.x, -a.y, -a.z, -a.w);
}

/* min */
static __inline__ RT_HOSTDEVICE float4 fminf(float4 a, float4 b)
{
	return make_float4(fminf(a.x,b.x), fminf(a.y,b.y), fminf(a.z,b.z), fminf(a.w,b.w));
}

/* max */
static __inline__ RT_HOSTDEVICE float4 fmaxf(float4 a, float4 b)
{
	return make_float4(fmaxf(a.x,b.x), fmaxf(a.y,b.y), fmaxf(a.z,b.z), fmaxf(a.w,b.w));
}

/* addition */
inline RT_HOSTDEVICE float4 operator+(float4 a, float4 b)
{
    return make_float4(a.x + b.x, a.y + b.y, a.z + b.z,  a.w + b.w);
}
inline RT_HOSTDEVICE float4 operator+(float4 a, float b)
{
    return make_float4(a.x + b, a.y + b, a.z + b,  a.w + b);
}
inline RT_HOSTDEVICE float4 operator+(float a, float4 b)
{
    return make_float4(a + b.x, a + b.y, a + b.z,  a + b.w);
}
inline RT_HOSTDEVICE void operator+=(float4 &a, float4 b)
{
    a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;
}

/* subtract */
inline RT_HOSTDEVICE float4 operator-(float4 a, float4 b)
{
    return make_float4(a.x - b.x, a.y - b.y, a.z - b.z,  a.w - b.w);
}
inline RT_HOSTDEVICE float4 operator-(float4 a, float b)
{
    return make_float4(a.x - b, a.y - b, a.z - b,  a.w - b);
}
inline RT_HOSTDEVICE float4 operator-(float a, float4 b)
{
    return make_float4(a - b.x, a - b.y, a - b.z,  a - b.w);
}
inline RT_HOSTDEVICE void operator-=(float4 &a, float4 b)
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;
}

/* multiply */
inline RT_HOSTDEVICE float4 operator*(float4 a, float4 s)
{
    return make_float4(a.x * s.w, a.y * s.w, a.z * s.w, a.w * s.w);
}
inline RT_HOSTDEVICE float4 operator*(float4 a, float s)
{
    return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline RT_HOSTDEVICE float4 operator*(float s, float4 a)
{
    return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}
inline RT_HOSTDEVICE void operator*=(float4 &a, float4 s)
{
    a.x *= s.x; a.y *= s.y; a.z *= s.z; a.w *= s.w;
}
inline RT_HOSTDEVICE void operator*=(float4 &a, float s)
{
    a.x *= s; a.y *= s; a.z *= s; a.w *= s;
}

/* divide */
inline RT_HOSTDEVICE float4 operator/(float4 a, float4 b)
{
    return make_float4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}
inline RT_HOSTDEVICE float4 operator/(float4 a, float s)
{
    float inv = 1.0f / s;
    return a * inv;
}
inline RT_HOSTDEVICE float4 operator/(float s, float4 a)
{
    return make_float4( s/a.x, s/a.y, s/a.z, s/a.w );
}
inline RT_HOSTDEVICE void operator/=(float4 &a, float s)
{
    float inv = 1.0f / s;
    a *= inv;
}

/* lerp */
inline RT_HOSTDEVICE float4 lerp(float4 a, float4 b, float t)
{
    return a + t*(b-a);
}

/* clamp */
inline RT_HOSTDEVICE float4 clamp(float4 v, float a, float b)
{
    return make_float4(clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b), clamp(v.w, a, b));
}

inline RT_HOSTDEVICE float4 clamp(float4 v, float4 a, float4 b)
{
    return make_float4(clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y), clamp(v.z, a.z, b.z), clamp(v.w, a.w, b.w));
}

/* dot product */
inline RT_HOSTDEVICE float dot(float4 a, float4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/* length */
inline RT_HOSTDEVICE float length(float4 r)
{
    return sqrtf(dot(r, r));
}

/* normalize */
inline RT_HOSTDEVICE float4 normalize(float4 v)
{
    float invLen = 1.0f / sqrtf(dot(v, v));
    return v * invLen;
}

/* floor */
inline RT_HOSTDEVICE float4 floor(const float4 v)
{
    return make_float4(floor(v.x), floor(v.y), floor(v.z), floor(v.w));
}

/* reflect */
inline RT_HOSTDEVICE float4 reflect(float4 i, float4 n)
{
	return i - 2.0f * n * dot(n,i);
}

/* faceforward */
inline RT_HOSTDEVICE float4 faceforward(float4 n, float4 i, float4 nref)
{
  return n * copy_sign( 1.0f, dot(i, nref) );
}

/* exp */
inline RT_HOSTDEVICE float4 expf(float4 v)
{
  return make_float4(expf(v.x), expf(v.y), expf(v.z), expf(v.w));
}

/* int3 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE int3 make_int3(int s)
{
    return make_int3(s, s, s);
}
inline RT_HOSTDEVICE int3 make_int3(float3 a)
{
    return make_int3(int(a.x), int(a.y), int(a.z));
}

/* negate */
inline RT_HOSTDEVICE int3 operator-(int3 &a)
{
    return make_int3(-a.x, -a.y, -a.z);
}

/* min */
inline RT_HOSTDEVICE int3 min(int3 a, int3 b)
{
    return make_int3(min(a.x,b.x), min(a.y,b.y), min(a.z,b.z));
}

/* max */
inline RT_HOSTDEVICE int3 max(int3 a, int3 b)
{
    return make_int3(max(a.x,b.x), max(a.y,b.y), max(a.z,b.z));
}

/* addition */
inline RT_HOSTDEVICE int3 operator+(int3 a, int3 b)
{
    return make_int3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline RT_HOSTDEVICE void operator+=(int3 &a, int3 b)
{
    a.x += b.x; a.y += b.y; a.z += b.z;
}

/* subtract */
inline RT_HOSTDEVICE int3 operator-(int3 a, int3 b)
{
    return make_int3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline RT_HOSTDEVICE void operator-=(int3 &a, int3 b)
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z;
}

/* multiply */
inline RT_HOSTDEVICE int3 operator*(int3 a, int3 b)
{
    return make_int3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline RT_HOSTDEVICE int3 operator*(int3 a, int s)
{
    return make_int3(a.x * s, a.y * s, a.z * s);
}
inline RT_HOSTDEVICE int3 operator*(int s, int3 a)
{
    return make_int3(a.x * s, a.y * s, a.z * s);
}
inline RT_HOSTDEVICE void operator*=(int3 &a, int s)
{
    a.x *= s; a.y *= s; a.z *= s;
}

/* divide */
inline RT_HOSTDEVICE int3 operator/(int3 a, int3 b)
{
    return make_int3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline RT_HOSTDEVICE int3 operator/(int3 a, int s)
{
    return make_int3(a.x / s, a.y / s, a.z / s);
}
inline RT_HOSTDEVICE int3 operator/(int s, int3 a)
{
    return make_int3(s /a.x, s / a.y, s / a.z);
}
inline RT_HOSTDEVICE void operator/=(int3 &a, int s)
{
    a.x /= s; a.y /= s; a.z /= s;
}

/* clamp */
inline RT_HOSTDEVICE int clamp(int f, int a, int b)
{
    return max(a, min(f, b));
}

inline RT_HOSTDEVICE int3 clamp(int3 v, int a, int b)
{
    return make_int3(clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b));
}

inline RT_HOSTDEVICE int3 clamp(int3 v, int3 a, int3 b)
{
    return make_int3(clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y), clamp(v.z, a.z, b.z));
}


/* uint3 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE uint3 make_uint3(uint s)
{
    return make_uint3(s, s, s);
}
inline RT_HOSTDEVICE uint3 make_uint3(float3 a)
{
    return make_uint3(uint(a.x), uint(a.y), uint(a.z));
}

/* min */
inline RT_HOSTDEVICE uint3 min(uint3 a, uint3 b)
{
    return make_uint3(min(a.x,b.x), min(a.y,b.y), min(a.z,b.z));
}

/* max */
inline RT_HOSTDEVICE uint3 max(uint3 a, uint3 b)
{
    return make_uint3(max(a.x,b.x), max(a.y,b.y), max(a.z,b.z));
}

/* addition */
inline RT_HOSTDEVICE uint3 operator+(uint3 a, uint3 b)
{
    return make_uint3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline RT_HOSTDEVICE void operator+=(uint3 &a, uint3 b)
{
    a.x += b.x; a.y += b.y; a.z += b.z;
}

/* subtract */
inline RT_HOSTDEVICE uint3 operator-(uint3 a, uint3 b)
{
    return make_uint3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline RT_HOSTDEVICE void operator-=(uint3 &a, uint3 b)
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z;
}

/* multiply */
inline RT_HOSTDEVICE uint3 operator*(uint3 a, uint3 b)
{
    return make_uint3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline RT_HOSTDEVICE uint3 operator*(uint3 a, uint s)
{
    return make_uint3(a.x * s, a.y * s, a.z * s);
}
inline RT_HOSTDEVICE uint3 operator*(uint s, uint3 a)
{
    return make_uint3(a.x * s, a.y * s, a.z * s);
}
inline RT_HOSTDEVICE void operator*=(uint3 &a, uint s)
{
    a.x *= s; a.y *= s; a.z *= s;
}

/* divide */
inline RT_HOSTDEVICE uint3 operator/(uint3 a, uint3 b)
{
    return make_uint3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline RT_HOSTDEVICE uint3 operator/(uint3 a, uint s)
{
    return make_uint3(a.x / s, a.y / s, a.z / s);
}
inline RT_HOSTDEVICE uint3 operator/(uint s, uint3 a)
{
    return make_uint3(s / a.x, s / a.y, s / a.z);
}
inline RT_HOSTDEVICE void operator/=(uint3 &a, uint s)
{
    a.x /= s; a.y /= s; a.z /= s;
}

/* clamp */
inline RT_HOSTDEVICE uint clamp(uint f, uint a, uint b)
{
    return max(a, min(f, b));
}

inline RT_HOSTDEVICE uint3 clamp(uint3 v, uint a, uint b)
{
    return make_uint3(clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b));
}

inline RT_HOSTDEVICE uint3 clamp(uint3 v, uint3 a, uint3 b)
{
    return make_uint3(clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y), clamp(v.z, a.z, b.z));
}

/* int4 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE int4 make_int4(int s)
{
    return make_int4(s, s, s, s);
}

/* equality */
inline RT_HOSTDEVICE bool operator==(int4 a, int4 b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/* uint4 functions */
/******************************************************************************/

/* additional constructors */
inline RT_HOSTDEVICE uint4 make_uint4(uint s)
{
    return make_uint4(s, s, s, s);
}

/* Narrowing */
inline RT_HOSTDEVICE int2 make_int2(int3 v0) { return make_int2( v0.x, v0.y ); }
inline RT_HOSTDEVICE int2 make_int2(int4 v0) { return make_int2( v0.x, v0.y ); }
inline RT_HOSTDEVICE int3 make_int3(int4 v0) { return make_int3( v0.x, v0.y, v0.z ); }
inline RT_HOSTDEVICE uint2 make_uint2(uint3 v0) { return make_uint2( v0.x, v0.y ); }
inline RT_HOSTDEVICE uint2 make_uint2(uint4 v0) { return make_uint2( v0.x, v0.y ); }
inline RT_HOSTDEVICE uint3 make_uint3(uint4 v0) { return make_uint3( v0.x, v0.y, v0.z ); }
inline RT_HOSTDEVICE float2 make_float2(float3 v0) { return make_float2( v0.x, v0.y ); }
inline RT_HOSTDEVICE float2 make_float2(float4 v0) { return make_float2( v0.x, v0.y ); }

/* Assemble from smaller vectors */
inline RT_HOSTDEVICE int3 make_int3(int v0, int2 v1) { return make_int3( v0, v1.x, v1.y ); }
inline RT_HOSTDEVICE int3 make_int3(int2 v0, int v1) { return make_int3( v0.x, v0.y, v1 ); }
inline RT_HOSTDEVICE int4 make_int4(int v0, int v1, int2 v2) { return make_int4( v0, v1, v2.x, v2.y ); }
inline RT_HOSTDEVICE int4 make_int4(int v0, int2 v1, int v2) { return make_int4( v0, v1.x, v1.y, v2 ); }
inline RT_HOSTDEVICE int4 make_int4(int2 v0, int v1, int v2) { return make_int4( v0.x, v0.y, v1, v2 ); }
inline RT_HOSTDEVICE int4 make_int4(int v0, int3 v1) { return make_int4( v0, v1.x, v1.y, v1.z ); }
inline RT_HOSTDEVICE int4 make_int4(int3 v0, int v1) { return make_int4( v0.x, v0.y, v0.z, v1 ); }
inline RT_HOSTDEVICE int4 make_int4(int2 v0, int2 v1) { return make_int4( v0.x, v0.y, v1.x, v1.y ); }
inline RT_HOSTDEVICE uint3 make_uint3(unsigned int v0, uint2 v1) { return make_uint3( v0, v1.x, v1.y ); }
inline RT_HOSTDEVICE uint3 make_uint3(uint2 v0, unsigned int v1) { return make_uint3( v0.x, v0.y, v1 ); }
inline RT_HOSTDEVICE uint4 make_uint4(unsigned int v0, unsigned int v1, uint2 v2) { return make_uint4( v0, v1, v2.x, v2.y ); }
inline RT_HOSTDEVICE uint4 make_uint4(unsigned int v0, uint2 v1, unsigned int v2) { return make_uint4( v0, v1.x, v1.y, v2 ); }
inline RT_HOSTDEVICE uint4 make_uint4(uint2 v0, unsigned int v1, unsigned int v2) { return make_uint4( v0.x, v0.y, v1, v2 ); }
inline RT_HOSTDEVICE uint4 make_uint4(unsigned int v0, uint3 v1) { return make_uint4( v0, v1.x, v1.y, v1.z ); }
inline RT_HOSTDEVICE uint4 make_uint4(uint3 v0, unsigned int v1) { return make_uint4( v0.x, v0.y, v0.z, v1 ); }
inline RT_HOSTDEVICE uint4 make_uint4(uint2 v0, uint2 v1) { return make_uint4( v0.x, v0.y, v1.x, v1.y ); }
inline RT_HOSTDEVICE float3 make_float3(float v0, float2 v1) { return make_float3( v0, v1.x, v1.y ); }
inline RT_HOSTDEVICE float4 make_float4(float v0, float v1, float2 v2) { return make_float4( v0, v1, v2.x, v2.y ); }
inline RT_HOSTDEVICE float4 make_float4(float v0, float2 v1, float v2) { return make_float4( v0, v1.x, v1.y, v2 ); }
inline RT_HOSTDEVICE float4 make_float4(float2 v0, float v1, float v2) { return make_float4( v0.x, v0.y, v1, v2 ); }
inline RT_HOSTDEVICE float4 make_float4(float v0, float3 v1) { return make_float4( v0, v1.x, v1.y, v1.z ); }
inline RT_HOSTDEVICE float4 make_float4(float2 v0, float2 v1) { return make_float4( v0.x, v0.y, v1.x, v1.y ); }

static __inline__ RT_HOSTDEVICE float fmaxf(float2 a)
{
  return fmaxf(a.x, a.y);
}

static __inline__ RT_HOSTDEVICE float fmaxf(float3 a)
{
  return fmaxf(fmaxf(a.x, a.y), a.z);
}

static __inline__ RT_HOSTDEVICE float fmaxf(float4 a)
{
  return fmaxf(fmaxf(a.x, a.y), fmaxf(a.z, a.w));
}

static __inline__ RT_HOSTDEVICE float fminf(float2 a)
{
  return fminf(a.x, a.y);
}

static __inline__ RT_HOSTDEVICE float fminf(float3 a)
{
  return fminf(fminf(a.x, a.y), a.z);
}

static __inline__ RT_HOSTDEVICE float fminf(float4 a)
{
  return fminf(fminf(a.x, a.y), fminf(a.z, a.w));
}

inline RT_HOSTDEVICE float2 make_float2(uint2 a)
{
    return make_float2(float(a.x), float(a.y));
}

inline RT_HOSTDEVICE float3 make_float3(uint3 a)
{
    return make_float3(float(a.x), float(a.y), float(a.z));
}

inline RT_HOSTDEVICE float4 make_float4(uint4 a)
{
    return make_float4(float(a.x), float(a.y), float(a.z), float(a.w));
}

/* Return a smooth value in [0,1], where the transition from 0
   to 1 takes place for values of x in [edge0,edge1]. */
inline RT_HOSTDEVICE float smoothstep( float edge0, float edge1, float x )
{
    /*assert( edge1 > edge0 ); */
    const float t = clamp( (x-edge0) / (edge1-edge0), 0.0f, 1.0f );
    return t*t * ( 3.0f - 2.0f*t );
}

/* Simple mapping from [0,1] to a temperature-like RGB color. */
inline RT_HOSTDEVICE float3 temperature( float t )
{
    /*assert( t >= 0 ); */
    /*assert( t <= 1 ); */
    const float b = t < 0.25f ? smoothstep( -0.25f, 0.25f, t ) : 1.0f-smoothstep( 0.25f, 0.5f, t );
    const float g = t < 0.5f  ? smoothstep( 0.0f, 0.5f, t ) :
               (t < 0.75f ? 1.0f : 1.0f-smoothstep( 0.75f, 1.0f, t ));
    const float r = smoothstep( 0.5f, 0.75f, t );
    return make_float3( r, g, b );
}

/*
  calculates refraction direction
  r   : refraction vector
  i   : incident vector
  n   : surface normal
  ior : index of refraction ( n2 / n1 )
  returns false in case of total internal reflection
*/
inline RT_HOSTDEVICE bool refract(float3& r, float3 i, float3 n, float ior)
{
  float3 nn = n;
  float negNdotV = dot(i,nn);
  float eta;

  if (negNdotV > 0.0f)
  {
    eta = ior;
    nn = -n;
    negNdotV = -negNdotV;
  }
  else
  {
    eta = 1.f / ior;
  }

  const float k = 1.f - eta*eta * (1.f - negNdotV * negNdotV);

  if (k < 0.0f) {
    return false;
  } else {
    r = normalize(eta*i - (eta*negNdotV + sqrtf(k)) * nn);
    return true;
  }
}

/* Schlick approximation of Fresnel reflectance */
inline RT_HOSTDEVICE float fresnel_schlick(float cos_theta, float exponent = 5.0f,
                                                            float minimum  = 0.0f,
                                                            float maximum  = 1.0f)
{
  /*
     clamp the result of the arithmetic due to floating point precision:
     the result should lie strictly within [minimum, maximum]
    return clamp(minimum + (maximum - minimum) * powf(1.0f - cos_theta, exponent),
                 minimum, maximum);

  */

  /* The max doesn't seem like it should be necessary, but without it you get
     annoying broken pixels at the center of reflective spheres where cos_theta ~ 1.
  */
  return clamp(minimum + (maximum - minimum) * powf(fmaxf(0.0f,1.0f - cos_theta), exponent),
               minimum, maximum);
}

inline RT_HOSTDEVICE float3 fresnel_schlick(float cos_theta, float exponent,
                                            float3 minimum, float3 maximum)
{
  return make_float3(fresnel_schlick(cos_theta, exponent, minimum.x, maximum.x),
                     fresnel_schlick(cos_theta, exponent, minimum.y, maximum.y),
                     fresnel_schlick(cos_theta, exponent, minimum.z, maximum.z));
}

/* orthonormal basis */
namespace optix {
  struct Onb
  {
    inline RT_HOSTDEVICE Onb( const float3& normal )
    {
      m_normal = normal;

      if( fabs(m_normal.x) > fabs(m_normal.z) )
        {
          m_binormal.x = -m_normal.y;
          m_binormal.y =  m_normal.x;
          m_binormal.z =  0;
        }
      else
        {
          m_binormal.x =  0;
          m_binormal.y = -m_normal.z;
          m_binormal.z =  m_normal.y;
        }

      m_binormal = normalize(m_binormal);
      m_tangent = cross( m_binormal, m_normal );
    }

    inline RT_HOSTDEVICE void inverse_transform( float3& p )
    {
      p = p.x*m_tangent + p.y*m_binormal + p.z*m_normal;
    }

    float3 m_tangent;
    float3 m_binormal;
    float3 m_normal;
  };
}

#endif
