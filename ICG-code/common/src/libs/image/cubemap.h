#ifndef _CUBEMAP_H_
#define _CUBEMAP_H_

#include "image.h"
#include <vmath/matrix4.h>
#include <vmath/vec3.h>
#include <vmath/geom3.h>
#include <vmath/vec2.h>
#include <vmath/vfunctions.h>
#include <algorithm>
using namespace std;

#define CUBEMAP_PX 0
#define CUBEMAP_NX 1
#define CUBEMAP_PY 2
#define CUBEMAP_NY 3
#define CUBEMAP_PZ 4
#define CUBEMAP_NZ 5

// cube map axis are set assuming right handed coordinates
// PX:   Z, Y
// NX:  -Z, Y
// PY:   X, Z
// NY:   X,-Z
// PZ:   X,-Y
// NZ:   X, Y

// linear image
// PX,NX,PY,NY,PZ,NZ

// vertical cross
//     PY
// NX, NZ, PX
//     NY
//     PZ
// NZ=front, PZ=back, PX=right, NX=left, PY=top, NY=bottom

// horizontal cross
//     PY
// NX, NZ, PX, PZ (transpose)
//     NY
// NZ=front, PZ=back, PX=right, NX=left, PY=top, NY=bottom

// latlong (this just gives the order of things
// vertical: [-Z, +Z]
// horizontal: [X, Y, -X, -Y] with a PI/2 shift
// convention taken by HDRShop

inline bool __insideBox(int i, int j, int bx, int by, int r) {
    return i >= bx && i < bx+r && j >= by && j < by+r;
}

struct CubeMapCoord {
	int f;
	int i,j;
};

template<class T>
class CubeMap {
public:
	CubeMap() {
	}

    CubeMap(int res) {
		Alloc(res);
    }

    CubeMap(int res, const T& v) {
		Alloc(res);
        Set(v);
    }

    ~CubeMap() {
		Clear();
    }

	void Copy(const CubeMap<T>& m) {
		for(int f = 0; f < 6; f ++) { faces[f].Copy(m.faces[f]); }
	}

    void Alloc(int res) {
		for(int f = 0; f < 6; f ++) { faces[f].Alloc(res,res); }
    }

    void Clear() {
		for(int f = 0; f < 6; f ++) { faces[f].Clear(); }
    }

	Image<T>& Face(int f) {
#ifdef IMAGE_CHECKBOUNDS
        assert(f >= 0 && f < 6);
#endif
		return faces[f];
	}

	const Image<T>& Face(int f) const {
#ifdef IMAGE_CHECKBOUNDS
        assert(f >= 0 && f < 6);
#endif
		return faces[f];
	}
    
    T& ElementAt(int f, int i, int j) {
#ifdef IMAGE_CHECKBOUNDS
        assert(f >= 0 && f < 6);
#endif
		return faces[f].ElementAt(i,j);
    }

	int Resolution() const { return faces[0].Width(); }
    int Width() const { return faces[0].Width(); }
    int Height() const { return faces[0].Height(); }
    int Size() const { return 6*faces[0].Size(); }

	void Set(const T& v) { for(int f = 0; f < 6; f ++) faces[f].Set(v); }
	void Set(const CubeMap<T>& v) { for(int f = 0; f < 6; f ++) faces[f].Set(v.faces[f]); }
    template<class TT> void SetToAverage(const CubeMap<TT>& v) { for(int f = 0; f < 6; f ++) faces[f].SetToAverage(v.Face(f)); }
    void Scale(const T& v) { for(int f = 0; f < 6; f ++) faces[f].Scale(v); }
    void Scale(const CubeMap<T>& v) { for(int f = 0; f < 6; f ++) faces[f].Scale(v.Face(f)); }

	T Average() const { T m = T(); for(int f = 0; f < 6; f ++) m += faces[f].Average(); return m / 6; } // BUG: shuld init be 0?
	T MaxElement() const { T m = faces[0].MaxElement(); for(int f = 1; f < 6; f ++) m = max(m,faces[f].MaxElement()); return m; }

	// convertions are listed above
	Vec3f GetDirection(int f, int i, int j) const {
        return GetDirection(f, i, j, Resolution());
	}
    
    static Vec3f GetDirection(int f, int i, int j, int r) {
		Vec3f dir;
		switch(f) {
			case CUBEMAP_PX:
				dir = Vec3f( 1, 1-2*(0.5f+j)/r,-1+2*(0.5f+i)/r).GetNormalized();
				break;
			case CUBEMAP_NX:
				dir = Vec3f(-1, 1-2*(0.5f+j)/r, 1-2*(0.5f+i)/r).GetNormalized();
				break;
			case CUBEMAP_PY:
				dir = Vec3f(-1+2*(0.5f+i)/r, 1, 1-2*(0.5f+j)/r).GetNormalized();
				break;
			case CUBEMAP_NY:
				dir = Vec3f(-1+2*(0.5f+i)/r,-1,-1+2*(0.5f+j)/r).GetNormalized();
				break;
			case CUBEMAP_PZ:
				dir = Vec3f(-1+2*(0.5f+i)/r,-1+2*(0.5f+j)/r, 1).GetNormalized();
				break;
			case CUBEMAP_NZ:
				dir = Vec3f(-1+2*(0.5f+i)/r, 1-2*(0.5f+j)/r,-1).GetNormalized();
				break;
			default:
				assert(0);
				break;
		}
		return dir;        
    }

