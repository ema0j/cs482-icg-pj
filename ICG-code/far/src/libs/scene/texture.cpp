#include "texture.h"
#include <misc/stats.h>
#include <imageio/imageio.h>

template<> Vec3f ImageTexture<Vec3f>::Sample(const Vec2f& st) 
{
    // bilinear sampling with tiling
    return image.Sample(st[0],st[1],linear,tile).ClampMin(Vec3f::Zero());
}

template<> float ImageTexture<float>::Sample(const Vec2f& st)
{
    // bilinear sampling with tiling
    return max(image.Sample(st[0],st[1],linear,tile), 0.0f);
}

CubeTexture::CubeTexture() {
	linear = true;
	mipmap = false;
	filename = "";
}

CubeTexture::CubeTexture(const CubeMap<Vec3f>& m, const string& imfile, bool linear, bool mipmap) {
	this->linear = linear;
	this->mipmap = mipmap;

	filename = imfile;
    map.Copy(m);
	_Init();
}

Vec3f CubeTexture::Sample(const Vec3f& v) {
	return map.Sample(v,linear);
}

void CubeTexture::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "CubeTextures");
    stat->Increment();
}

string CubeTexture::serialize_typename() { return "CubeTexture"; } 

void CubeTexture::serialize(Archive* a) {
	a->member("filename", filename);
	a->optional("linear", linear, true);
	a->optional("mipmap", mipmap, false);

	if(a->isreading()) {
		CubeMap<Vec3f> *m = ImageIO::LoadCubeRGBF(filename);
		if(m->Resolution() == 0) cerr << "cannot load texture " << filename << endl << flush;
		map.Copy(*m);
		delete m;
		_Init();
	}
}

void CubeTexture::_Init() {
	average = map.Average();
}
