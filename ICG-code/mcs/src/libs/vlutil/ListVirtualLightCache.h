#include <vector>
#include <scene/vlight.h>
#include <lightgen/LightData.h>

using std::vector;

#ifndef _LIST_VIRTUAL_LIGHT_CACHE_H_
#define _LIST_VIRTUAL_LIGHT_CACHE_H_

class ListVirtualLightCache : public VirtualLightCache
{
public:
	ListVirtualLightCache(LightList &llist)
		: VirtualLightCache(), _llist(llist), _nIndirect(0) {}
	~ListVirtualLightCache(void) {}

	virtual void			Clear() { _nIndirect = 0; _llist.Clear(); }
	virtual void			AddOmniDirLight(const Vec3f &position, const Vec3f &intensity) 
	{     
		_llist._ptLights.push_back(OmniDirLight(position, intensity));
	}
	virtual void			AddDirLight(const Vec3f &normal, const Vec3f &le)
	{
		_llist._dirLights.push_back(DirLight(normal, le));
	}
	virtual void			AddOrientedLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
	{
		_llist._otrLights.push_back(OrientedLight(position, normal, le));
	}
	virtual void			AddIndirectLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
	{
		_llist._otrLights.push_back(OrientedLight(position, normal, le));
		_nIndirect++;
	}
	virtual uint32_t      DirectLightNum() const { return static_cast<uint32_t>(_llist.GetSize() - _nIndirect); }
	virtual uint32_t      IndirectLightNum() const { return _nIndirect; }
private:
	uint32_t						_nIndirect;
	LightList						&_llist;
};

#endif // _LIST_VIRTUAL_LIGHT_CACHE_H_
