#include "SerializableLightCache.h"


void SerializableLightCache::Save(const string &filename)
{
	ofstream os(filename.c_str(), ios_base::out | ios_base::binary);
	uint32_t lsize = 0;
	
	// write oriented light
	lsize = (uint32_t)_otrLights.size();
	os.write((const char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		const OrientedLight& ortLight = _otrLights[i];
		_Save(os, ortLight);
	}

	// write directional light
	lsize = (uint32_t)_dirLights.size();
	os.write((const char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		const DirLight& dirLight = _dirLights[i];
		_Save(os, dirLight);
	}

	// write point light
	lsize = (uint32_t)_ptLights.size();
	os.write((const char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		const OmniDirLight& ptLight = _ptLights[i];
		_Save(os, ptLight);
	}

	// write indirect light
	lsize = (uint32_t)_idtLights.size();
	os.write((const char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		const OrientedLight& idtLight = _idtLights[i];
		_Save(os, idtLight);
	}
}

void SerializableLightCache::Load(const string &filename)
{
	Clear();
	ifstream is(filename.c_str(), ios_base::in | ios_base::binary);
	uint32_t lsize = 0;

	// read oriented light
	is.read((char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		OrientedLight ortLight;
		_Load(is, ortLight);
		_otrLights.push_back(ortLight);
	}

	// read directional light
	is.read((char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		DirLight dirLight;
		_Load(is, dirLight);
		_dirLights.push_back(dirLight);
	}

	// write point light
	is.read((char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		OmniDirLight ptLight;
		_Load(is, ptLight);
		_ptLights.push_back(ptLight);
	}

	// write indirect light
	is.read((char*)&lsize, sizeof(uint32_t));
	for (uint32_t i = 0; i < lsize; i++)
	{
		OrientedLight idtLight;
		_Load(is, idtLight);
		_idtLights.push_back(idtLight);
	}
}

void SerializableLightCache::_Save(ofstream &os,  const OmniDirLight& ptLight )
{
	_WriteValue(os, ptLight.position);
	_WriteValue(os, ptLight.intensity);
}

void SerializableLightCache::_Save(ofstream &os,  const OrientedLight& ortLight )
{
	_WriteValue(os, ortLight.position);
	_WriteValue(os, ortLight.normal);
	_WriteValue(os, ortLight.le);
}


void SerializableLightCache::_Save(ofstream &os,  const DirLight& dirLight )
{
	_WriteValue(os, dirLight.normal);
	_WriteValue(os, dirLight.le);
}

void SerializableLightCache::_Load(ifstream &is, OmniDirLight& ptLight )
{
	_ReadValue(is, ptLight.position);
	_ReadValue(is, ptLight.intensity);
}

void SerializableLightCache::_Load(ifstream &is, OrientedLight& ortLight )
{
	_ReadValue(is, ortLight.position);
	_ReadValue(is, ortLight.normal);
	_ReadValue(is, ortLight.le);
}


void SerializableLightCache::_Load(ifstream &is, DirLight& dirLight )
{
	_ReadValue(is, dirLight.normal);
	_ReadValue(is, dirLight.le);
}


bool operator == (const OmniDirLight &l1, const OmniDirLight &l2)
{
	return (l1.position == l2.position) && (l1.intensity == l2.intensity);
}

bool operator != (const OmniDirLight &l1, const OmniDirLight &l2)
{
	return !(l1 == l2);
}

bool operator == (const DirLight &l1, const DirLight &l2)
{
	return (l1.normal == l2.normal) && (l1.le == l2.le);
}

bool operator != (const DirLight &l1, const DirLight &l2)
{
	return !(l1 == l2);
}

bool operator == (const OrientedLight &l1, const OrientedLight &l2)
{
	return (l1.position == l2.position) && (l1.normal == l2.normal) && (l1.le == l2.le);
}

bool operator != (const OrientedLight &l1, const OrientedLight &l2)
{
	return !(l1 == l2);
}


bool SerializableLightCache::operator == (const SerializableLightCache& cache)
{
	if(	_otrLights.size() != cache._otrLights.size() ||
		_ptLights.size() != cache._ptLights.size() ||
		_dirLights.size() != cache._dirLights.size() ||
		_idtLights.size() != cache._idtLights.size())
		return false;

	for (uint32_t i = 0; i < _otrLights.size(); i++)
	{
		if(_otrLights[i] != cache._otrLights[i])
			return false;
	}

	for (uint32_t i = 0; i < _dirLights.size(); i++)
	{
		if(_dirLights[i] != cache._dirLights[i])
			return false;
	}

	for (uint32_t i = 0; i < _ptLights.size(); i++)
	{
		if(_ptLights[i] != cache._ptLights[i])
			return false;
	}

	for (uint32_t i = 0; i < _idtLights.size(); i++)
	{
		if(_idtLights[i] != cache._idtLights[i])
			return false;
	}
	return true;
}