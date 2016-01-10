#ifndef _IMAGE_UTILS_H_
#define _IMAGE_UTILS_H_

#include <vmath/vec3.h>
#include "image.h"

#define _GAMMA_EPSILON 0.001f
#define _LOG_EPSILON 0.001f

class ImageUtils {
public:
    static void ApplyExposure(Image<Vec3f>& img, float e) { img.Scale(powf(2,e)); }
    static void ApplyExposureGamma(Image<Vec3f>& img, float e, float g) { if(fabs(g-1) < _GAMMA_EPSILON) ApplyExposure(img, e); else ApplyScaleGamma(img,powf(2,e),g); }
    static void ApplyScaleGamma(Image<Vec3f>& img, float s, float g) { 
        for(uint32_t i = 0; i < img.Size(); i ++) {
            img.ElementAt(i) = (img.ElementAt(i) * s).Pow(g);
        }
    }
    static void ApplyLog(Image<Vec3f>& img, float b) { 
        for(uint32_t i = 0; i < img.Size(); i ++) { 
            img.ElementAt(i) = (img.ElementAt(i) + Vec3f(_LOG_EPSILON)).Log(b);
        } 
    }

    static Image<Vec3f> ExposureGamma(const Image<Vec3f>& img, float e, float g) { Image<Vec3f> ret = img; ApplyExposureGamma(ret,e,g); return ret; }
    static Image<Vec3f> ScaleGamma(const Image<Vec3f>& img, float s, float g) { Image<Vec3f> ret = img; ApplyScaleGamma(ret,s,g); return ret; }

    template<class T>
    static Image<T> VerticalStack(const Image<T>& a, const Image<T>& b) {
        Image<T> ret(max(a.Width(), b.Width()), a.Height()+b.Height());
        ret.SetSubImage(a,0,0,0,0,a.Width(),a.Height());
        ret.SetSubImage(b,0,a.Height(),0,0,b.Width(),b.Height());
        return ret;
    }

    template<class T>
    static Image<T> VerticalStack(const Image<T>& a, const Image<T>& b, const Image<T>& c, const Image<T>& d) {
        Image<T> ret(max(max(max(a.Width(), b.Width()),c.Width()),d.Width()), 
                     a.Height()+b.Height()+c.Height()+d.Height());
        ret.SetSubImage(a,0,0,0,0,a.Width(),a.Height());
        ret.SetSubImage(b,0,a.Height(),0,0,b.Width(),b.Height());
        ret.SetSubImage(c,0,a.Height()+b.Height(),0,0,c.Width(),c.Height());
        ret.SetSubImage(d,0,a.Height()+b.Height()+c.Height(),0,0,d.Width(),d.Height());
        return ret;
    }

    template<class T>
    static Image<T> VerticalStack(const Image<T>& a, const Image<T>& b, const Image<T>& c) {
        Image<T> ret(max(max(a.Width(), b.Width()),c.Width()), a.Height()+b.Height()+c.Height());
        ret.SetSubImage(a,0,0,0,0,a.Width(),a.Height());
        ret.SetSubImage(b,0,a.Height(),0,0,b.Width(),b.Height());
        ret.SetSubImage(c,0,a.Height()+b.Height(),0,0,c.Width(),c.Height());
        return ret;
    }

    template<class T>
    static void Accumulate(Image<T>& a, const Image<T>& b) {
        assert(a.Width() == b.Width() && a.Height() == b.Height());
        for(int i = 0; i < a.Size(); i ++) a.ElementAt(i) += b.ElementAt(i);
    }

    template<class T>
    static Image<T> Add(const Image<T>& a, const Image<T>& b) {
        assert(a.Width() == b.Width() && a.Height() == b.Height());
        Image<T> ret(a.Width(), a.Height());
        Add(ret, a, b);
        return ret;
    }

    template<class T>
    static void Add(Image<T>& ret, const Image<T>& a, const Image<T>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
        for(int i = 0; i < a.Size(); i ++) ret.ElementAt(i) = a.ElementAt(i) + b.ElementAt(i);
    }

    template<class T>
    static Image<T> Subtract(const Image<T>& a, const Image<T>& b) {
        assert(a.Width() == b.Width() && a.Height() == b.Height());
        Image<T> ret(a.Width(), a.Height());
        Subtract(ret, a, b);
        return ret;
    }

    template<class T>
    static void Subtract(Image<T>& ret, const Image<T>& a, const Image<T>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
        for(int i = 0; i < a.Size(); i ++) ret.ElementAt(i) = a.ElementAt(i) - b.ElementAt(i);
    }

    template<class T>
    static Image<T> Multiply(const Image<T>& a, const Image<T>& b) {
        assert(a.Width() == b.Width() && a.Height() == b.Height());
        Image<T> ret(a.Width(), a.Height());
        Multiply(ret, a, b);
        return ret;
    }

    template<class T>
    static void Multiply(Image<T>& ret, const Image<T>& a, const Image<T>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
        for(int i = 0; i < a.Size(); i ++) ret.ElementAt(i) = a.ElementAt(i) * b.ElementAt(i);
    }

