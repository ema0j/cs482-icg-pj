#ifndef _DELTASOURCE_H_
#define _DELTASOURCE_H_

#include "source.h"

class DeltaSource : public Source {
public:
    virtual Vec3f SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist) = 0;
    virtual float SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time) { return 0; }
	virtual Vec3f SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf) {
		*pdf = 1; 
        return SampleShadow(P,time,wi,shadowRayMaxDist);
	}
	virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const 
        Vec2f& ss, const Vec2f& sa, float time) = 0;
    virtual Vec3f SampleBackground(const Vec3f& dir, float time) { return Vec3f::Zero(); }

	virtual bool IsDelta() { return true; }
    virtual int GetShadowSamples() { return 1; }
	
    virtual shared_ptr<Surface> GetSurface(shared_ptr<Xform> xform) { return shared_ptr<Surface>(); }	
};

class PointSource : public DeltaSource {
public:
    PointSource() : intensity(Vec3f::One()) { }
    PointSource(const Vec3f& i) : intensity(i) { }

    virtual Vec3f SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist);
    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, 
        float time, Vec3f* E, Vec3f* D, float* pdf);
    virtual Vec3f Power(float sceneRadius) { return intensity * PIf * 4; }
	virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
        const Vec2f& ss, const Vec2f& sa, float time);
    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = intensity; directional = false; }

    Vec3f& Intensity() { return intensity; }

	static string serialize_typename(); 
	virtual void serialize(Archive * a);

protected:
	Vec3f intensity;
};

class DirectionalSource : public DeltaSource {
public:
    DirectionalSource() : le(Vec3f::One()) { }
    DirectionalSource(const Vec3f& l) : le(l) { }

    virtual Vec3f SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist);
    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, 
        float time, Vec3f* E, Vec3f* D, float* pdf);
    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
        const Vec2f& ss, const Vec2f& sa, float time);
    virtual Vec3f Power(float sceneRadius) { return le * PIf * sceneRadius * sceneRadius; }

    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = le; directional = true; }

    Vec3f& Le() { return le; }

	static string serialize_typename();
	virtual void serialize(Archive * a);

protected:
	Vec3f le;
};

class CosineSource : public DeltaSource {
public:
    CosineSource() : intensity(Vec3f::One()) { }
    CosineSource(const Vec3f& i) : intensity(i) { }

    virtual Vec3f SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist);
    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf);
    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
        const Vec2f& ss, const Vec2f& sa, float time);
    virtual Vec3f Power(float sceneRadius) { assert(0); return intensity * PIf; }
    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = intensity; directional = false; }

    Vec3f& Intensity() { return intensity; }

	static string serialize_typename(); 
	virtual void serialize(Archive * a);

protected:
	Vec3f intensity;
};

class CosineClampedSource : public DeltaSource {
public:
    CosineClampedSource() : intensity(Vec3f::One()), minDist(0.1f) { }
    CosineClampedSource(const Vec3f& i, float md) : intensity(i), minDist(md) { }
	
    virtual Vec3f SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist);
    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf);
    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, 
        const Vec2f& ss, const Vec2f& sa, float time);
    virtual Vec3f Power(float sceneRadius) { assert(0); return intensity * PIf; }

    Vec3f& Intensity() { return intensity; }
    float& MinDist() { return minDist; }

    float ClampDistSqr(float distSqr) {
        return max(distSqr, minDist * minDist);
    }

    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = intensity; directional = false; }

    static string serialize_typename();
	virtual void serialize(Archive * a);
		
protected:
    Vec3f intensity;
    float minDist;
};

// aligned around negative z
class SpotSource : public DeltaSource {
public:
    SpotSource() : intensity(Vec3f::One()), innerAngleDegree(45), outerAngleDegree(45) { _Init(); }
    SpotSource(const Vec3f& i, float innerDegree, float outerDegree)  :
        intensity(Vec3f::One()), innerAngleDegree(innerDegree), outerAngleDegree(outerDegree) { _Init(); }
	
    virtual Vec3f SampleShadow(const Vec3f& P, float time, Vec3f* wi, float* shadowRayMaxDist);
    virtual Vec3f SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf);
    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time);
    virtual Vec3f Power(float sceneRadius) { return intensity * PIf * 2 * (1-0.5f*(cosInnerAngle+cosOuterAngle)); }

	// DANGEROUS!!!! only for building simple ui with direct memory refs
    Vec3f& Intensity() { return intensity; }
    float& InnerAngleDegree() { return innerAngleDegree; }
    float& OuterAngleDegree() { return outerAngleDegree; }
	void InitFromRefs() { _Init(); }

    virtual void ApproximateAsGLLight(float time, Vec3f& Cl, bool& directional) { Cl = intensity; directional = false; }

    static string serialize_typename(); 
	virtual void serialize(Archive * a);
		
protected:
    Vec3f intensity;
	float innerAngleDegree;
	float outerAngleDegree;

    float cosInnerAngle;
    float cosOuterAngle;

    void _Init();
    float Falloff(const Vec3f& wi);
};

#endif