    static Vec3f GetDirectionContinuous(int f, float u, float v) {
		Vec3f dir;
		switch(f) {
			case CUBEMAP_PX:
				dir = Vec3f( 1, 1-2*(v),-1+2*(u)).GetNormalized();
				break;
			case CUBEMAP_NX:
				dir = Vec3f(-1, 1-2*(v), 1-2*(u)).GetNormalized();
				break;
			case CUBEMAP_PY:
				dir = Vec3f(-1+2*(u), 1, 1-2*(v)).GetNormalized();
				break;
			case CUBEMAP_NY:
				dir = Vec3f(-1+2*(u),-1,-1+2*(v)).GetNormalized();
				break;
			case CUBEMAP_PZ:
				dir = Vec3f(-1+2*(u),-1+2*(v), 1).GetNormalized();
				break;
			case CUBEMAP_NZ:
				dir = Vec3f(-1+2*(u), 1-2*(v),-1).GetNormalized();
				break;
			default:
				assert(0);
				break;
		}
		return dir;        
    }

	static float ApproximateSolidAngle(int f, int i, int j, int r) {
		//assumes cube is 2x2x2 (r=1)
		Vec3f dir = GetDirection(f,i,j,r);
		float A = (2.0f/r); A *= A;
		Vec3f N;
		switch(f){
			case CUBEMAP_PX: N = Vec3f(-1,0,0); break;
			case CUBEMAP_NX: N = Vec3f(1,0,0); break;
			case CUBEMAP_PY: N = Vec3f(0,-1,0); break;
			case CUBEMAP_NY: N = Vec3f(0,1,0); break;
			case CUBEMAP_PZ: N = Vec3f(0,0,-1); break;
			case CUBEMAP_NZ: N = Vec3f(0,0,1); break;
		}
		float cosalpha = N%(-dir);
		float dist = 1.0f/cosalpha;
		return (A*cosalpha) / (dist*dist);
	}

    static CubeMap<float> ApproximateSolidAngle(int r) {
        CubeMap<float> ret(r);
        for(int f = 0; f < 6; f ++) {
            for(int j = 0; j < r; j ++) {
                for(int i = 0; i < r; i ++) {
                    ret.Face(f).ElementAt(i,j) = ApproximateSolidAngle(f,i,j,r);
                }
            }
        }
        return ret;
    }
    
	T Lookup(const Vec3f& d) const {
		Vec3f da = d; // TODO: OSX hacks to get it to compile -> investigate
		da[0] = fabs(d[0]); da[1] = fabs(d[1]); da[2] = fabs(d[2]);
		int f,i,j;
		if(da[0] >= da[1] && da[0] >= da[2]) {
			if(d[0] > 0) {
				f = CUBEMAP_PX;
				i = (int) (Resolution() * ( d[2]/da[0] * 0.5f + 0.5f));
				j = (int) (Resolution() * (-d[1]/da[0] * 0.5f + 0.5f));
			} else {
				f = CUBEMAP_NX;
				i = (int) (Resolution() * (-d[2]/da[0] * 0.5f + 0.5f));
				j = (int) (Resolution() * (-d[1]/da[0] * 0.5f + 0.5f));
			}
		} else if(da[1] >= da[0] && da[1] >= da[2]) {
			if(d[1] > 0) {
				f = CUBEMAP_PY;
				i = (int) (Resolution() * ( d[0]/da[1] * 0.5f + 0.5f));
				j = (int) (Resolution() * (-d[2]/da[1] * 0.5f + 0.5f));
			} else {
				f = CUBEMAP_NY;
				i = (int) (Resolution() * ( d[0]/da[1] * 0.5f + 0.5f));
				j = (int) (Resolution() * ( d[2]/da[1] * 0.5f + 0.5f));
			}
		} else {
			if(d[2] > 0) {
				f = CUBEMAP_PZ;
				i = (int) (Resolution() * ( d[0]/da[2] * 0.5f + 0.5f));
				j = (int) (Resolution() * ( d[1]/da[2] * 0.5f + 0.5f));
			} else {
				f = CUBEMAP_NZ;
				i = (int) (Resolution() * ( d[0]/da[2] * 0.5f + 0.5f));
				j = (int) (Resolution() * (-d[1]/da[2] * 0.5f + 0.5f));
			}
		}
		// make sure there are no our of index things
		if(i == Resolution()) i = Resolution()-1;
		if(j == Resolution()) j = Resolution()-1;
		return faces[f].Lookup(i,j);
	}

