#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <vmath/vec2.h>
#include <vmath/functions.h>
#include <cmath>
#include <assert.h>

#pragma warning ( push ) 
#pragma warning (disable: 4244)

inline int __tile(int x, int w) {
    x %= w;
    if(x < 0) x += w;
    return x;
}

template<class T>
class Image {
public:
    Image() {
        w = 0;
        h = 0;
        d = 0;
    }

    Image(uint32_t nw, uint32_t nh) {
        w = nw;
        h = nh;
        d = new T[w*h];
    }

	Image(const Image<T>& img) {
        w = img.Width();
        h = img.Height();
        d = new T[w*h];
		Set(img);
	}

    ~Image() {
        Clear();
    }

	void Copy(const Image<T>& img) {
		Alloc(img.w,img.h);
		Set(img);
	}

    void Alloc(uint32_t nw, uint32_t nh) {
    	if(w == nw && h == nh) return;
		Clear();
		w = nw;
		h = nh;
		d = new T[w*h];
    }

    void Clear() {
        if(d) delete[] d;
        d = 0;
        w = h = 0;
    }

    inline T& ElementAt(uint32_t i, uint32_t j) {
        assert(w*j+i >=0 && w*j+i < w*h && j >= 0 && j < h && i >=0 && i < w);
        return d[w*j+i];
    }

    inline T& ElementAt(uint32_t idx) {
        assert(idx >=0 && idx < w*h);
        return d[idx];
    }

    inline const T& ElementAt(uint32_t i, uint32_t j) const {
        assert(w*j+i >=0 && w*j+i < w*h && j >= 0 && j < h && i >=0 && i < w);
        return d[w*j+i];
    }
    
    inline const T& ElementAt(uint32_t idx) const {
        assert(idx >=0 && idx < w*h);
        return d[idx];
    }
    
    T Lookup(uint32_t i, uint32_t j) const {
        if(i < 0 || i > w-1 || j < 0 || j > h-1) return T(); // BUG: should this be 0
        else return ElementAt(i,j);
    }
    
    T Sample(float u, float v, bool linear, bool tile, float lookupPixelOffset = 0) const {
		u += lookupPixelOffset / w;
		v += lookupPixelOffset / h;
        if(linear) {
            int im = (int)floor(u * w); int iM = im + 1;
            int jm = (int)floor(v * h); int jM = jm + 1;
            float iw = u*w - im;
            float jw = v*h - jm;
            if(tile) {
                im = __tile(im,w); iM = __tile(iM,w); 
                jm = __tile(jm,h); jM = __tile(jM,h); 
            }
            return Lookup(im, jm) * (1-iw)*(1-jw) + 
                   Lookup(im,jM) * (1-iw)*jw + 
                   Lookup(iM,jm) * iw*(1-jw) + 
                   Lookup(iM,jM) * iw*jw;
        } else {
			int i = (int)floor(u * w);
            int j = (int)floor(v * h);
            if(tile) { i = __tile(i,w); j = __tile(j, h); }
            return Lookup(i,j);
        }
    }

    T SampleClamped(float u, float v, bool linear, float lookupPixelOffset = 0) const {
		u += lookupPixelOffset / w;
		v += lookupPixelOffset / h;
        if(linear) {
            uint32_t im = (uint32_t)floor(u * w); uint32_t iM = im + 1;
            uint32_t jm = (uint32_t)floor(v * h); uint32_t jM = jm + 1;
            float iw = u*w - im;
            float jw = v*h - jm;
            im = clamp(im,0u,Width()-1); iM = clamp(iM,0u,Width()-1);
            jm = clamp(jm,0u,Height()-1); jM = clamp(jM,0u,Height()-1);
            return Lookup(im, jm) * (1-iw)*(1-jw) + 
                   Lookup(im,jM) * (1-iw)*jw + 
                   Lookup(iM,jm) * iw*(1-jw) + 
                   Lookup(iM,jM) * iw*jw;
        } else {
			uint32_t i = (uint32_t)floor(u * w);
            uint32_t j = (uint32_t)floor(v * h);
            i = clamp(i,0u,Width()-1);
            j = clamp(j,0u,Height()-1);
            return Lookup(i,j);
        }
    }
    
    inline void GetIndices(uint32_t idx, uint32_t* i, uint32_t *j) const { *i = idx % w; *j = idx / w; }
    inline uint32_t GetIndex(uint32_t i, uint32_t j) const { return w*j+i; }
    inline bool Inside(uint32_t i, uint32_t j) const { return i >= 0 && i < w && j >= 0 && j < h; }

