
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

#ifndef __optix_optix_internal_h__
#define __optix_optix_internal_h__

#if defined(_WIN64) || defined(__LP64__)
#error Compilation in 64-bit mode not supported, please specify the -m32 compiler flag.
#endif

#include "optix_datatypes.h"
#include "optix_defines.h"

struct rtObject;

namespace optix {

  __noinline__ __device__ void
    rt_undefined_use(int)
  {
  }

  inline __device__ void*
    rt_buffer_get(void* buffer, unsigned int dim, unsigned int element_size,
                  unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3)
  {
    void* tmp;
    asm volatile("call (%0), _rt_buffer_get, (%1, %2, %3, %4, %5, %6, %7);" :
                 "=r"(tmp) :
                 "r"(buffer), "r"(dim), "r"(element_size), "r"(i0), "r"(i1), "r"(i2), "r"(i3) :
                 );
    rt_undefined_use((int)tmp);
    return tmp;
  }

  inline __device__ uint4
    rt_buffer_get_size(const void* buffer, unsigned int dim, unsigned int element_size)
  {
    unsigned int d0, d1, d2, d3;
    asm volatile("call (%0, %1, %2, %3), _rt_buffer_get_size, (%4, %5, %6);" :
                 "=r"(d0), "=r"(d1), "=r"(d2), "=r"(d3) :
                 "r"(buffer), "r"(dim), "r"(element_size) :
                 );
    return make_uint4(d0, d1, d2, d3);
  }

  inline __device__ void rt_trace(unsigned int group,
                                  float3 origin, float3 direction, unsigned int ray_type, float tmin, float tmax,
                                  void* prd, unsigned int prd_size)
  {
    float ox = origin.x, oy = origin.y, oz = origin.z;
    float dx = direction.x, dy = direction.y, dz = direction.z;
    rt_undefined_use((int)prd);
    asm volatile("call _rt_trace, (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11);" :
                 /* no return value */ :
                 "r"(group), "f"(ox), "f"(oy), "f"(oz), "f"(dx), "f"(dy), "f"(dz),
                 "r"(ray_type), "f"(tmin), "f"(tmax), "r"(prd), "r"(prd_size) :
                 );
  }

  inline __device__ bool rt_potential_intersection(float t)
  {
    int ret;
    asm volatile("call (%0), _rt_potential_intersection, (%1);" :
                 "=r"(ret) :
                 "f"(t):
                 );
    return ret;
  }

  inline __device__ bool rt_report_intersection(unsigned int matlIndex)
  {
    int ret;
    asm volatile("call (%0), _rt_report_intersection, (%1);" :
                 "=r"(ret) :
                 "r"(matlIndex) :
                 );
    return ret;
  }

  inline __device__ void rt_ignore_intersection()
  {
    asm volatile("call _rt_ignore_intersection, ();");
  }

  inline __device__ void rt_terminate_ray()
  {
    asm volatile("call _rt_terminate_ray, ();");
  }

  inline __device__ void rt_intersect_child(unsigned int index)
  {
    asm volatile("call _rt_intersect_child, (%0);" :
                 /* no return value */ :
                 "r"(index) :
                 );
  }

  inline __device__ float3 rt_transform_point( RTtransformkind kind, const float3& p )
  {

    float f0, f1, f2, f3;
    asm volatile("call (%0, %1, %2, %3), _rt_transform_tuple, (%4, %5, %6, %7, %8);" :
                 "=f"(f0), "=f"(f1), "=f"(f2), "=f"(f3) :
                 "r"(kind), "f"(p.x), "f"(p.y), "f"(p.z), "f"(1.0f) :
                 );
    rt_undefined_use((int)f0);
    rt_undefined_use((int)f1);
    rt_undefined_use((int)f2);
    rt_undefined_use((int)f3);
    return make_float3( f0/f3, f1/f3, f2/f3 );

  }

  inline __device__ float3 rt_transform_vector( RTtransformkind kind, const float3& v )
  {
    float f0, f1, f2, f3;
    asm volatile("call (%0, %1, %2, %3), _rt_transform_tuple, (%4, %5, %6, %7, %8);" :
                 "=f"(f0), "=f"(f1), "=f"(f2), "=f"(f3) :
                 "r"(kind), "f"(v.x), "f"(v.y), "f"(v.z), "f"(0.0f) :
                 );
    rt_undefined_use((int)f0);
    rt_undefined_use((int)f1);
    rt_undefined_use((int)f2);
    rt_undefined_use((int)f3);
    f3 = f3;
    return make_float3( f0, f1, f2 );
  }