    static Image<Vec3f> AbsSubtract(const Image<Vec3f>& a, const Image<Vec3f>& b) {
        assert(a.Width() == b.Width() && a.Height() == b.Height());
        Image<Vec3f> ret(a.Width(), a.Height());
        for(uint32_t i = 0; i < a.Size(); i ++) ret.ElementAt(i) = (a.ElementAt(i) - b.ElementAt(i)).Abs();
        return ret;
    }

    static Image<Vec3f> ClampSubtract(const Image<Vec3f>& a, const Image<Vec3f>& b) {
        assert(a.Width() == b.Width() && a.Height() == b.Height());
        Image<Vec3f> ret(a.Width(), a.Height());
        ClampSubtract(ret, a, b);
        return ret;
    }

    static void ClampSubtract(Image<Vec3f>& ret, const Image<Vec3f>& a, const Image<Vec3f>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
        for(uint32_t i = 0; i < a.Size(); i ++) ret.ElementAt(i) = Vec3f::Max((a.ElementAt(i) - b.ElementAt(i)), Vec3f(0));
    }

    template<class T>
    static void Max(Image<T>& ret, const Image<T>& a, const Image<T>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
		for(uint32_t i = 0; i < a.Size(); i ++) ret.ElementAt(i) = Vec3f::Max(a.ElementAt(i), b.ElementAt(i));
    }

    template<class T>
    static void Min(Image<T>& ret, const Image<T>& a, const Image<T>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
		for(uint32_t i = 0; i < a.Size(); i ++) ret.ElementAt(i) = Vec3f::Min(a.ElementAt(i), b.ElementAt(i));
    }

    static Image<Vec3f> ToVec3(const Image<float>& img) {
        Image<Vec3f> ret(img.Width(), img.Height());
        for(uint32_t i = 0; i < ret.Size(); i ++) ret.ElementAt(i) = img.ElementAt(i);
        return ret;
    }

    static Image<float> ToFloat(const Image<Vec3f>& img) {
        Image<float> ret(img.Width(), img.Height());
        for(uint32_t i = 0; i < ret.Size(); i ++) ret.ElementAt(i) = img.ElementAt(i).Average();
        return ret;
    }

    static Image<Vec3f> Pow(const Image<Vec3f>& img, float p) {
        Image<Vec3f> ret(img);
        for(uint32_t i = 0; i < ret.Size(); i ++) ret.ElementAt(i).Pow(p);
        return ret;
    }

    static Image<Vec3f> Sqrt(const Image<Vec3f>& img) {
        Image<Vec3f> ret(img);
        for(uint32_t i = 0; i < ret.Size(); i ++) ret.ElementAt(i).Sqrt();
        return ret;
    }

    template<class T>
    static Image<T> Normalize(const Image<T>& img) { Image<T> ret = img; ret.Scale(1/ret.Average()); return img; }
    template<class T>
    static Image<T> NormalizeMax(const Image<T>& img) { Image<T> ret = img; ret.Scale(1/ret.MaxElement()); return img; }

    template<class T>
    static void SetToNormalize(Image<T>& img) { img.Scale(1/img.Average()); }
    template<class T>
    static void SetToNormalizeMax(Image<T>& img) { img.Scale(1/img.MaxElement()); }

    static void Split(const Image<Vec3f>& img, Image<Vec3f>& foreground, Image<Vec3f>& background, const Image<float>& weight) {
        for(uint32_t i = 0; i < img.Size(); i ++) {
            background.ElementAt(i) = img.ElementAt(i) * (1-weight.ElementAt(i));
            foreground.ElementAt(i) = img.ElementAt(i) * weight.ElementAt(i);
        }
    }

    static void MakeBinary(Image<float>& img, float t) {
        for(uint32_t i = 0; i < img.Size(); i ++) img.ElementAt(i) = (img.ElementAt(i) > t)?1.0f:0.0f;
    }

    static void MakeBinaryFromAverage(Image<Vec3f>& img, float t) {
        for(uint32_t i = 0; i < img.Size(); i ++) img.ElementAt(i) = (img.ElementAt(i).Average() > t)?1.0f:0.0f;
    }

    static void Clamp(Image<Vec3f>& ret, const Vec3f& a, const Vec3f& b) {
        for(uint32_t i = 0; i < ret.Size(); i ++) ret.ElementAt(i) = ret.ElementAt(i).Clamp(a, b);
    }

    static void ClampMin(Image<Vec3f>& ret, const Vec3f& a) {
        for(uint32_t i = 0; i < ret.Size(); i ++) ret.ElementAt(i) = ret.ElementAt(i).ClampMin(a);
    }

    static void ThresholdMin(Image<Vec3f>& ret, const Vec3f& a) {
        for(uint32_t i = 0; i < ret.Size(); i ++) {
            if(ret.ElementAt(i).x < a.x) ret.ElementAt(i).x = 0;
            if(ret.ElementAt(i).y < a.y) ret.ElementAt(i).y = 0;
            if(ret.ElementAt(i).z < a.z) ret.ElementAt(i).z = 0;
        }
    }

    static void ThresholdMin(Image<float>& ret, float a) {
        for(uint32_t i = 0; i < ret.Size(); i ++) {
            if(ret.ElementAt(i) < a) ret.ElementAt(i) = 0;
        }
    }
};

#endif
