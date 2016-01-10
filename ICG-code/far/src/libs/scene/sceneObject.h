#ifndef _SCENE_COMMON_H_
#define _SCENE_COMMON_H_

#include <misc/stdcommon.h>
#include <misc/stats.h>
#include <serialization/archive.h>

class Background;
class Surface;
class InstanceGroup;
class Shape;
class Light;
class AreaLight;
class Material;
template<typename T>
class Texture;
class CubeTexture;
class Camera;
class Xform;

class ShadedSurface;
class Shader;
class ShaderParamSet;
class ShadedPass;

class ExportContext;

class SceneObject : public ISerializable {
public:
    virtual void CollectStats(StatsManager& stats) = 0;

	void SetUserData(shared_ptr<void> ud) { userData = ud; }
	template<class T> shared_ptr<T> GetUserData() { return static_pointer_cast<T>(userData); }

protected:
	shared_ptr<void> userData;
};

#endif
