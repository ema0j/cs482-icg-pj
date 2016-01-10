#include "imageio.h"

#include <float.h>
#ifdef __APPLE__
#include <FreeImage.h>
#else
#include <FreeImage/FreeImage.h>
#endif

static void _Init() {
	static bool _initialized = false;

	if(_initialized) return;

	_initialized = true;
	FreeImage_Initialise();
}

template<class T>
static FIBITMAP* _toFreeImage(const Image<T>* image, bool flipY) {
	assert(0);
    return 0;
}

template<>
FIBITMAP* _toFreeImage<Vec4f>(const Image<Vec4f>* image, bool flipY) {
	FIBITMAP* fi = FreeImage_AllocateT(FIT_RGBAF, image->Width(), image->Height());

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBAF* fiScanline = (FIRGBAF*)FreeImage_GetScanLine(fi, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            fiScanline[i].red = image->ElementAt(i,j)[0];
            fiScanline[i].green = image->ElementAt(i,j)[1];
            fiScanline[i].blue = image->ElementAt(i,j)[2];
            fiScanline[i].alpha = image->ElementAt(i,j)[3];
        }
    }
	if(flipY) FreeImage_FlipVertical(fi);

	return fi;
}

template<>
FIBITMAP* _toFreeImage<Vec3f>(const Image<Vec3f>* image, bool flipY) {
	FIBITMAP* fi = FreeImage_AllocateT(FIT_RGBF, image->Width(), image->Height());

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBF* fiScanline = (FIRGBF*)FreeImage_GetScanLine(fi, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            fiScanline[i].red = image->ElementAt(i,j)[0];
            fiScanline[i].green = image->ElementAt(i,j)[1];
            fiScanline[i].blue = image->ElementAt(i,j)[2];
        }
    }
	if(flipY) FreeImage_FlipVertical(fi);

	return fi;
}

template<>
FIBITMAP* _toFreeImage<Spectrum3f>(const Image<Spectrum3f>* image, bool flipY) {
	FIBITMAP* fi = FreeImage_AllocateT(FIT_RGBF, image->Width(), image->Height());

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBF* fiScanline = (FIRGBF*)FreeImage_GetScanLine(fi, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            fiScanline[i].red = image->ElementAt(i,j)[0];
            fiScanline[i].green = image->ElementAt(i,j)[1];
            fiScanline[i].blue = image->ElementAt(i,j)[2];
        }
    }
	if(flipY) FreeImage_FlipVertical(fi);

	return fi;
}

template<>
FIBITMAP* _toFreeImage<uint32_t>(const Image<uint32_t>* image, bool flipY) {
    FIBITMAP* fi = FreeImage_AllocateT(FIT_RGBAF, image->Width(), image->Height());

    for(uint32_t j = 0; j < image->Height(); j ++) {
        // FreeImage is already flipped
        FIRGBAF* fiScanline = (FIRGBAF*)FreeImage_GetScanLine(fi, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            uint32_t v = image->ElementAt(i,j);
            fiScanline[i].red = static_cast<float>(v);
            fiScanline[i].green = static_cast<float>(v);
            fiScanline[i].blue = static_cast<float>(v);
            fiScanline[i].alpha = static_cast<float>(v);
        }
    }
    if(flipY) FreeImage_FlipVertical(fi);

    return fi;
}

template<>
FIBITMAP* _toFreeImage<float>(const Image<float>* image, bool flipY) {
	FIBITMAP* fi = FreeImage_AllocateT(FIT_RGBAF, image->Width(), image->Height());

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBAF* fiScanline = (FIRGBAF*)FreeImage_GetScanLine(fi, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            float f = image->ElementAt(i,j);
            if(f == FLT_MAX) {
				fiScanline[i].red = 0;
				fiScanline[i].green = 0;
				fiScanline[i].blue = 0;
				fiScanline[i].alpha = 0;
			} else {
				fiScanline[i].red = f;
				fiScanline[i].green = f;
				fiScanline[i].blue = f;
				fiScanline[i].alpha = f;
			}
        }
    }
	if(flipY) FreeImage_FlipVertical(fi);

	return fi;
}

template<class T>
static void _SaveFreeImage(const string& filename, const Image<T>* image, 
						   bool flipY, FREE_IMAGE_FORMAT fifType) {
	_Init();
	
	FIBITMAP* fi = _toFreeImage(image, flipY);

	FreeImage_Save(fifType, fi, filename.c_str());

	FreeImage_Unload(fi);
}

