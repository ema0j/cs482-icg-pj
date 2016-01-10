#include <lightgen/LightData.h>
#include <vector>
#include <scene/vlight.h>

using std::vector;

#ifndef _SIMPLE_VIRTUAL_LIGHT_CACHE_H_
#define _SIMPLE_VIRTUAL_LIGHT_CACHE_H_

class SimpleVirtualLightCache : public VirtualLightCache
{
public:
	SimpleVirtualLightCache()
		: VirtualLightCache(), _nDirect(0), _nIndirect(0) {}
	~SimpleVirtualLightCache(void) {}

	virtual void			Clear() { _nDirect = _nIndirect = 0; _Clear(); }
	virtual void			AddOmniDirLight(const Vec3f &position, const Vec3f &intensity) 
	{     
		_omniDirLights.push_back(OmniDirLight(position, intensity));
	}
	virtual void			AddDirLight(const Vec3f &normal, const Vec3f &le)
	{
		_dirLights.push_back(DirLight(normal, le));
	}
	virtual void			AddOrientedLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
	{
		_orientedLights.push_back(OrientedLight(position, normal, le));
	}
	virtual void			AddIndirectLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
	{
		_orientedLights.push_back(OrientedLight(position, normal, le));
		_nIndirect++;
	}
	virtual uint32_t      DirectLightNum() const { return static_cast<uint32_t>(_omniDirLights.size() + _dirLights.size() + _orientedLights.size() - _nIndirect); }
	virtual uint32_t      IndirectLightNum() const { return _nIndirect; }
	void _Clear()                                      { _omniDirLights.clear(); _orientedLights.clear(); _dirLights.clear(); }

	vector<OmniDirLight>    &OmniDirLights() { return _omniDirLights; }
	vector<DirLight>        &DirectionalLights() { return _dirLights; }
	vector<OrientedLight>   &OrientedLights() { return _orientedLights; }
private:
	uint32_t						_nDirect;
	uint32_t						_nIndirect;

	vector<OmniDirLight>                _omniDirLights;
	vector<DirLight>                    _dirLights;
	vector<OrientedLight>               _orientedLights;
};

class SimpleRefVirtualLightCache : public VirtualLightCache
{
public:
    SimpleRefVirtualLightCache(vector<OmniDirLight> &omniDirLights, vector<DirLight> &dirLights, vector<OrientedLight> &orientedLights)
        : VirtualLightCache(), _orientedLights(orientedLights), _dirLights(dirLights), _omniDirLights(omniDirLights), _nDirect(0), _nIndirect(0) {}
    ~SimpleRefVirtualLightCache(void) {}

    virtual void			Clear() { _nDirect = _nIndirect = 0; _Clear(); }
    virtual void			AddOmniDirLight(const Vec3f &position, const Vec3f &intensity) 
    {     
        _omniDirLights.push_back(OmniDirLight(position, intensity));
    }
    virtual void			AddDirLight(const Vec3f &normal, const Vec3f &le)
    {
        _dirLights.push_back(DirLight(normal, le));
    }
    virtual void			AddOrientedLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
    {
        _orientedLights.push_back(OrientedLight(position, normal, le));
    }
    virtual void			AddIndirectLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
    {
        _orientedLights.push_back(OrientedLight(position, normal, le));
        _nIndirect++;
    }
    virtual uint32_t      DirectLightNum() const { return static_cast<uint32_t>(_omniDirLights.size() + _dirLights.size() + _orientedLights.size() - _nIndirect); }
    virtual uint32_t      IndirectLightNum() const { return _nIndirect; }
    void _Clear()                                      { _omniDirLights.clear(); _orientedLights.clear(); _dirLights.clear(); }

    vector<OmniDirLight>    &OmniDirLights() { return _omniDirLights; }
    vector<DirLight>        &DirectionalLights() { return _dirLights; }
    vector<OrientedLight>   &OrientedLights() { return _orientedLights; }
private:
    uint32_t						_nDirect;
    uint32_t						_nIndirect;

    vector<OmniDirLight>            &_omniDirLights;
    vector<DirLight>                &_dirLights;
    vector<OrientedLight>           &_orientedLights;
};

#endif // _SIMPLE_VIRTUAL_LIGHT_CACHE_H_
