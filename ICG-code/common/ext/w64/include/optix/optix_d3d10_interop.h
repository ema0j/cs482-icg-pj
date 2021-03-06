/*
 * Copyright (c) 2010 NVIDIA Corporation.  All rights reserved.
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

#ifndef __optix_optix_dx10_interop_h__
#define __optix_optix_dx10_interop_h__

/************************************
 **
 **    DX10 Interop functions
 **
 ***********************************/

#if defined( _WIN32 )

#include "optix.h"

struct ID3D10Device;
struct ID3D10Resource;

#ifdef __cplusplus
  extern "C" 
  {
#endif

  RTresult RTAPI rtContextSetD3D10Device( RTcontext context, ID3D10Device* device );

  RTresult RTAPI rtBufferCreateFromD3D10Resource        ( RTcontext context, RTbuffertype type, ID3D10Resource* resource,  RTbuffer* buffer );
  RTresult RTAPI rtTextureSamplerCreateFromD3D10Resource( RTcontext context, ID3D10Resource* resource,  RTtexturesampler* textureSampler );
  RTresult RTAPI rtBufferGetD3D10Resource               ( RTbuffer buffer, ID3D10Resource** resource );
  RTresult RTAPI rtTextureSamplerGetD3D10Resource       ( RTtexturesampler textureSampler, ID3D10Resource** resource );
  RTresult RTAPI rtBufferD3D10Register                  ( RTbuffer buffer );
  RTresult RTAPI rtBufferD3D10Unregister                ( RTbuffer buffer );
  RTresult RTAPI rtTextureSamplerD3D10Register          ( RTtexturesampler textureSampler );
  RTresult RTAPI rtTextureSamplerD3D10Unregister        ( RTtexturesampler textureSampler );


#ifdef __cplusplus
  }

#endif

#endif /* defined( _WIN32 ) */

#endif /* __optix_optix_dx10_interop_h__ */
