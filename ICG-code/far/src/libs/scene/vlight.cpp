//////////////////////////////////////////////////////////////////////////
//!
//!	\file    virtual_light.cpp
//!	\date    3:3:2010   9:39
//!	\author  Jiawei Ou
//!	
//!	\brief   virtual_light definition
//!
//////////////////////////////////////////////////////////////////////////
#include "vlight.h"


//////////////////////////////////////////////////////////////////////////
//!	
//!	\brief Omni-directional light sample constructor
//!
//////////////////////////////////////////////////////////////////////////
OmniVLightSample::OmniVLightSample(const Vec3f &pos, const Vec3f &I)
: _position(pos), _intensity(I)
{
}

//! \brief Transform the sample
void OmniVLightSample::Transform(const Matrix4d &m)
{
    _position = m.TransformPoint(_position);
}

//! \brief Check if the sample is valid
bool OmniVLightSample::IsValid() const
{
    return (_intensity.IsZero() == false);
}

void OmniVLightSample::AddToCache(VirtualLightCache *cache, uint32_t samples) const
{
    cache->AddOmniDirLight(_position, _intensity / static_cast<float>(samples));
}

//////////////////////////////////////////////////////////////////////////
//!	
//!	\brief Spot light sample constructor
//!
//////////////////////////////////////////////////////////////////////////
SpotVLightSample::SpotVLightSample(const Vec3f &pos, const Vec3f &intensity, 
                                   float innerCos, float outterCos) 
                                   : OmniVLightSample(pos, intensity), 
                                   _innerCosAngle(innerCos), 
                                   _outterCosAngle(outterCos)
{
}

//! \brief Add light sample to the cache
void SpotVLightSample::AddToCache(VirtualLightCache *cache, uint32_t samples) const
{
    //TODO
}

//////////////////////////////////////////////////////////////////////////
//!	
//!	\brief Directional light sample constructor
//!
//////////////////////////////////////////////////////////////////////////
DirVLightSample::DirVLightSample(const Vec3f &norm, const Vec3f &le)
: _normal(norm), _le(le)
{
}

//! \brief Transform the light sample
void DirVLightSample::Transform(const Matrix4d &m)
{
    _normal = m.TransformVector(_normal);
}

//! \brief Check if the light sample is valid
bool DirVLightSample::IsValid() const
{
    return (_le.IsZero() == false);
}

//! \brief Add light sample to the cache
void DirVLightSample::AddToCache(VirtualLightCache *cache, uint32_t samples) const
{
    cache->AddDirLight(_normal, _le / static_cast<float>(samples));
}

//////////////////////////////////////////////////////////////////////////
//!	
//!	\brief Oriented light sample constructor
//!
//////////////////////////////////////////////////////////////////////////
OrientdVLightSample::OrientdVLightSample(const Vec3f &pos, const Vec3f &norm, 
                                         const Vec3f &le) 
: VLightSample(), _position(pos), _normal(norm), _le(le)
{
}

//! \brief Check if the light sample is valid
bool OrientdVLightSample::IsValid() const { return (_le.IsZero() == false); }

//! \brief Transform the light sample
void OrientdVLightSample::Transform(const Matrix4d &m)
{
    _position = m.TransformPoint(_position);
    _normal = m.TransformVector(_normal);
}

//! \brief Add light sample to the cache
void OrientdVLightSample::AddToCache(VirtualLightCache *cache, uint32_t samples) const
{
    cache->AddOrientedLight(_position, _normal, _le / static_cast<float>(samples));
}
