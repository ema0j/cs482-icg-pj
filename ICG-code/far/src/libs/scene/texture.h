#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "sceneObject.h"
#include <misc/stats.h>
#include <scene/smath.h>
#include <image/image.h>
#include <image/cubemap.h>
#include <imageio/imageio.h>

template<typename T>
class Texture : public SceneObject
{
public:
    virtual T Sample(const Vec2f& st) = 0;
    virtual void CollectStats(StatsManager& stats) = 0;
    virtual T Average() const = 0;
};

typedef Texture<float> TextureF;
typedef Texture<Vec3f> TextureV;

template<typename T>
class ImageTexture : public Texture<T> {
public:
    ImageTexture();
    ImageTexture(const string& imfile, bool load, bool linear = true, bool mipmap = false, bool tile = true, bool flipY = true);
    ImageTexture(const Image<Vec3f>& im, const string& imfile, bool linear, bool mipmap, bool tile, bool flipY = false);

    Image<T>& ImageRef() { return image; }
    bool& Linear() { return linear; }
    bool& Mipmap() { return mipmap; }
    bool& Tile() { return tile; }
    string& Filename() { return filename; }

    T Average() const { return average; }

    virtual T Sample(const Vec2f& st);
    virtual void CollectStats(StatsManager& stats);

    static string serialize_typename();
    virtual void serialize(Archive* a);

protected:
    Image<T>        image;
    string	        filename;
    T               average;
    bool		    flipY;

    bool linear, mipmap, tile;

    void _Init();
};

typedef ImageTexture<float> ImageTextureF;
typedef ImageTexture<Vec3f> ImageTextureV;

template<typename T>
class ConstTexture : public Texture<T>
{
public:
    ConstTexture() {}
    ConstTexture(const T &v) : _value(v) {}

    virtual T Sample(const Vec2f& st) { return _value; }
    virtual void CollectStats(StatsManager& stats)
    {
        StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "ConstTexture");
        stat->Increment();
    }

    T Average() const { return _value; }
    static string serialize_typename();
    virtual void serialize(Archive* a);

    T _value;
};

typedef ConstTexture<float> ConstTextureF;
typedef ConstTexture<Vec3f> ConstTextureV;

template<typename T, typename T2>
class ScaleTexture : public Texture<T>
{
public:
    ScaleTexture() {}
    ScaleTexture(shared_ptr<Texture<T> > tex1, shared_ptr<Texture<T2> > tex2) : _tex1(tex1), _tex2(tex2) {}

    virtual T Sample(const Vec2f& st) { return _tex1->Sample(st) * _tex2->Sample(st); }
    virtual void CollectStats(StatsManager& stats)
    {
        StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "ScaleTexture");
        stat->Increment();
    }

    T Average() const { return _tex1->Average() * _tex2->Average(); }
    static string serialize_typename();
    virtual void serialize(Archive* a);

    shared_ptr<Texture<T> > _tex1;
    shared_ptr<Texture<T2> > _tex2;
};

typedef ScaleTexture<float, float> ScaleTextureF;
typedef ScaleTexture<Vec3f, Vec3f> ScaleTextureV;
typedef ScaleTexture<Vec3f, float> ScaleTextureVF;

class CubeTexture : public SceneObject {
public:
	CubeTexture();
    CubeTexture(const CubeMap<Vec3f>& m, const string& imfile, bool linear, bool mipmap);

    CubeMap<Vec3f>& CubeMapRef() { return map; }
	bool& Linear() { return linear; }
	bool& Mipmap() { return mipmap; }

    Vec3f Average() const { return average; }

	virtual Vec3f Sample(const Vec3f& v);

    virtual void CollectStats(StatsManager& stats);

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    CubeMap<Vec3f>  map;
    Vec3f			average;
	string		    filename;

	bool linear, mipmap;

	void _Init() ;
};

#include "texture_impl.h"
#endif