    void LookupIndex(const Vec3f& d, int* f, int* i, int *j) const { LookupIndex(d,Resolution(),f,i,j); }
	static void LookupIndex(const Vec3f& d, int r, int* f, int* i, int *j) {
		Vec3f da = d; // TODO: OSX hacks to get it to compile -> investigate
		da[0] = fabs(d[0]); da[1] = fabs(d[1]); da[2] = fabs(d[2]);
		if(da[0] >= da[1] && da[0] >= da[2]) {
			if(d[0] > 0) {
				*f = CUBEMAP_PX;
				*i = (int) (r * ( d[2]/da[0] * 0.5f + 0.5f));
				*j = (int) (r * (-d[1]/da[0] * 0.5f + 0.5f));
			} else {
				*f = CUBEMAP_NX;
				*i = (int) (r * (-d[2]/da[0] * 0.5f + 0.5f));
				*j = (int) (r * (-d[1]/da[0] * 0.5f + 0.5f));
			}
		} else if(da[1] >= da[0] && da[1] >= da[2]) {
			if(d[1] > 0) {
				*f = CUBEMAP_PY;
				*i = (int) (r * ( d[0]/da[1] * 0.5f + 0.5f));
				*j = (int) (r * (-d[2]/da[1] * 0.5f + 0.5f));
			} else {
				*f = CUBEMAP_NY;
				*i = (int) (r * ( d[0]/da[1] * 0.5f + 0.5f));
				*j = (int) (r * ( d[2]/da[1] * 0.5f + 0.5f));
			}
		} else {
			if(d[2] > 0) {
				*f = CUBEMAP_PZ;
				*i = (int) (r * ( d[0]/da[2] * 0.5f + 0.5f));
				*j = (int) (r * ( d[1]/da[2] * 0.5f + 0.5f));
			} else {
				*f = CUBEMAP_NZ;
				*i = (int) (r * ( d[0]/da[2] * 0.5f + 0.5f));
				*j = (int) (r * (-d[1]/da[2] * 0.5f + 0.5f));
			}
		}
		// make sure there are no our of index things
		if(*i == r) *i = r-1;
		if(*j == r) *j = r-1;
	}

	T Sample(const Direction3f& d, bool linear, float lookupPixelOffset = 0) const {
        return Sample(Vec3f(d.x,d.y,d.z), linear, lookupPixelOffset);
    }

	T Sample(const Vec3f& d, bool linear, float lookupPixelOffset = 0) const {
		Vec3f da = d; // TODO: OSX hacks to get it to compile -> investigate
		da[0] = fabs(d[0]); da[1] = fabs(d[1]); da[2] = fabs(d[2]);
		int f; float u, v;
		if(da[0] >= da[1] && da[0] >= da[2]) {
			if(d[0] > 0) {
				f = CUBEMAP_PX;
				u =  d[2]/da[0] * 0.5f + 0.5f;
				v = -d[1]/da[0] * 0.5f + 0.5f;
			} else {
				f = CUBEMAP_NX;
				u = -d[2]/da[0] * 0.5f + 0.5f;
				v = -d[1]/da[0] * 0.5f + 0.5f;
			}
		} else if(da[1] >= da[0] && da[1] >= da[2]) {
			if(d[1] > 0) {
				f = CUBEMAP_PY;
				u =  d[0]/da[1] * 0.5f + 0.5f;
				v = -d[2]/da[1] * 0.5f + 0.5f;
			} else {
				f = CUBEMAP_NY;
				u =  d[0]/da[1] * 0.5f + 0.5f;
				v =  d[2]/da[1] * 0.5f + 0.5f;
			}
		} else {
			if(d[2] > 0) {
				f = CUBEMAP_PZ;
				u =  d[0]/da[2] * 0.5f + 0.5f;
				v =  d[1]/da[2] * 0.5f + 0.5f;
			} else {
				f = CUBEMAP_NZ;
				u =  d[0]/da[2] * 0.5f + 0.5f;
				v = -d[1]/da[2] * 0.5f + 0.5f;
			}
		}
		// make sure there are no our of index things
		if(u >= 1) u = 0.9999f;
		if(v >= 1) v = 0.9999f;
        return faces[f].SampleClamped(u,v,linear,lookupPixelOffset);
	}

