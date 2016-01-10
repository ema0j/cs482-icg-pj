//////////////////////////////////////////////////////////////////////////
//!
//!	\file    VirtualLightData.h
//!	\date    31:3:2010   14:49
//!	\author  Jiawei Ou
//!	
//!	\brief   VirtualLightData declaration
//!
//////////////////////////////////////////////////////////////////////////
#ifndef _VIRTUAL_LIGHT_DATA_H_
#define _VIRTUAL_LIGHT_DATA_H_

#include <vmath/vec3.h>
#include <misc/stdcommon.h>
#include "vmath/range1.h"

struct VLight {};

struct OmniDirLight : public VLight
{
	OmniDirLight() {};
    OmniDirLight(const Vec3f &pos, const Vec3f &it) 
        : position(pos), intensity(it) {}
	Vec3f intensity;
    Vec3f position;
};

struct DirLight : public VLight
{
	DirLight() {};
    DirLight(const Vec3f &norm, const Vec3f &l) 
        : normal(norm), le(l) {} 
	Vec3f le;
    Vec3f normal;
};

struct OrientedLight : public VLight
{
	OrientedLight() {};
    OrientedLight(const Vec3f &pos, const Vec3f &norm, const Vec3f &l) 
        : position(pos), normal(norm), le(l) {}
	Vec3f le;
    Vec3f position;
    Vec3f normal;
};

enum VL_TYPE
{
	OMNIDIR_LIGHT,
	DIRECTIONAL_LIGHT,
	ORIENTED_LIGHT,
	UNKNOWN_LIGHT
};


struct LightList 
{
    VL_TYPE         GetLightType(uint32_t idx) const;
    const VLight*   GetLight(uint32_t idx) const;
    VLight*         GetLight(uint32_t idx);
    Vec3f           GetPower( uint32_t idx ) const;
    inline uint32_t GetSize() const { return static_cast<uint32_t>(_ptLights.size() + _dirLights.size() + _otrLights.size()); }
    inline void     Clear() { _ptLights.clear(); _dirLights.clear(); _otrLights.clear(); }
    Range1u         GetRange(VL_TYPE type) const;

	vector<OrientedLight>	_otrLights;
	vector<DirLight>		_dirLights;
	vector<OmniDirLight>	_ptLights;
};
#endif // _VIRTUAL_LIGHT_DATA_H_