void ImageIO::Save(const string& filename, const Image<Vec4f>* image) {
	_SaveFreeImage(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

void ImageIO::Save(const string& filename, const Image<uint32_t>* image) {
    _SaveFreeImage(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

void ImageIO::Save(const string& filename, const Image<Vec3f>* image) {
	_SaveFreeImage(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

void ImageIO::Save(const string& filename, const Image<float>* image) {
	_SaveFreeImage(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

void ImageIO::Save(const string& filename, const Image<Spectrum3f>* image) {
	_SaveFreeImage(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

template<class T>
FIBITMAP* _toFreeImageLDR(const Image<T>* image, bool flipY) {
	assert(0);
    return 0;
}

template<>
FIBITMAP* _toFreeImageLDR<Vec4f>(const Image<Vec4f>* image, bool flipY) {
	FIBITMAP* fi = FreeImage_Allocate(image->Width(), image->Height(), 24);

	for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		RGBTRIPLE* fiScanline = (RGBTRIPLE*)FreeImage_GetScanLine(fi, image->Height()-j-1);
		for(uint32_t i = 0; i < image->Width(); i ++) {
			fiScanline[i].rgbtRed = (unsigned char)(clamp(image->ElementAt(i,j)[0],0.0f,1.0f)*255+0.4999f);
			fiScanline[i].rgbtGreen = (unsigned char)(clamp(image->ElementAt(i,j)[1],0.0f,1.0f)*255+0.4999f);
			fiScanline[i].rgbtBlue = (unsigned char)(clamp(image->ElementAt(i,j)[2],0.0f,1.0f)*255+0.4999f);
		}
	}
	if(flipY) FreeImage_FlipVertical(fi);

	return fi;
}

template<>
FIBITMAP* _toFreeImageLDR<Vec3f>(const Image<Vec3f>* image, bool flipY) {
	FIBITMAP* fi = FreeImage_Allocate(image->Width(), image->Height(), 24);

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		RGBTRIPLE* fiScanline = (RGBTRIPLE*)FreeImage_GetScanLine(fi, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            fiScanline[i].rgbtRed = (unsigned char)(clamp(image->ElementAt(i,j)[0],0.0f,1.0f)*255+0.4999f);
            fiScanline[i].rgbtGreen = (unsigned char)(clamp(image->ElementAt(i,j)[1],0.0f,1.0f)*255+0.4999f);
            fiScanline[i].rgbtBlue = (unsigned char)(clamp(image->ElementAt(i,j)[2],0.0f,1.0f)*255+0.4999f);
        }
    }
	if(flipY) FreeImage_FlipVertical(fi);

	return fi;
}

template<class T>
static void _SaveFreeImageLDR(const string& filename, const Image<T>* image, 
						      bool flipY, FREE_IMAGE_FORMAT fifType) {
	_Init();
	
	FIBITMAP* fi = _toFreeImageLDR(image, flipY);

	FreeImage_Save(fifType, fi, filename.c_str());

	FreeImage_Unload(fi);
}

void ImageIO::SaveLDR(const string& filename, const Image<Vec4f>* image) {
	_SaveFreeImageLDR(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

void ImageIO::SaveLDR(const string& filename, const Image<Vec3f>* image) {
	_SaveFreeImageLDR(filename, image, false, FreeImage_GetFIFFromFilename(filename.c_str()));
}

template<class T>
Image<T>* _fromFreeImage(FIBITMAP* fi) { assert(0); }

template<>
Image<float>* _fromFreeImage<float>(FIBITMAP* fi) {
    FIBITMAP* fif = FreeImage_ConvertToRGBF(fi);

    Image<float>* image = new Image<float>(FreeImage_GetWidth(fif),FreeImage_GetHeight(fif));

    for(uint32_t j = 0; j < image->Height(); j ++) {
        // FreeImage is already flipped
        FIRGBF* fiScanline = (FIRGBF*)FreeImage_GetScanLine(fif, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            Vec3f v(fiScanline[i].red, fiScanline[i].green, fiScanline[i].blue);
            image->ElementAt(i, j) = v.Average();
        }
    }

    FreeImage_Unload(fif);

    return image;
}

template<>
Image<Vec4f>* _fromFreeImage<Vec4f>(FIBITMAP* fi) {
	FIBITMAP* fif = FreeImage_ConvertToRGBF(fi);
	if(FreeImage_GetColorType(fi) == FIC_RGBALPHA && FreeImage_GetColorType(fi) != FIC_RGBALPHA) {
		// BUG - This should not loose the alpha channel!!!
		printf("alpha lost in translation during image load\n");
	}

    Image<Vec4f>* image = new Image<Vec4f>(FreeImage_GetWidth(fif),FreeImage_GetHeight(fif));

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBF* fiScanline = (FIRGBF*)FreeImage_GetScanLine(fif, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            image->ElementAt(i,j)[0] = fiScanline[i].red;
            image->ElementAt(i,j)[1] = fiScanline[i].green;
            image->ElementAt(i,j)[2] = fiScanline[i].blue;
            image->ElementAt(i,j)[3] = 1;
        }
    }

	FreeImage_Unload(fif);

	return image;
}

template<>
Image<Vec3f>* _fromFreeImage<Vec3f>(FIBITMAP* fi) {
	FIBITMAP* fif = FreeImage_ConvertToRGBF(fi);

    Image<Vec3f>* image = new Image<Vec3f>(FreeImage_GetWidth(fif),FreeImage_GetHeight(fif));

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBF* fiScanline = (FIRGBF*)FreeImage_GetScanLine(fif, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            image->ElementAt(i,j)[0] = fiScanline[i].red;
            image->ElementAt(i,j)[1] = fiScanline[i].green;
            image->ElementAt(i,j)[2] = fiScanline[i].blue;
        }
    }

	FreeImage_Unload(fif);

	return image;
}

template<>
Image<Spectrum3f>* _fromFreeImage<Spectrum3f>(FIBITMAP* fi) {
	FIBITMAP* fif = FreeImage_ConvertToRGBF(fi);

    Image<Spectrum3f>* image = new Image<Spectrum3f>(FreeImage_GetWidth(fif),FreeImage_GetHeight(fif));

    for(uint32_t j = 0; j < image->Height(); j ++) {
		// FreeImage is already flipped
		FIRGBF* fiScanline = (FIRGBF*)FreeImage_GetScanLine(fif, image->Height()-j-1);
        for(uint32_t i = 0; i < image->Width(); i ++) {
            image->ElementAt(i,j)[0] = fiScanline[i].red;
            image->ElementAt(i,j)[1] = fiScanline[i].green;
            image->ElementAt(i,j)[2] = fiScanline[i].blue;
        }
    }

	FreeImage_Unload(fif);

	return image;
}

template<class T>
Image<T>* _LoadFreeImage(const string& filename, bool flipY) {
	_Init();

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str());
	FIBITMAP* fi = FreeImage_Load(format, filename.c_str());
	if(flipY) FreeImage_FlipVertical(fi);

	Image<T>* image = _fromFreeImage<T>(fi);

	FreeImage_Unload(fi);

    return image;
}

Image<float>* ImageIO::LoadF( const string& filename )
{
    return _LoadFreeImage<float>(filename, false);
}

Image<Vec4f>* ImageIO::LoadRGBAF(const string& filename) {
	return _LoadFreeImage<Vec4f>(filename, false);
}

Image<Vec3f>* ImageIO::LoadRGBF(const string& filename) {
	return _LoadFreeImage<Vec3f>(filename, false);
}

Image<Spectrum3f>* ImageIO::LoadSpectrum3F(const string& filename) {
    return _LoadFreeImage<Spectrum3f>(filename, false);
}

template<class T>
CubeMap<T>* _LoadCubeFreeImage(const string& filename, bool flipY) {
	Image<T>* i = _LoadFreeImage<T>(filename, flipY);
	CubeMap<T>* m = new CubeMap<T>();

	// check horizontal cross
	if(i->Width() / 4 == i->Height() / 3) m->FromHorizontalCross(*i);
	else if(i->Width() / 3 == i->Height() / 4) m->FromVerticalCross(*i);
	else { printf("cube format not supported\n"); }

	delete i;

	return m;
}

CubeMap<Vec3f>* ImageIO::LoadCubeRGBF(const string& filename) {
	return _LoadCubeFreeImage<Vec3f>(filename, false);
}

CubeMap<Vec4f>* ImageIO::LoadCubeRGBAF(const string& filename) {
	return _LoadCubeFreeImage<Vec4f>(filename, false);
}

CubeMap<Spectrum3f>* ImageIO::LoadCubeSpectrum3F(const string& filename) {
	return _LoadCubeFreeImage<Spectrum3f>(filename, false);
}

void ImageIO::SaveSparse(const string& filename, const SparseImage<Vec3f>* image) {
	FILE* f = fopen(filename.c_str(), "wb");

	SaveSparse(f, image);

	fclose(f);
}

void ImageIO::SaveSparse(FILE* f, const SparseImage<Vec3f>* image) {
	uint32_t w = image->Width(); uint32_t h = image->Height(); uint32_t l = image->SparseSize();
	fwrite(&w, sizeof(uint32_t), 1, f);
	fwrite(&h, sizeof(uint32_t), 1, f);
	fwrite(&l, sizeof(uint32_t), 1, f);
	fwrite(image->GetDataPointer(), sizeof(float), 3*image->SparseSize(), f);
	fwrite(image->GetIndexPointer(), sizeof(uint32_t), image->SparseSize(), f);
}

SparseImage<Vec3f>* ImageIO::LoadSparseRGBF(const string& filename) {
	FILE* f = fopen(filename.c_str(), "rb");

	SparseImage<Vec3f>* image = LoadSparseRGBF(f);

	fclose(f);	

	return image;
}

SparseImage<Vec3f>* ImageIO::LoadSparseRGBF(FILE* f) {
	uint32_t w, h, l;
	fread(&w, sizeof(uint32_t), 1, f);
	fread(&h, sizeof(uint32_t), 1, f);
	fread(&l, sizeof(uint32_t), 1, f);

	SparseImage<Vec3f>* image = new SparseImage<Vec3f>(w,h,l);
	fread(image->GetDataPointer(), sizeof(float), 3*image->SparseSize(), f);
	fread(image->GetIndexPointer(), sizeof(uint32_t), image->SparseSize(), f);

	return image;
}

void ImageIO::SaveSparseCube(const string& filename, const SparseCubeMap<Vec3f>* cubeMap) {
    FILE* f = fopen(filename.c_str(), "wb");
	SaveSparseCube(f, cubeMap);
	fclose(f);
}

void ImageIO::SaveSparseCube(FILE *f, const SparseCubeMap<Vec3f>* cubeMap) {
	uint32_t r = cubeMap->Resolution();
	fwrite(&r, sizeof(uint32_t), 1, f);
	for(uint32_t ff = 0; ff < 6; ff ++) {
		const SparseImage<Vec3f>* image = &cubeMap->Face(ff);
		uint32_t l = image->SparseSize();
		fwrite(&l, sizeof(int), 1, f);
		fwrite(image->GetDataPointer(), sizeof(float), 3*image->SparseSize(), f);
		fwrite(image->GetIndexPointer(), sizeof(uint32_t), image->SparseSize(), f);
	}
}

SparseCubeMap<Vec3f>* ImageIO::LoadSparseCubeRGBF(const string& filename) {
    FILE* f = fopen(filename.c_str(), "rb");
	SparseCubeMap<Vec3f>* cubeMap = LoadSparseCubeRGBF(f);
	fclose(f);
    return cubeMap;
}

SparseCubeMap<Vec3f>* ImageIO::LoadSparseCubeRGBF(FILE *f) {
	SparseCubeMap<Vec3f>* cubeMap = new SparseCubeMap<Vec3f>();
    LoadSparseCubeRGBF(f, cubeMap);
    return cubeMap;
}

void ImageIO::LoadSparseCubeRGBF(FILE *f, SparseCubeMap<Vec3f>* cubeMap) {
	uint32_t r;
	fread(&r, sizeof(uint32_t), 1, f);
	for(uint32_t ff = 0; ff < 6; ff ++) {
		uint32_t l;
		fread(&l, sizeof(uint32_t), 1, f);
		cubeMap->Face(ff).Alloc(r,r,l);
		fread(cubeMap->Face(ff).GetDataPointer(), sizeof(float), 3*cubeMap->Face(ff).SparseSize(), f);
		fread(cubeMap->Face(ff).GetIndexPointer(), sizeof(uint32_t), cubeMap->Face(ff).SparseSize(), f);
	}
}


