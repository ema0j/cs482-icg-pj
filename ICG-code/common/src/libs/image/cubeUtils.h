#ifndef _CUBE_UTILS_H_
#define _CUBE_UTILS_H_

#include "imageUtils.h"
#include "cubeMap.h"

class CubeUtils {
public:
    template<class T>
    static void Accumulate(CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution());
        for(int i = 0; i < 6; i ++) ImageUtils::Accumulate(a.Face(i), b.Face(i));
    }

    template<class T>
    static CubeMap<T> Add(const CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution());
        CubeMap<T> ret(a.Resolution());
        Add(ret, a, b);
        return ret;
    }

    template<class T>
    static void Add(CubeMap<T>& ret, const CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution() && ret.Resolution() == a.Resolution());
        for(int i = 0; i < 6; i ++) ret.Face(i) = ImageUtils::Add(a.Face(i), b.Face(i));
    }

    template<class T>
    static CubeMap<T> Subtract(const CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution());
        CubeMap<T> ret(a.Resolution());
        for(int i = 0; i < 6; i ++) ret.Face(i) = ImageUtils::Subtract(a.Face(i), b.Face(i));
        return ret;
    }

    template<class T>
    static void Subtract(CubeMap<T>& ret, const CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution() && ret.Resolution() == a.Resolution());
        for(int i = 0; i < 6; i ++) ret.Face(i) = ImageUtils::Subtract(a.Face(i), b.Face(i));
    }

    template<class T>
    static CubeMap<T> Multiply(const CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution());
        CubeMap<T> ret(a.Resolution());
        Multiply(ret, a, b);
        return ret;
    }

    template<class T>
    static void Multiply(CubeMap<T>& ret, const CubeMap<T>& a, const CubeMap<T>& b) {
        assert(a.Resolution() == b.Resolution() && ret.Resolution() == a.Resolution());
        for(int i = 0; i < 6; i ++) ImageUtils::Multiply(ret.Face(i), a.Face(i), b.Face(i));
    }

    static CubeMap<Vec3f> AbsSubtract(const CubeMap<Vec3f>& a, const CubeMap<Vec3f>& b) {
        assert(a.Resolution() == b.Resolution());
        CubeMap<Vec3f> ret(a.Resolution());
        for(int i = 0; i < 6; i ++) ret.Face(i) = ImageUtils::AbsSubtract(a.Face(i), b.Face(i));
        return ret;
    }

    static CubeMap<Vec3f> ClampSubtract(const CubeMap<Vec3f>& a, const CubeMap<Vec3f>& b) {
        assert(a.Resolution() == b.Resolution());
        CubeMap<Vec3f> ret(a.Resolution());
        ClampSubtract(ret, a, b);
        return ret;
    }

    static void ClampSubtract(CubeMap<Vec3f>& ret, const CubeMap<Vec3f>& a, const CubeMap<Vec3f>& b) {
        assert(ret.Width() == a.Width() && a.Width() == b.Width() && ret.Height() == a.Height() && a.Height() == b.Height());
        for(int i = 0; i < 6; i ++) ImageUtils::ClampSubtract(ret.Face(i), a.Face(i), b.Face(i));
    }

    static CubeMap<Vec3f> ToVec3(const CubeMap<float>& img) {
        CubeMap<Vec3f> ret(img.Resolution());
        for(int i = 0; i < 6; i ++) ret.Face(i) = ImageUtils::ToVec3(img.Face(i));
        return ret;
    }

    static CubeMap<float> ToFloat(const CubeMap<Vec3f>& img) {
        CubeMap<float> ret(img.Resolution());
        for(int i = 0; i < 6; i ++) ret.Face(i) = ImageUtils::ToFloat(img.Face(i));
        return ret;
    }

    static CubeMap<Vec3f> Pow(const CubeMap<Vec3f>& img, float p) {
        CubeMap<Vec3f> ret(img);
        for(int i = 0; i < 6; i ++) ImageUtils::Pow(ret.Face(i),p);
        return ret;
    }

    static CubeMap<Vec3f> Sqrt(const CubeMap<Vec3f>& img) {
        CubeMap<Vec3f> ret(img);
        for(int i = 0; i < 6; i ++) ImageUtils::Sqrt(ret.Face(i));
        return ret;
    }

    template<class T>
    static CubeMap<T> Normalize(const CubeMap<T>& img) { CubeMap<T> ret = img; ret.Scale(1/ret.Average()); return img; }
    template<class T>
    static CubeMap<T> NormalizeMax(const CubeMap<T>& img) { CubeMap<T> ret = img; ret.Scale(1/ret.MaxElement()); return img; }

    template<class T>
    static void SetToNormalize(CubeMap<T>& img) { img.Scale(1/img.Average()); }
    template<class T>
    static void SetToNormalizeMax(CubeMap<T>& img) { img.Scale(1/img.MaxElement()); }

    static void Split(const CubeMap<Vec3f>& img, CubeMap<Vec3f>& foreground, CubeMap<Vec3f>& background, const CubeMap<float>& weight) {
        for(int i = 0; i < 6; i ++) ImageUtils::Split(img.Face(i),foreground.Face(i),background.Face(i),weight.Face(i));
    }

    static void MakeBinary(CubeMap<float>& img, float t) {
        for(int i = 0; i < 6; i ++) ImageUtils::MakeBinary(img.Face(i), t);
    }

    // default to average
    static void MakeBinary(CubeMap<Vec3f>& img, float t) {
        MakeBinaryFromAverage(img, t);
    }

    static void MakeBinaryFromAverage(CubeMap<Vec3f>& img, float t) {
        for(int i = 0; i < 6; i ++) ImageUtils::MakeBinaryFromAverage(img.Face(i), t);
    }

    template<class T>
    static void Clamp(CubeMap<T>& ret, const T& a, const T& b) {
        for(int i = 0; i < 6; i ++) ImageUtils::Clamp(ret.Face(i), a, b);
    }

    template<class T>
    static void ClampMin(CubeMap<T>& ret, const T& a) {
        for(int i = 0; i < 6; i ++) ImageUtils::ClampMin(ret.Face(i), a);
    }

    template<class T>
    static void ThresholdMin(CubeMap<T>& ret, const T& a) {
        for(int i = 0; i < 6; i ++) ImageUtils::ThresholdMin(ret.Face(i), a);
    }
};

#endif
