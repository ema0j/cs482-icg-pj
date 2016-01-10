#ifndef _IMAGEIO_H_
#define _IMAGEIO_H_

#include <vmath/vec4.h>
#include <vmath/spectrum3.h>
#include <image/image.h>
#include <image/sparseImage.h>
#include <image/cubemap.h>
#include <image/sparseCubeMap.h>
#include <stdio.h>
#include <misc/stdcommon.h>

class ImageIO {
public:
    template<class T>
    static void Save(const string& filename, shared_ptr<Image<T> > image) { Save(filename, image.get()); }
    template<class T>
    static void Save(const string& filename, const Image<T> &image) { Save(filename, &image); }
    static void Save(const string& filename, const Image<Vec4f>* image);
    static void Save(const string& filename, const Image<uint32_t>* image);
    static void Save(const string& filename, const Image<Vec3f>* image);
    static void Save(const string& filename, const Image<float>* image);
    static void Save(const string& filename, const Image<Spectrum3f>* image);

	template<class T>
	static void SaveLDR(const string& filename, shared_ptr<Image<T> > image) { SaveLDR(filename, image.get()); }
    static void SaveLDR(const string& filename, const Image<Vec3f>* image);
	static void SaveLDR(const string& filename, const Image<Vec4f>* image);

	static void SaveSparse(const string& filename, const SparseImage<Vec3f>* image);
	static void SaveSparse(FILE* f, const SparseImage<Vec3f>* image);

	static void SaveSparseCube(const string& filename, const SparseCubeMap<Vec3f>* cubeMap);
	static void SaveSparseCube(FILE* f, const SparseCubeMap<Vec3f>* cubeMap);

    template<typename T>
    static Image<T>* Load(const string& filename);
    static Image<float>* LoadF(const string& filename);
    static Image<Vec4f>* LoadRGBAF(const string& filename);
    static Image<Vec3f>* LoadRGBF(const string& filename);
    static Image<Spectrum3f>* LoadSpectrum3F(const string& filename);

	static CubeMap<Vec4f>* LoadCubeRGBAF(const string& filename);
	static CubeMap<Vec3f>* LoadCubeRGBF(const string& filename);
	static CubeMap<Spectrum3f>* LoadCubeSpectrum3F(const string& filename);

    static SparseImage<Vec3f>* LoadSparseRGBF(const string& filename);
    static SparseImage<Vec3f>* LoadSparseRGBF(FILE* f);

    static SparseCubeMap<Vec3f>* LoadSparseCubeRGBF(const string& filename);
    static SparseCubeMap<Vec3f>* LoadSparseCubeRGBF(FILE* f);
    static void LoadSparseCubeRGBF(FILE* f, SparseCubeMap<Vec3f>* cubemap);
};

template<typename T>
inline Image<T>* ImageIO::Load(const string& filename)
{
    //return LoadF(filename);
}


template<>
inline Image<float>* ImageIO::Load(const string& filename)
{
    return LoadF(filename);
}

template<>
inline Image<Vec3f>* ImageIO::Load(const string& filename)
{
    return LoadRGBF(filename);
}

template<>
inline Image<Vec4f>* ImageIO::Load(const string& filename)
{
    return LoadRGBAF(filename);
}

template<>
inline Image<Spectrum3f>* ImageIO::Load(const string& filename)
{
    return LoadSpectrum3F(filename);
}

#endif
