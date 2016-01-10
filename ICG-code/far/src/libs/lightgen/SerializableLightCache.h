#ifndef _SERIALIZABLE_LIGHT_CACHE_H_
#define _SERIALIZABLE_LIGHT_CACHE_H_

#include <scene/vlight.h>
#include "LightData.h"

#include <fstream>
#include <ios>

using std::ios_base;
using std::ofstream;
using std::ifstream;

class SerializableLightCache : public VirtualLightCache
{
	friend class LightSerializeGenerator;
public:
	virtual void			Clear() { _otrLights.clear(); _ptLights.clear(); _dirLights.clear(); _idtLights.clear(); }
	virtual void			AddOmniDirLight(const Vec3f &position, const Vec3f &intensity) 
	{     
		_ptLights.push_back(OmniDirLight(position, intensity));
	}
	virtual void			AddDirLight(const Vec3f &normal, const Vec3f &le)
	{
		_dirLights.push_back(DirLight(normal, le));
	}
	virtual void			AddOrientedLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
	{
		_otrLights.push_back(OrientedLight(position, normal, le));
	}
	virtual void			AddIndirectLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
	{
		_idtLights.push_back(OrientedLight(position, normal, le));
	}
	virtual uint32_t		DirectLightNum() const { return static_cast<uint32_t>(_ptLights.size() + _dirLights.size() + _otrLights.size()); }
	virtual uint32_t		IndirectLightNum() const { return (uint32_t)_idtLights.size(); }

	void					Save(const string &filename);
	void					Load(const string &filename);
	bool					operator == (const SerializableLightCache& cache);
private:
	void					_Save(ofstream &os, const OrientedLight& ortLight );
	void					_Save(ofstream &os, const OmniDirLight& ptLight );
	void					_Save(ofstream &os, const DirLight& dirLight );

	void					_Load(ifstream &is, OrientedLight& ortLight );
	void					_Load(ifstream &is, OmniDirLight& ptLight );
	void					_Load(ifstream &is, DirLight& dirLight );
	template<typename T>
	void					_WriteValue( ofstream & os, const T &v )
	{
		uint32_t c = sizeof(v);
		os.write((char*)&v, c);
	}
	template<typename T>
	void					_ReadValue( ifstream & is, T &v )
	{
		uint32_t c = sizeof(v);
		is.read((char*)&v, c);
	}
	vector<OrientedLight>	_otrLights;
	vector<OrientedLight>	_idtLights;
	vector<DirLight>		_dirLights;
	vector<OmniDirLight>	_ptLights;
};

#endif // _SERIALIZABLE_LIGHT_CACHE_H_

