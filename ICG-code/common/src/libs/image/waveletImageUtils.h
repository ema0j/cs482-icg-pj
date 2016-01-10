#ifndef _WAVELETIMAGEUTILS_H_
#define _WAVELETIMAGEUTILS_H_

#include "image.h"
#include <vmath/vec3.h>

// ATTENTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// support only sqaure images right now!!!
// no checks right now !!!

template<class T>
inline void ImageToHaar2DSquareWavelet(Image<T>& wavelet, const Image<T>& image, int maxRecurse = -1) {
	Image<T> aux(image.Width(), image.Height());

	wavelet.Set(image);
	int w = image.Width(); 
    int h = image.Height();
	
    int recurse = 0;
	while(w > 0 && (maxRecurse < 0 || recurse < maxRecurse)) 
    {
		aux.SetSubImage(wavelet, 0, 0, 0, 0, w, h);
		for(int j = 0; j < h/2; j ++) {
			for(int i = 0; i < w/2; i ++) {
				T v00 = aux.ElementAt(i*2+0,j*2+0);
				T v10 = aux.ElementAt(i*2+1,j*2+0);
				T v01 = aux.ElementAt(i*2+0,j*2+1);
				T v11 = aux.ElementAt(i*2+1,j*2+1);

				wavelet.ElementAt(i    ,j    ) = (v00 + v01 + v10 + v11) / 2;
				wavelet.ElementAt(i+w/2,j    ) = (v00 + v01 - v10 - v11) / 2;
				wavelet.ElementAt(i    ,j+h/2) = (v00 - v01 + v10 - v11) / 2;
				wavelet.ElementAt(i+w/2,j+h/2) = (v00 - v01 - v10 + v11) / 2;
			}
		}

		w = w/2;
		h = h/2;

		recurse ++;
	}
}

template<class T>
inline void ImageToHaar1DWaveletHorizontal(Image<T>& wavelet, const Image<T>& image, int maxRecurse = -1) {
    Image<T> aux(image.Width(), image.Height());

    wavelet.Set(image);
    int w = image.Width(); 
    int h = image.Height();

    int recurse = 0;
    while(w > 0 && (maxRecurse < 0 || recurse < maxRecurse)) 
    {
        aux.SetSubImage(wavelet, 0, 0, 0, 0, w, h);
        for(int i = 0; i < w/2; i ++) 
        {
            for(int j = 0; j < h; j ++) 
            {
                T v0 = aux.ElementAt(i*2+0,j);
                T v1 = aux.ElementAt(i*2+1,j);

                wavelet.ElementAt(i    , j) = (v0 + v1) / 2;
                wavelet.ElementAt(i+w/2, j) = (v0 - v1) / 2;
            }
        }
        w = w/2;
        recurse ++;
    }
}

template<class T>
inline void ImageToHaar1DWaveletVertical(Image<T>& wavelet, const Image<T>& image, int maxRecurse = -1) {
    Image<T> aux(image.Width(), image.Height());

    wavelet.Set(image);
    int w = image.Width(); 
    int h = image.Height();

    int recurse = 0;
    while(w > 0 && (maxRecurse < 0 || recurse < maxRecurse)) 
    {
        aux.SetSubImage(wavelet, 0, 0, 0, 0, w, h);
        for(int j = 0; j < h/2; j ++) {
            for(int i = 0; i < w; i ++) {
                T v0 = aux.ElementAt(i, j*2+0);
                T v1 = aux.ElementAt(i, j*2+1);

                wavelet.ElementAt(i    ,j    ) = (v0 + v1) / 2;
                wavelet.ElementAt(i    ,j+h/2) = (v0 - v1) / 2;
            }
        }
        h = h/2;
        recurse ++;
    }
}

template<class T>
inline void Haar2DSquareWaveletToImage(Image<T>& image, const Image<T>& wavelet, int maxRecurse = -1) {
	Image<T> aux(image.Width(), image.Height());

	uint32_t w = 2; uint32_t h = 2;
	image.Set(wavelet);
	while(w <= aux.Width()) {
		aux.SetSubImage(image, 0, 0, 0, 0, w, h);
		for(uint32_t j = 0; j < h/2; j ++) {
			for(uint32_t i = 0; i < w/2; i ++) {
				T w00 = aux.ElementAt(i    ,j    );
				T w10 = aux.ElementAt(i+w/2,j    );
				T w01 = aux.ElementAt(i    ,j+h/2);
				T w11 = aux.ElementAt(i+w/2,j+h/2);

				image.ElementAt(i*2+0,j*2+0) = (w00 + w01 + w10 + w11) / 2;
				image.ElementAt(i*2+0,j*2+1) = (w00 - w01 + w10 - w11) / 2;
				image.ElementAt(i*2+1,j*2+0) = (w00 + w01 - w10 - w11) / 2;
				image.ElementAt(i*2+1,j*2+1) = (w00 - w01 - w10 + w11) / 2;
			}
		}

		w = w*2;
		h = h*2;
	}
}

inline uint32_t WaveletCompress(Image<Vec3f>& wavelet, float epsilon) {
	uint32_t count = 0;
	for(uint32_t i = 0; i < wavelet.Size(); i ++) {
		if(wavelet[i].GetLength() < epsilon){ 
			wavelet[i] = 0; 
		}else { 
			++count; 
		}
	}
	return count;
}

inline uint32_t WaveletCompress(Image<Vec4f>& wavelet, float epsilon) {
    uint32_t count = 0;
    for(uint32_t i = 0; i < wavelet.Size(); i ++) {
        if(wavelet[i].IgnoreLast().GetLength() < epsilon){ 
            wavelet[i] = 0; 
        }else { 
            ++count; 
        }
    }
    return count;
}

#endif
