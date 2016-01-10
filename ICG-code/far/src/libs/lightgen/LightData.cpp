#include "LightData.h"

VL_TYPE LightList::GetLightType(uint32_t idx) const
{
    if (idx < _ptLights.size())
        return OMNIDIR_LIGHT;

    if (idx < _ptLights.size() + _dirLights.size())
        return DIRECTIONAL_LIGHT;

    if (idx < _ptLights.size() + _dirLights.size() + _otrLights.size())
        return ORIENTED_LIGHT;

    assert(false);
    return UNKNOWN_LIGHT;
}

const VLight* LightList::GetLight(uint32_t idx) const
{
    if (idx < _ptLights.size())
        return &_ptLights[idx];

    if (idx < _ptLights.size() + _dirLights.size())
        return &_dirLights[idx - _ptLights.size()];

    if (idx < _ptLights.size() + _dirLights.size() + _otrLights.size())
        return &_otrLights[idx - _ptLights.size() - _dirLights.size()];;

    assert(false);
    return NULL;
}

VLight* LightList::GetLight(uint32_t idx)
{
    if (idx < _ptLights.size())
        return &_ptLights[idx];

    if (idx < _ptLights.size() + _dirLights.size())
        return &_dirLights[idx - _ptLights.size()];

    if (idx < _ptLights.size() + _dirLights.size() + _otrLights.size())
        return &_otrLights[idx - _ptLights.size() - _dirLights.size()];;

    assert(false);
    return NULL;
}

Vec3f LightList::GetPower( uint32_t idx ) const
{
    if (idx < _ptLights.size())
        return _ptLights[idx].intensity;

    if (idx < _ptLights.size() + _dirLights.size())
        return _dirLights[idx - _ptLights.size()].le;

    if (idx < _ptLights.size() + _dirLights.size() + _otrLights.size())
        return _otrLights[idx - _ptLights.size() - _dirLights.size()].le;

    assert(false);
    return Vec3f::Zero();
}

Range1u LightList::GetRange( VL_TYPE type ) const
{
    switch(type)
    {
    case OMNIDIR_LIGHT:
        return Range1u(0, (uint32_t)_ptLights.size());
    case DIRECTIONAL_LIGHT:
        return Range1u((uint32_t)_ptLights.size(), (uint32_t)(_ptLights.size() + _dirLights.size()));
    case ORIENTED_LIGHT:
        return Range1u((uint32_t)(_ptLights.size() + _dirLights.size()), (uint32_t)(_ptLights.size() + _dirLights.size() + _otrLights.size()));
    }
    return Range1u::Empty();
}