  inline __device__ float3 rt_transform_normal( RTtransformkind kind, const float3& n )
  {
    float f0, f1, f2, f3;
    asm volatile("call (%0, %1, %2, %3), _rt_transform_tuple, (%4, %5, %6, %7, %8);" :
                 "=f"(f0), "=f"(f1), "=f"(f2), "=f"(f3) :
                 "r"(kind | RT_INTERNAL_INVERSE_TRANSPOSE ), "f"(n.x), "f"(n.y), "f"(n.z), "f"(0.0f) :
                 );
    rt_undefined_use((int)f0);
    rt_undefined_use((int)f1);
    rt_undefined_use((int)f2);
    rt_undefined_use((int)f3);
    f3 = f3;
    return make_float3( f0, f1, f2 );
  }

  inline __device__ void rt_get_transform( RTtransformkind kind, float matrix[16] )
  {
    asm volatile("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), _rt_get_transform, (%16);" :
                 "=f"(matrix[ 0]), "=f"(matrix[ 1]), "=f"(matrix[ 2]), "=f"(matrix[ 3]),
                 "=f"(matrix[ 4]), "=f"(matrix[ 5]), "=f"(matrix[ 6]), "=f"(matrix[ 7]),
                 "=f"(matrix[ 8]), "=f"(matrix[ 9]), "=f"(matrix[10]), "=f"(matrix[11]),
                 "=f"(matrix[12]), "=f"(matrix[13]), "=f"(matrix[14]), "=f"(matrix[15]) :
                 "r"( kind ) :
                 );
  }

  inline __device__ void rt_throw( unsigned int code )
  {
    asm volatile("call _rt_throw, (%0);" :
                 /* no return value */ :
                 "r"(code) :
                 );
  }
  
  inline __device__ unsigned int rt_get_exception_code()
  {
    unsigned int result;
    asm volatile("call (%0), _rt_get_exception_code, ();" :
                 "=r"(result) :
                 );
    return result;
  }

  /*
     Printing
  */

  /*
   Type descriptors for printf arguments:
   0 = 32 bit integer value
   1 = 64 bit integer value
   2 = 32 bit float value
   3 = 64 bit double value
  */
  template<typename T> struct rt_print_t                     { static const int desc = 0; };
  template<>           struct rt_print_t<long long>          { static const int desc = 1; };
  template<>           struct rt_print_t<unsigned long long> { static const int desc = 1; };
  template<>           struct rt_print_t<float>              { static const int desc = 2; };
  template<>           struct rt_print_t<double>             { static const int desc = 3; };

  inline __device__ int rt_print_strlen( const char* s )
  {
    const char* p = s;
    while( *p ) ++p;
    return p - s;
  }

  template<typename T>
  inline __device__ int rt_print_arg( T arg, int off )
  {
    const int sz = max( 4, (int)sizeof( arg ) );
    const int typedesc = rt_print_t<T>::desc;

    const unsigned int* p;

    /* Get a pointer to a (at least) 32 bit value. */
    unsigned int iarg;
    if( sizeof(arg) < 4 )
    {
      iarg = (unsigned int)arg;
      p = &iarg;
    }
    else
    {
      p = (unsigned int*)&arg;
    }

    /* Write type descriptor. */
    asm volatile("call (), _rt_print_write32, (%0, %1);" :
                   :
                   "r"(typedesc), "r"(off) :
                   );

    /* Write argument. */
    for( int i=0; i<sz/4; ++i )
    {
      asm volatile("call (), _rt_print_write32, (%0, %1);" :
                   :
                   "r"(p[i]), "r"( off + (i+1)*4 ) :
                   );
    }

    return sz;
  }

  inline __device__ int rt_print_active()
  {
    int ret;
    asm volatile("call (%0), _rt_print_active, ();" :
                 "=r"(ret) :
                 :
                 );
    return ret;
  }

  inline __device__ int rt_print_start( const char* fmt, int sz )
  {
    int ret;
    asm volatile("call (%0), _rt_print_start, (%1, %2);" :
                 "=r"(ret) :
                 "r"(fmt), "r"(sz) :
                 );
    return ret;
  }

#define _RT_PRINTF_1()                                                  \
  if( !optix::rt_print_active() )                                       \
    return;                                                             \
  /* Compute length of header (=batchsize) plus format string. */       \
  const int fmtlen = optix::rt_print_strlen( fmt );                     \
  int sz = 4 + fmtlen + 1;                                              \
  sz = (sz+3) & ~3; /* align */

#define _RT_PRINTF_2()                                                  \
  int off; /* offset where to start writing args */                     \
  if( !(off=optix::rt_print_start(fmt,sz)) )                            \
    return;  /* print buffer is full */

#define _RT_PRINTF_ARG_1( a )                                           \
  /* Sum up argument sizes. */                                          \
  sz += 4;  /* type descriptor */                                       \
  sz += max( 4, static_cast<unsigned int>(sizeof( a )) );

#define _RT_PRINTF_ARG_2( a )                                           \
  /* Write out argument. */                                             \
  off += optix::rt_print_arg( a, off );                                 \
  off += 4; /* space for type desc */

} /* end namespace optix */

#endif /* __optix_optix_internal_h__ */