    inline uint32_t Width() const { return w; }
    inline uint32_t Height() const { return h; }
    inline uint32_t Size() const { return w*h; }

    inline T* GetDataPointer() const { return d; }

    inline T& operator[](uint32_t idx) {
        return d[idx];
    }

    inline const T& operator[](uint32_t idx) const {
        return d[idx];
    }

	void DownSample(uint32_t factor) {
		if(factor <= 1) return;
        //assert((w / factor) * factor == w && (h / factor) * factor == h);

		Image<T> aux(*this);
		Alloc(w/factor,h/factor);

		for(uint32_t j = 0; j < h; j ++) {
			for(uint32_t i = 0; i < w; i ++) {
				ElementAt(i,j) = 0;
				for(uint32_t jj = 0; jj < factor; jj ++) {
					for(uint32_t ii = 0; ii < factor; ii ++) {
						ElementAt(i,j) += aux.ElementAt(i*factor+ii,j*factor+jj) / float(factor*factor);
					}
				}
			}
		}
	}
    
	void UpSample(uint32_t factor) {
		if(factor <= 1) return;

		Image<T> aux(*this);
		Alloc(w*factor,h*factor);

		for(uint32_t j = 0; j < h; j ++) {
			for(uint32_t i = 0; i < w; i ++) {
                ElementAt(i,j) = aux.ElementAt(i/factor,j/factor);
            }
		}
	}
    
    void Downsample2() {
        if(w == 1 && h == 1) return;
        
        assert(isPow2(w) && isPow2(h));
        
        Image<T> aux(*this);
        Alloc(w/2,h/2);
        
        if(w >= 2 && h >= 2) {
            for(uint32_t j = 0; j < h; j ++) {
                for(uint32_t i = 0; i < w; i ++) {
                    ElementAt(i,j) = ( 
                                      aux.ElementAt(i*2+0,j*2+0) +
                                      aux.ElementAt(i*2+0,j*2+1) + 
                                      aux.ElementAt(i*2+1,j*2+0) +
                                      aux.ElementAt(i*2+1,j*2+1) 
                                      ) / 4;
                }
            }            
        } else {
            if(w == 1) {
                for(uint32_t j = 0; j < h; j ++) {
                    for(uint32_t i = 0; i < w; i ++) {
                        ElementAt(i,j) = (aux.ElementAt(i*2+0,j*2+0) + aux.ElementAt(i*2+0,j*2+1)) / 2;
                    }
                }                
            }
        }
    }

    void Rotate(float phi, uint32_t samples = 3) {
        Image<T> aux = *this;
        for(int j = 0; j < h; j ++) {
            for(int i = 0; i < w; i ++) {
                ElementAt(i,j) = 0;
                for(int jj = 0; jj < samples; jj ++) {
                    for(int ii = 0; ii < samples; ii ++) {
                        Vec2f uv((i+(ii+0.5f)/samples)/w, (j+(jj+0.5f)/samples)/h);
                        uv = uv*2-1;
                        Vec2f uvRotated = Vec2f(cos(phi)*uv.x - sin(phi)*uv.y, sin(phi)*uv.x - cos(phi)*uv.y);
                        ElementAt(i,j) += aux.Sample(uvRotated.x, uvRotated.y, true, false);
                    }
                }
                ElementAt(i,j) /= (float)samples*samples;
            }
        }
    }

	void Set(const T& v) { for(uint32_t i = 0; i < w*h; i ++) d[i] = v; }
	void Set(const T& v, const Image<float>& mask) { 
		Alloc(mask.Width(),mask.Height());
        for(uint32_t i = 0; i < w*h; i ++) d[i] = v * mask.ElementAt(i) + d[i] * (1-mask.ElementAt(i));
    }
	void Set(const Image<T>& src) {
		Alloc(src.w,src.h);
		for(uint32_t i = 0; i < w*h; i ++) d[i] = src.d[i];
	}
    template<class TT>
	void SetToAverage(const Image<TT>& src) {
		Alloc(src.Width(),src.Height());
		for(uint32_t i = 0; i < w*h; i ++) d[i] = src.ElementAt(i).Average();
	}
	void Set(const Image<T>& src, const Image<float>& mask) {
        assert(src.Width() == mask.Width() && src.Height() == mask.Height());
		Alloc(src.w,src.h);
		for(uint32_t i = 0; i < w*h; i ++) d[i] = src.d[i] * mask.ElementAt(i) + d[i] * (1-mask.ElementAt(i));
	}
	void SetSubImage(const T& v, int dest_x, int dest_y, uint32_t sw, uint32_t sh) {
		assert(dest_x >= 0 && dest_y >= 0 && dest_x+sw <= w && dest_y+sh <= h); // check dest

		for(uint32_t y = 0; y < sh; y ++) {
			for(uint32_t x = 0; x < sw; x ++) {
				_ElementAtUnchecked(dest_x+x,dest_y+y) = v;
			}
		}
	}
	void SetSubImage(const Image<T>& src, int dest_x, int dest_y, int src_x, int src_y, uint32_t sw, uint32_t sh) {
		assert(dest_x >= 0 && dest_y >= 0 && dest_x+sw <= w && dest_y+sh <= h); // check dest
		assert(src_x >= 0 && src_y >= 0 && src_x+sw <= src.w && src_y+sh <= src.h); // check src

		for(uint32_t y = 0; y < sh; y ++) {
			for(uint32_t x = 0; x < sw; x ++) {
				_ElementAtUnchecked(dest_x+x,dest_y+y) =
					src._ElementAtUnchecked(src_x+x,src_y+y);
			}
		}
	}

