#ifndef _SMOOTHSOURCE_H_
#define _SMOOTHSOURCE_H_

#include "source.h"
#include "shape_mesh.h"
#include "texture.h"
#include "sampling.h"

class AreaSource : public Source {
public:
    AreaSource() : le(Vec3f::One()), shadowSamples(16) { }
    AreaSource(const Vec3f& l, int ss) : le(l), shadowSamples(ss) { }
    
    Vec3f& Le() { return le; }

    virtual bool IsDelta() { return false; }
    virtual int GetShadowSamples() { return shadowSamples; }
    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const 
        Vec2f& ss, const Vec2f& sa, float time) = 0;
    virtual void serialize(Archive * a);
    
protected:
    Vec3f le;
    int shadowSamples;
};

class QuadAreaSource : public AreaSource {
public:
    QuadAreaSource() : size(1) { }
    
    QuadAreaSource(float s, const Vec3f& L, int ss) : AreaSource(L,ss), size(s) { }

    float& Size() { return size; }
    
    float Area() { return size*size; }

    virtual Vec3f SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf);
    virtual float SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time);

    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf);

    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const 
        Vec2f& ss, const Vec2f& sa, float time);

    virtual Vec3f SampleBackground(const Vec3f& dir, float time) { return Vec3f::Zero(); }

    shared_ptr<Surface> GetSurface(shared_ptr<Xform> xform) { if(insertSurface) return _CreateIntersectionMesh(xform); else return shared_ptr<Surface>(); }

    virtual Vec3f Power(float sceneRadius) { return le * Area() * PIf; }	
    
    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = le * Area(); directional = false; }

    static string serialize_typename(); 
    virtual void serialize(Archive * a);

protected:
    float           size;
    bool            insertSurface;
    
    bool _Intersect(const Vec3f& P, const Vec3f& wi, Vec3f& Pl);

    // uv in [-1,1]^2
    Vec3f ComputeSurfacePos(const Vec2f &uv) { return Vec3f(size*uv.x/2,size*uv.y/2,0); }
    shared_ptr<Surface> _CreateIntersectionMesh(shared_ptr<Xform> xform);
};

class ShapeAreaSource : public AreaSource
{
public:
    ShapeAreaSource();
    ShapeAreaSource(shared_ptr<Shape> shape, const Vec3f& L, int ss)  : AreaSource(L, ss), _shape(shape) { _area = _shape->ComputeArea(0.0f); }

    float Area() { return _area; }

    virtual Vec3f SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf);
    virtual float SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time);

    static string serialize_typename(); 
    virtual void serialize(Archive * a);

protected:
    bool _Intersect(const Vec3f& P, const Vec3f& wi, Vec3f& Pl);

    shared_ptr<Shape>           _shape;
    float                       _area;
};

// from pbrt
class _Distribution {
public:
    _Distribution() { }
    _Distribution(const carray<float>& f) { Init(f); }

    float GetFuncIntegral() { return funcIntegral; }
    uint64_t GetFuncSize() { return func.size(); }

    void Init(const carray<float>& f) {
        func = f;
        cdf.resize(f.size()+1);
        cdf = Sampling::ComputeStepCDF(func, &funcIntegral);
        invFuncIntegral = 1.0f / funcIntegral;
    }

    float Sample(float u, float* pdf) {
        // Find surrounding cdf segments
        float *ptr = std::lower_bound(cdf.data(), cdf.data()+cdf.size(), u);
        int offset = (int) (ptr-cdf.data()-1);
        // Return offset along current cdf segment
        u = (u - cdf[offset]) / (cdf[offset+1] - cdf[offset]);
        
        *pdf = func[offset] * invFuncIntegral;
        return offset + u;
    }

    uint64_t Map(float x) { return clamp<uint32_t>(static_cast<uint32_t>(x*func.size()), 0, func.size()-1); }

    float Pdf(float x) {
        uint32_t offset = Map(x);
        return func[offset] * invFuncIntegral;
    }

protected:
    carray<float> func;
    carray<float> cdf;
    float funcIntegral;
    float invFuncIntegral;
};

class EnvSource : public AreaSource {
public:
    EnvSource() : useImportanceSampling(false) {}
    EnvSource(const Vec3f& L, int ss, bool is) : AreaSource(L,ss), useImportanceSampling(is) { }

    virtual Vec3f SampleBackground(const Vec3f &d, float time) { return le * _Sample(d.GetNormalized()); }

    virtual Vec3f SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf);
    virtual float SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time);

    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius,  const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf);

    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const 
        Vec2f& ss, const Vec2f& sa, float time);

    virtual Vec3f Power(float sceneRadius) { return le * _MapAverage() * PIf * sceneRadius * sceneRadius; }

    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = 0; directional = false; }

    virtual shared_ptr<Surface> GetSurface(shared_ptr<Xform> xform) { return shared_ptr<Surface>();	}

    virtual void serialize(Archive* a);

protected:
    bool                    useImportanceSampling;
    
    _Distribution           uDist;
    carray<_Distribution>   vDist;

    virtual Vec3f _Sample(const Vec3f& dir) = 0;
    virtual Vec3f _MapAverage() = 0;

    virtual void _InitImportanceSampling() = 0;
    virtual void _BuildImportancePDFs(Image<float> &luminance);
};

class EnvCubeSource : public EnvSource {
public:
    EnvCubeSource() { useImportanceSampling = false; }

    EnvCubeSource(const Vec3f& L, shared_ptr<CubeTexture> m, int ss, bool is) : EnvSource(L, ss, is) {
        map = m;
        if(useImportanceSampling) _InitImportanceSampling();
    }

    static string serialize_typename();
    virtual void serialize(Archive* a);

protected:
    shared_ptr<CubeTexture>	map;

    virtual Vec3f _Sample(const Vec3f& dir) { if(map) return map->Sample(dir); else return Vec3f(1,1,1); }
    virtual Vec3f _MapAverage() { if(map) return map->Average(); else return Vec3f(1,1,1); }
    virtual void _InitImportanceSampling();
};

class EnvLatLongSource : public EnvSource {
public:
    EnvLatLongSource() { useImportanceSampling = false; }

    EnvLatLongSource(const Vec3f& L, shared_ptr<ImageTextureV> m, int ss, bool is) : EnvSource(L, ss, is) {
        map = m;
        if(useImportanceSampling) _InitImportanceSampling();
    }

    static string serialize_typename();
    virtual void serialize(Archive* a) ;

protected:
    shared_ptr<ImageTextureV>	map;

    virtual Vec3f _Sample(const Vec3f& dir) { 
        if(map) {
            Vec2f uv = CubeMap<Vec4f>::LatLongUVFromDir(dir);
            uv.y = 1-uv.y;
            return map->Sample(uv);
        } 
        else return Vec3f(1,1,1); 
    }

    virtual Vec3f _MapAverage() { if(map) return map->Average(); else return Vec3f(1,1,1); }
    virtual void _InitImportanceSampling();
};

#endif
