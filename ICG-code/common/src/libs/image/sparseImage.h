#ifndef _SPARSEIMAGE_H_
#define _SPARSEIMAGE_H_

#include "image.h"

template<class T>
class SparseImage {
public:
    SparseImage() {
        w = 0;
        h = 0;
        d = 0;
		idx = 0;
		l = 0;
    }

    SparseImage(int nw, int nh, int nl) {
        w = nw;
        h = nh;
		l = nl;
        d = new T[l];
		idx = new int[l];
    }

    SparseImage(const SparseImage<T>& img) { 
        w = 0;
        h = 0;
        d = 0;
		idx = 0;
		l = 0;
        Set(img);
    }

    ~SparseImage() {
        Clear();
    }

    void Alloc(int nw, int nh, int nl) {
		if(w == nw && h == nh && l == nl) return;
        Clear();
        w = nw;
        h = nh;
		l = nl;
        d = new T[l];
        idx = new int[l];
    }

    void Clear() {
		if(d) { delete[] d; delete[] idx; }
        d = 0; idx = 0;
        w = h = l = 0;
    }

	T& SparseElementAt(int i) {
#ifdef IMAGE_CHECKBOUNDS
        assert(i >=0 && i < l);
#endif
		return d[i];
	}

	int SparseIndexAt(int i) {
#ifdef IMAGE_CHECKBOUNDS
        assert(i >=0 && i < l);
#endif
		return idx[i];
	}

	const T& SparseElementAt(int i) const {
#ifdef IMAGE_CHECKBOUNDS
        assert(i >=0 && i < l);
#endif
		return d[i];
	}
    
	const int SparseIndexAt(int i) const {
#ifdef IMAGE_CHECKBOUNDS
        assert(i >=0 && i < l);
#endif
		return idx[i];
	}
    
    uint32_t GetIndex(int i, int j) { return w*j+i; }

    uint32_t Width() const { return w; }
    uint32_t Height() const { return h; }
    uint32_t Size() const { return w*h; }

    uint32_t SparseSize() const { return l; }

    T* GetDataPointer() { return d; }
    int* GetIndexPointer() { return idx; }
    const T* GetDataPointer() const { return d; }
    const int* GetIndexPointer() const { return idx; }

	void Set(const T& v) { for(int i = 0; i < l; i ++) d[i] = v; }
    void Set(const SparseImage<T>& img) {
        Alloc(img.w,img.h,img.l);
        memcpy(idx,img.idx,sizeof(int)*l);
        memcpy(d,img.d,sizeof(T)*l);
    }

    SparseImage<T>& operator=(const SparseImage<T>& img) { Set(img); return *this; }

	void ToImage(Image<T>& img) const {
		img.Alloc(w,h);
		img.Set(0);
		for(uint32_t i = 0; i < l; i ++) img[idx[i]] = d[i];
	}

	void FromImage(const Image<T>& img) {
		int count = 0;
		for(uint32_t i = 0; i < img.Size(); i ++) {
			if(!(img[i] == T(0))) count ++;
		}

		Alloc(img.Width(),img.Height(),count);

		count = 0;
		for(uint32_t i = 0; i < img.Size(); i ++) {
			if(!(img[i] == T(0))) { 
				d[count] = img[i]; 
				idx[count] = i; 
				count++; 
			}
		}
	}

protected:
    T* d;
	int* idx;
    uint32_t w;
    uint32_t h;
	uint32_t l;
};

#endif