    inline Image<T> GetCropped(int x, int y, int w, int h) {
        Image<T> ret(w,h);
        ret.SetSubImage(*this, 0, 0, x, y, w, h);
        return ret;
    }

    inline Image<T>& operator=(const Image<T>& img) {
        Set(img);
        return *this;
    }

	void Scale(const T& v) { for(uint32_t i = 0; i < w*h; i ++) d[i] *= v; }
	void Scale(const Image<T>& v) { assert(w == v.w && h == v.h); for(uint32_t i = 0; i < w*h; i ++) d[i] *= v.d[i]; }
    Image<T> GetScaled(const T& v) { Image<T> ret(*this); ret.Scale(v); return ret; }
	T Average() const { T m = T(); for(uint32_t i = 0; i < w*h; i ++) m += d[i]; return m / (w*h); } // BUG: should the init be 0?
	T MaxElement() const { T m = d[0]; for(uint32_t i = 0; i < w*h; i ++) m = max(m,d[i]); return m; } // BUG: should the init be 0?

	void BilerpCorners(const T& v00, const T& v10, const T& v01, const T& v11) {
		for(uint32_t j = 0; j < h; j ++) {
			for(uint32_t i = 0; i < w; i ++) {
				float x = float(i+0.5f) / float(w);
				float y = float(j+0.5f) / float(h);
				// lerp in x
				T vx0 = v00 * (1-x) + v10 * x;
				T vx1 = v01 * (1-x) + v11 * x;
				// lerp in y
				T v = vx0 * (1-y) + vx1 * y;
				// set in image
				_ElementAtUnchecked(i,j) = v;
			}
		}
	}

	void Rotate90() {
		Image<T> aux(*this);
		Alloc(h,w);
		for(uint32_t j = 0; j < h; j ++) {
			for(uint32_t i = 0; i < w; i ++) {
				d[GetIndex(i,j)] = aux.d[aux.GetIndex(h-j-1,i)];
			}
		}
	}

	inline void Rotate180() {
		// not efficient
		Rotate90();
		Rotate90();
	}

	inline void Rotate270() {
		// not efficient
		Rotate90();
		Rotate90();
		Rotate90();
	}

    void Shift(int x, int y, bool toroidal) {
        // HACK: can be written more efficiently
        Image<T> copy = *this;
        if(toroidal) {
            for(uint32_t j = 0; j < Height(); j ++) {
                for(uint32_t i = 0; i < Width(); i ++) {
                    ElementAt(i,j) = copy.ElementAt((i+x+Width())%Width(), (j+y+Height())%Height());
                }
            }
        } else {
            for(uint32_t j = 0; j < Height(); j ++) {
                for(uint32_t i = 0; i < Width(); i ++) {
                    ElementAt(i,j) = copy.ElementAt(max(min(i+x,Width()-1),0), max(min(j+y,Height()-1),0));
                }
            }
        }
    }

	void FlipY() {
		for(uint32_t j = 0; j < h/2; j ++) {
			for(uint32_t i = 0; i < w; i ++) {
				swap(d[GetIndex(i,j)], d[GetIndex(i,h-j-1)]);
			}
		}
	}

protected:
    T* d;
    uint32_t w;
    uint32_t h;

	inline T& _ElementAtUnchecked(uint32_t i, uint32_t j) {
		return d[w*j+i];
	}

	inline const T& _ElementAtUnchecked(uint32_t i, uint32_t j) const {
		return d[w*j+i];
	}
};

#pragma warning ( pop ) 

#endif
