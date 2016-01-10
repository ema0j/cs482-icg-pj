#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "source.h"
#include "xform.h"
//#include <sampler/Distribution1D.h>

struct GLLightApproximation 
{
    Vec3f Cl;
    Vec3f posDir;
    bool isDirectional;
};

struct ShadowSample {
    Vec3f le;
    float maxDist;
    Vec3f wi;
    float pdf;

    bool Valid() { return !le.IsZero() && pdf != 0; }
};

struct PhotonSample {
    Vec3f le;
    Vec3f P;
    Vec3f wo;
    float pdf; // 1 / solid angle * area

    bool Valid() { return !le.IsZero() && pdf != 0; }
};

struct LightSample {
    shared_ptr<Light> light;
    float pdf;
};

class Light : public SceneObject {
public:
    Light() { }
    Light(shared_ptr<Source> s, shared_ptr<Xform> xf, const string& n = "") : source(s), xform(xf), name(n) { }

    virtual ShadowSample SampleShadow(const Vec3f& P, const Vec2f& s, float time);
    virtual float SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time);
                                                                                                                                            
    virtual PhotonSample SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time);
    
    virtual shared_ptr<VLightSample> SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time);
    
    virtual Vec3f SampleBackground(const Vec3f& dir, float time);

    virtual Vec3f Power(float sceneRadius) { return source->Power(sceneRadius); }

    virtual bool IsDelta() { return source->IsDelta(); }
    virtual int GetShadowSamples(){ return source->GetShadowSamples(); }

    virtual GLLightApproximation ApproximateAsGLLight(float time);

    shared_ptr<Source>& SourceRef() { return source; }
    shared_ptr<Xform>& XformRef() { return xform; }
    string& Name() { return name; }

    // this is for area lights
    virtual shared_ptr<Surface> GetSurface() { return source->GetSurface(xform); }
	
    virtual void CollectStats(StatsManager& stats);

	static string serialize_typename();
	virtual void serialize(Archive * a);

protected:
    shared_ptr<Source> source;
    shared_ptr<Xform> xform;

    string name;
};

#endif