	void ToLinearImage(const Image<T>& img) const {
		assert(img.Width() == Resolution()*6 && img.Height() == Resolution());

		img.SetSubImage(faces[CUBEMAP_PX], 0*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_NX], 1*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_PY], 2*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_NY], 3*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_PZ], 4*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_NZ], 5*Resolution(), 0, 0, 0, Resolution(), Resolution());
	}

	Image<T> ToLinearImage() const {
        Image<T> img(Resolution()*6, Resolution());

		img.SetSubImage(faces[CUBEMAP_PX], 0*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_NX], 1*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_PY], 2*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_NY], 3*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_PZ], 4*Resolution(), 0, 0, 0, Resolution(), Resolution());
		img.SetSubImage(faces[CUBEMAP_NZ], 5*Resolution(), 0, 0, 0, Resolution(), Resolution());

        return img;
	}

	Image<T> ToVerticalCross(const T& background = 0) const {
        Image<T> ret(Resolution()*3, Resolution()*4);
        ToVerticalCross(ret,background);
        return ret;
    }

	void ToVerticalCross(Image<T>& img, const T& background = 0) const {
		assert(img.Width() == Resolution()*3 && img.Height() == Resolution()*4);

		int r = Resolution();

		img.Set(background);
		img.SetSubImage(faces[CUBEMAP_PX],2*r,1*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_NX],0*r,1*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_PY],1*r,0*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_NY],1*r,2*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_PZ],1*r,3*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_NZ],1*r,1*r,0,0,r,r);
	}

	Image<T> ToHalfHorizontalCross(const T& background = 0) const {
        Image<T> ret = ToHorizontalCross(background);
        return ret.GetCropped(0,0,ret.Width(),ret.Height()/2);
    }

	Image<T> ToHorizontalCross(const T& background = 0) const {
        Image<T> ret(Resolution()*4, Resolution()*3);
        ToHorizontalCross(ret,background);
        return ret;
    }

	void ToHorizontalCross(Image<T>& img, const T& background = 0) const {
		assert(img.Width() == Resolution()*4 && img.Height() == Resolution()*3);

		int r = Resolution();

		img.Set(background);
		img.SetSubImage(faces[CUBEMAP_PX],2*r,1*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_NX],0*r,1*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_PY],1*r,0*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_NY],1*r,2*r,0,0,r,r);
        Image<T> pz = faces[CUBEMAP_PZ]; pz.Rotate180(); img.SetSubImage(pz,3*r,1*r,0,0,r,r);
		img.SetSubImage(faces[CUBEMAP_NZ],1*r,1*r,0,0,r,r);
	}

	void ToLatLong(Image<T>& img, int samples = 3) const {
		assert(img.Width() == Resolution()*4 && img.Height() == img.Width()/2);

		for(uint32_t i=0; i<img.Width(); ++i){
			for(uint32_t j=0; j<img.Height(); ++j){
				img.ElementAt(i,j) = 0;
				for(int ii=0; ii<samples; ++ii){
					for(int jj=0; jj<samples; ++jj){
						Vec2f uv((i+(ii+0.5f)/samples)/img.Width(),
							(j+(jj+0.5f)/samples)/img.Height());
						Vec3f dir = LatLongDirFromUV(uv);
						T c = Sample(dir,true);
						img.ElementAt(i,j) += c;
					}
				}
				img.ElementAt(i,j) *= 1.0f/(samples*samples);
			}
		}
	}

    Image<T> ToHalfPolar(int res, int samples) const {
        Image<T> img(res, res);
		for(uint32_t i=0; i<img.Width(); ++i){
			for(uint32_t j=0; j<img.Height(); ++j){
				img.ElementAt(i,j) = 0;
                int count = 0;
				for(int ii=0; ii<samples; ++ii){
					for(int jj=0; jj<samples; ++jj){
						Vec2f uv((i+(ii+0.5f)/samples)/img.Width(),
							(j+(jj+0.5f)/samples)/img.Height());
                        bool inside;
						Vec3f dir = HalfPolarDirFromUV(uv,inside);
                        if(inside) {
						    T c = Sample(dir,true);
						    img.ElementAt(i,j) += c;
                            count ++;
                        }
					}
				}
				if(count) img.ElementAt(i,j) *= 1.0f/count;
			}
		}
        return img;
    }

    Image<T> ToVerticalPolar(int res, int samples) const {
        Image<T> img(res, 2*res);
		for(int i=0; i<img.Width(); ++i){
			for(int j=0; j<img.Height(); ++j){
				img.ElementAt(i,j) = 0;
                int count = 0;
				for(int ii=0; ii<samples; ++ii){
					for(int jj=0; jj<samples; ++jj){
						Vec2f uv((i+(ii+0.5f)/samples)/img.Width(),
							(j+(jj+0.5f)/samples)/img.Height());
                        bool inside;
						Vec3f dir = VerticalPolarDirFromUV(uv,inside);
                        if(inside) {
						    T c = Sample(dir,true);
						    img.ElementAt(i,j) += c;
                            count ++;
                        }
					}
				}
				if(count) img.ElementAt(i,j) *= 1.0f/count;
			}
		}
        return img;
    }

    Image<T> ToHorizontalPolar(int res, int samples) const {
        Image<T> img(2*res, res);
		for(int i=0; i<img.Width(); ++i){
			for(int j=0; j<img.Height(); ++j){
				img.ElementAt(i,j) = 0;
                int count = 0;
				for(int ii=0; ii<samples; ++ii){
					for(int jj=0; jj<samples; ++jj){
						Vec2f uv((i+(ii+0.5f)/samples)/img.Width(),
							(j+(jj+0.5f)/samples)/img.Height());
                        bool inside;
						Vec3f dir = HorizontalPolarDirFromUV(uv,inside);
                        if(inside) {
						    T c = Sample(dir,true);
						    img.ElementAt(i,j) += c;
                            count ++;
                        }
					}
				}
				if(count) img.ElementAt(i,j) *= 1.0f/count;
			}
		}
        return img;
    }

	void FromLinearImage(const Image<T>& img) {
        assert(img.Width() / 6 == img.Height());

        Alloc(img.Height());

		int r = Resolution();
		faces[CUBEMAP_PX].SetSubImage(img,0,0,0*r,0,r,r);
		faces[CUBEMAP_NX].SetSubImage(img,0,0,1*r,0,r,r);
		faces[CUBEMAP_PY].SetSubImage(img,0,0,2*r,0,r,r);
		faces[CUBEMAP_NY].SetSubImage(img,0,0,3*r,0,r,r);
		faces[CUBEMAP_PZ].SetSubImage(img,0,0,4*r,0,r,r);
		faces[CUBEMAP_NZ].SetSubImage(img,0,0,5*r,0,r,r);
	}

	void FromVerticalCross(const Image<T>& img) {
		assert(img.Width() / 3 == img.Height() / 4);

		Alloc(img.Width() / 3);

		int r = Resolution();
		faces[CUBEMAP_PX].SetSubImage(img,0,0,2*r,1*r,r,r);
		faces[CUBEMAP_NX].SetSubImage(img,0,0,0*r,1*r,r,r);
		faces[CUBEMAP_PY].SetSubImage(img,0,0,1*r,0*r,r,r);
		faces[CUBEMAP_NY].SetSubImage(img,0,0,1*r,2*r,r,r);
		faces[CUBEMAP_PZ].SetSubImage(img,0,0,1*r,3*r,r,r);
		faces[CUBEMAP_NZ].SetSubImage(img,0,0,1*r,1*r,r,r);
	}

	void FromHorizontalCross(const Image<T>& img) {
		assert(img.Width()/ 4 == img.Height() / 3);

		Alloc(img.Width() / 4);

		int r = Resolution();
		faces[CUBEMAP_PX].SetSubImage(img,0,0,2*r,1*r,r,r);
		faces[CUBEMAP_NX].SetSubImage(img,0,0,0*r,1*r,r,r);
		faces[CUBEMAP_PY].SetSubImage(img,0,0,1*r,0*r,r,r);
		faces[CUBEMAP_NY].SetSubImage(img,0,0,1*r,2*r,r,r);
		faces[CUBEMAP_PZ].SetSubImage(img,0,0,3*r,1*r,r,r); faces[CUBEMAP_PZ].Rotate180();
		faces[CUBEMAP_NZ].SetSubImage(img,0,0,1*r,1*r,r,r);
	}

	void FromHalfHorizontalCross(const Image<T>& img) {
        Image<Vec3f> aux(img.Width(), img.Height()*2);
        aux.Set(0);
        aux.SetSubImage(img,0,0,0,0,img.Width(),img.Height());
        return FromHorizontalCross(aux);
    }

    bool MapToHorizontalCross(int f, int i, int j, int& ci, int& cj) const { return MapToHorizontalCross(f, i, j, Resolution(), ci, cj); }
    static bool MapToHorizontalCross(int f, int i, int j, int r, int& ci, int& cj) {
        if(!__insideBox(i,j,0,0,r)) return false;
        switch(f) {
            case CUBEMAP_PX: ci = 2*r+i; cj = 1*r+j; break;
            case CUBEMAP_NX: ci = 0*r+i; cj = 1*r+j; break;
            case CUBEMAP_PY: ci = 1*r+i; cj = 0*r+j; break;
            case CUBEMAP_NY: ci = 1*r+i; cj = 2*r+j; break;
            case CUBEMAP_PZ: ci = 3*r+r-i; cj = 1*r+r-j; break;
            case CUBEMAP_NZ: ci = 1*r+i; cj = 1*r+j; break;
        }
        return true;
    }

    bool MapFromHorizontalCross(int ci, int cj, int& f, int& i, int& j) const { return MapFromHorizontalCross(ci,cj,Resolution(),f,i,j); }
    static bool MapFromHorizontalCross(int ci, int cj, int r, int& f, int& i, int& j) {
        if(__insideBox(ci,cj,0*r,1*r,r)) { f = CUBEMAP_NX; i = ci-0*r; j = cj-1*r; return true; }
        if(__insideBox(ci,cj,1*r,1*r,r)) { f = CUBEMAP_NZ; i = ci-1*r; j = cj-1*r; return true; }
        if(__insideBox(ci,cj,2*r,1*r,r)) { f = CUBEMAP_PX; i = ci-2*r; j = cj-1*r; return true; }
        if(__insideBox(ci,cj,3*r,1*r,r)) { f = CUBEMAP_PZ; i = ci-3*r; j = cj-1*r; i = r-i-1; j = r-j-1; return true; } // BUG
        if(__insideBox(ci,cj,1*r,0*r,r)) { f = CUBEMAP_PY; i = ci-1*r; j = cj-0*r; return true; }
        if(__insideBox(ci,cj,1*r,2*r,r)) { f = CUBEMAP_NY; i = ci-1*r; j = cj-2*r; return true; }
        return false;
    }

    bool MapToVerticalCross(int f, int i, int j, int& ci, int& cj) const { return MapToVerticalCross(f,i,j,Resolution(),ci,cj); }
    static bool MapToVerticalCross(int f, int i, int j, int r, int& ci, int& cj) {
        if(!__insideBox(i,j,0,0,r)) return false;
        switch(f) {
            case CUBEMAP_PX: ci = 2*r+i; cj = 1*r+j; break;
            case CUBEMAP_NX: ci = 0*r+i; cj = 1*r+j; break;
            case CUBEMAP_PY: ci = 1*r+i; cj = 0*r+j; break;
            case CUBEMAP_NY: ci = 1*r+i; cj = 2*r+j; break;
            case CUBEMAP_PZ: ci = 1*r+i; cj = 3*r+j; break;
            case CUBEMAP_NZ: ci = 1*r+i; cj = 1*r+j; break;
        }
        return true;
    }

    bool MapFromVerticalCross(int ci, int cj, int& f, int& i, int& j) const { return MapFromVerticalCross(ci,cj,Resolution(),f,i,j); }
    static bool MapFromVerticalCross(int ci, int cj, int r, int& f, int& i, int& j) {
        if(__insideBox(ci,cj,0*r,1*r,r)) { f = CUBEMAP_NX; i = ci-0*r; j = cj-1*r; return true; }
        if(__insideBox(ci,cj,1*r,1*r,r)) { f = CUBEMAP_NZ; i = ci-1*r; j = cj-1*r; return true; }
        if(__insideBox(ci,cj,2*r,1*r,r)) { f = CUBEMAP_PX; i = ci-2*r; j = cj-1*r; return true; }
        if(__insideBox(ci,cj,1*r,3*r,r)) { f = CUBEMAP_PZ; i = ci-1*r; j = cj-3*r; return true; }
        if(__insideBox(ci,cj,1*r,0*r,r)) { f = CUBEMAP_PY; i = ci-1*r; j = cj-0*r; return true; }
        if(__insideBox(ci,cj,1*r,2*r,r)) { f = CUBEMAP_NY; i = ci-1*r; j = cj-2*r; return true; }
        return false;
    }

	//from spherical parameterized image
	// noet that the Y coordinates are flipped
	void FromLatLong(const Image<T>& img, int samples = 3){
		Alloc(img.Width() / 4);
		
		int r = Resolution();
		//for each face
		for(int f = 0; f < 6; f++) {
			//sample the latitude/longitude image
			for(int i = 0; i < r; i++) {
				for(int j = 0; j < r; j++) {
					for(int ii=0; ii<samples; ++ii){
						for(int jj=0; jj<samples; ++jj){
							Vec3f dir = GetDirectionContinuous(f,(float)(i+(ii+0.5f)/samples)/r,(float)(j+(jj+0.5f)/samples)/r);
							Vec2f uv = LatLongUVFromDir(dir);
							T c = img.Sample(uv[0], uv[1], true, true, 0.5f);
							faces[f].ElementAt(i,j) += c;
						}
					}
					faces[f].ElementAt(i,j) *= 1.0f/(samples*samples);
				}
			}
		}
	}

	//from spherical parameterized image
	// noet that the Y coordinates are flipped
	void FromHalfPolar(const Image<T>& img, int samples = 3){
		Alloc(img.Width() / 2);
		
		int r = Resolution();
		//for each face
		for(int f = 0; f < 6; f++) {
			//sample the latitude/longitude image
			for(int i = 0; i < r; i++) {
				for(int j = 0; j < r; j++) {
                    int count = 0;
					for(int ii=0; ii<samples; ++ii){
						for(int jj=0; jj<samples; ++jj){
							Vec3f dir = GetDirectionContinuous(f,(float)(i+(ii+0.5f)/samples)/r,(float)(j+(jj+0.5f)/samples)/r);
                            bool inside;
                            Vec2f uv = HalfPolarUVFromDir(dir,inside);
                            if(inside) {
							    T c = img.Sample(uv[0], uv[1], true, true, 0.5f);
							    faces[f].ElementAt(i,j) += c;
                                count ++;
                            }
						}
					}
					faces[f].ElementAt(i,j) *= 1.0f/count;
				}
			}
		}
	}

	void RotateY(int pixels) {
		//incorrect hack
		Image<T> img(Resolution()*4,Resolution()*3);
		ToHorizontalCross(img);
		Image<T> rot(Resolution()*4,Resolution()*3);
		rot.Set(img);
		for(int j = Resolution(); j < 2*Resolution(); j ++) {
			for(int i = 0; i < Resolution()*4; i ++) {
				rot.ElementAt(i,j) = img.ElementAt((i+pixels) % (4*Resolution()),j);
			}
		}
		FromHorizontalCross(rot);
	}

	void RotateX(int pixels) {
		//incorrect hack
		Image<T> img(Resolution()*3,Resolution()*4);
		ToVerticalCross(img);
		Image<T> rot(Resolution()*3,Resolution()*4);
		rot.Set(img);
		for(int i = Resolution(); i < 2*Resolution(); i ++) {
			for(int j = 0; j < 4*Resolution(); j ++) {
				rot.ElementAt(i,j) = img.ElementAt(i,(4*Resolution()+j+pixels) % (4*Resolution()));
			}
		}
		FromVerticalCross(rot);
	}

    void Rotate(const Matrix4d& r, int samplesXandY) {
        CubeMap<T> rot(Resolution());
        for(int f = 0; f < 6; f ++) {
            for(int j = 0; j < Resolution(); j ++) {
                for(int i = 0; i < Resolution(); i ++) {
                    rot.Face(f).ElementAt(i,j) = 0;
                    for(int jj = 0; jj < samplesXandY; jj ++) {
                        for(int ii = 0; ii < samplesXandY; ii ++) {
                            float u = (((ii+0.5f)/samplesXandY+i) / Resolution());
                            float v = (((jj+0.5f)/samplesXandY+j) / Resolution());
                            Vec3f d = GetDirectionContinuous(f,u,v);
                            Vec3f dr = r.TransformVector(d);
                            rot.Face(f).ElementAt(i,j) += Sample(dr,true,-0.5f);
                        }
                    }
                    rot.Face(f).ElementAt(i,j) /= T(samplesXandY*samplesXandY);
                }
            }
        }
        Set(rot);
    }


	// conversion from latlong image coordinates to direction
	static Vec3f LatLongDirFromUV(const Vec2f& uv) {
        Vec2f angle(uv.x * TWO_PIf, uv.y * PIf);
        Vec3f dir = angleToDir(angle);
        return dir;
	}

	static Vec2f LatLongUVFromDir(const Vec3f& dir) {
        Vec2f uv = dirToAngle(dir);
        uv.x *= INV_TWO_PIf;
        uv.y *= INV_PIf;
        return uv;
	}

    Vec3f VerticalCrossDirFromIJ(int ci, int cj, bool& inside) const { return VerticalCrossDirFromIJ(ci,cj,Resolution(),inside); }
	static Vec3f VerticalCrossDirFromIJ(int ci, int cj, int r, bool& inside) {
        int f, i, j;
        inside = MapFromVerticalCross(ci, cj, r, f, i, j);
        if(inside) return GetDirection(f,i,j,r);
        else return Vec3f(0,0,0);
	}

    Vec2i VerticalCrossIJFromDir(const Vec3f& dir) const { return VerticalCrossIJFromDir(dir, Resolution()); }
	static Vec2i VerticalCrossIJFromDir(const Vec3f& dir, int r) {
        int f, i, j, ci, cj;
        LookupIndex(dir, r, &f, &i, &j);
        MapToVerticalCross(f,i,j,r,ci,cj);
        return Vec2i(ci,cj);
	}

    Vec3f HorizontalCrossDirFromIJ(int ci, int cj, bool& inside) const { return HorizontalCrossDirFromIJ(ci,cj,Resolution(),inside); }
	static Vec3f HorizontalCrossDirFromIJ(int ci, int cj, int r, bool& inside) {
        int f, i, j;
        inside = MapFromHorizontalCross(ci, cj, r, f, i, j);
        if(inside) return GetDirection(f,i,j,r);
        else return Vec3f(0,0,0);
	}

    Vec2i HorizontalCrossIJFromDir(const Vec3f& dir) const { return HorizontalCrossIJFromDir(dir, Resolution()); }
	static Vec2i HorizontalCrossIJFromDir(const Vec3f& dir, int r) {
        int f, i, j, ci, cj;
        LookupIndex(dir, r, &f, &i, &j);
        MapToHorizontalCross(f,i,j,r,ci,cj);
        return Vec2i(ci,cj);
	}

	// conversion from half polar image coordinates to direction
    static Vec3f HalfPolarDirFromIJ(int ci, int cj, int res, bool& inside) { return HalfPolarDirFromUV(Vec2f((ci+0.5f)/(res),(cj+0.5f)/(res)),inside); }
	static Vec3f HalfPolarDirFromUV(const Vec2f& uv, bool& inside) {
        float x = uv.x*2-1;
        float z = uv.y*2-1;
        inside = x*x+z*z <= 1;
        if(inside) return Vec3f(x,sqrt(1-(x*x+z*z)),z);
        else return Vec3f(0);
	}

    static Vec2i HalfPolarIJFromDir(const Vec3f& dir, int res, bool& inside) { 
        Vec2f uv = HalfPolarUVFromDir(dir, inside); 
        if(inside) return Vec2i((int)(uv.x*res), (int)(uv.y*res));
        else return Vec2i(-1,-1);
    }
	static Vec2f HalfPolarUVFromDir(const Vec3f& dir, bool& inside) {
        inside = dir.y >= 0;
        if(inside) return Vec2f(dir.x,dir.z)*0.5f+0.5f;
        else return Vec2f(-1,-1);
	}

    static Vec3f VerticalPolarDirFromIJ(int ci, int cj, int res, bool& inside) { return VerticalPolarDirFromUV(Vec2f((ci+0.5f)/(res),(cj+0.5f)/(2*res)),inside); }
	static Vec3f VerticalPolarDirFromUV(const Vec2f& uv, bool& inside) {
        bool top = uv.y <= 0.5f;
        Vec2f cuv = (top)?Vec2f(uv.x,uv.y*2):Vec2f(uv.x,(uv.y-0.5f)*2);
        float x = cuv.x*2-1;
        float z = cuv.y*2-1;
        inside = x*x+z*z <= 1;
        if(inside) return Vec3f(x,(top)?sqrt(1-(x*x+z*z)):-sqrt(1-(x*x+z*z)),z);
        else return Vec3f(0);
	}
    static Vec2i VerticalPolarIJFromDir(const Vec3f& dir, int res) { 
        Vec2f uv = VerticalPolarUVFromDir(dir); 
        return Vec2i((int)(uv.x*res), (int)(uv.y*res*2));
    }
	static Vec2f VerticalPolarUVFromDir(const Vec3f& dir) {
        bool top = dir.y >= 0;
        Vec2f uv = Vec2f(dir.x,dir.z)*0.5f+0.5f;
        if(top) uv.y = uv.y / 2; else uv.y = uv.y / 2 + 0.5f;
        return uv;
	}

    static Vec3f HorizontalPolarDirFromIJ(int ci, int cj, int res, bool& inside) { return HorizontalPolarDirFromUV(Vec2f((ci+0.5f)/(2*res),(cj+0.5f)/(res)),inside); }
	static Vec3f HorizontalPolarDirFromUV(const Vec2f& uv, bool& inside) {
        bool top = uv.x <= 0.5f;
        Vec2f cuv = (top)?Vec2f(uv.x*2,uv.y):Vec2f((uv.x-0.5f)*2,uv.y);
        float x = cuv.x*2-1;
        float z = cuv.y*2-1;
        inside = x*x+z*z <= 1;
        if(inside) return Vec3f(x,(top)?sqrt(1-(x*x+z*z)):-sqrt(1-(x*x+z*z)),z);
        else return Vec3f(0);
	}
    static Vec2i HorizontalPolarIJFromDir(const Vec3f& dir, int res) { 
        Vec2f uv = HorizontalPolarUVFromDir(dir); 
        return Vec2i((int)(uv.x*res*2), (int)(uv.y*res));
    }
	static Vec2f HorizontalPolarUVFromDir(const Vec3f& dir) {
        bool top = dir.y >= 0;
        Vec2f uv = Vec2f(dir.x,dir.z)*0.5f+0.5f;
        if(top) uv.x = uv.x / 2; else uv.x = uv.x / 2 + 0.5f;
        return uv;
	}

    void DownSampleToSize(int resolution) {
        DownSample(Resolution() / resolution);
    }

    void DownSample(int factor) {
        for(int f = 0; f < 6; f ++) faces[f].DownSample(factor);
    }

    void UpSample(int factor) {
        for(int f = 0; f < 6; f ++) faces[f].UpSample(factor);
    }

protected:
	Image<T> faces[6];

	// this is for sampling latlong maps
	// look it up from image sample
	/*
	inline Vec3f _Sample2d(const Vec2f &st, const Image<T> &image) {
        int s0 = (int)floorf(st[0]*image.Width()-0.5);
        int s1 = s0+1;
        int t0 = (int)floorf(st[1]*image.Height()-0.5);
        int t1 = t0+1;
        float ds = (st[0]*image.Width()-0.5)-s0, dt = (st[1]*image.Height()-0.5)-t0;	
		s0 = (s0 + image.Width()) % image.Width();
		s1 = (s1 + image.Width()) % image.Width();
		t0 = (t0 + image.Height()) % image.Height();
		t1 = (t1 + image.Height()) % image.Height();
        Vec4f val = image.ElementAt(s0,t0)* (1.0f-ds)*(1.0f-dt) +
			image.ElementAt(s0,t1) * (1.0f-ds)*(dt) +
			image.ElementAt(s1,t0) * (ds)*(1.0f-dt) +
			image.ElementAt(s1,t1) * (ds)*(dt);
		ret = Vec3f(val[0],val[1],val[2]);
    }
	*/
};

#endif
