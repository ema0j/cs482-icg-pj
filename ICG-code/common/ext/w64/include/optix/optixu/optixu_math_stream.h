
/*
 * Copyright (c) 2008 - 2009 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and proprietary
 * rights in and to this software, related documentation and any modifications thereto.
 * Any use, reproduction, disclosure or distribution of this software and related
 * documentation without an express license agreement from NVIDIA Corporation is strictly
 * prohibited.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS*
 * AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY
 * SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR
 * INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES
 */

/* Stream operators for CUDA vector types */

#ifndef __optixu_math_stream_h__
#define __optixu_math_stream_h__

#include <optix_math.h>

#include <iostream>

namespace {
  inline std::ostream& operator<<(std::ostream& os, const float4& v) { os << '[' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, float4& v) { char st; is >> st >> v.x >> st >> v.y >> st >> v.z >> st >> v.w >> st; return is; }
  inline std::ostream& operator<<(std::ostream& os, const float3& v) { os << '[' << v.x << ", " << v.y << ", " << v.z << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, float3& v) { char st; is >> st >> v.x >> st >> v.y >> st >> v.z >> st; return is; }
  inline std::ostream& operator<<(std::ostream& os, const float2& v) { os << '[' << v.x << ", " << v.y << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, float2& v) { char st; is >> st >> v.x >> st >> v.y >> st; return is; }

  inline std::ostream& operator<<(std::ostream& os, const int4& v) { os << '[' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, int4& v) { char st; is >> st >> v.x >> st >> v.y >> st >> v.z >> st >> v.w >> st; return is; }
  inline std::ostream& operator<<(std::ostream& os, const int3& v) { os << '[' << v.x << ", " << v.y << ", " << v.z << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, int3& v) { char st; is >> st >> v.x >> st >> v.y >> st >> v.z >> st; return is; }
  inline std::ostream& operator<<(std::ostream& os, const int2& v) { os << '[' << v.x << ", " << v.y << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, int2& v) { char st; is >> st >> v.x >> st >> v.y >> st; return is; }

  inline std::ostream& operator<<(std::ostream& os, const uint4& v) { os << '[' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, uint4& v) { char st; is >> st >> v.x >> st >> v.y >> st >> v.z >> st >> v.w >> st; return is; }
  inline std::ostream& operator<<(std::ostream& os, const uint3& v) { os << '[' << v.x << ", " << v.y << ", " << v.z << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, uint3& v) { char st; is >> st >> v.x >> st >> v.y >> st >> v.z >> st; return is; }
  inline std::ostream& operator<<(std::ostream& os, const uint2& v) { os << '[' << v.x << ", " << v.y << ']'; return os; }
  inline std::istream& operator>>(std::istream& is, uint2& v) { char st; is >> st >> v.x >> st >> v.y >> st; return is; }

}

#endif /* __optixu_math_stream_h__ */

