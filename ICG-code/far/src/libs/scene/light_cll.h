#ifndef _LIGHT_CLL_H_
#define _LIGHT_CLL_H_

#include "light.h"

class CllDecoupLight : public Light {
public:
    CllDecoupLight() { }

    virtual ShadowSample SampleShadow(const Vec3f& P, const Vec2f& s, float time);                                                                                                   
    virtual Vec3f Power(float sceneRadius) { return diffuseSource->Power(sceneRadius); }

    virtual bool IsDelta() { return diffuseSource->IsDelta(); }
    virtual int GetShadowSamples(){ return diffuseSource->GetShadowSamples(); }

    // this is for area lights
    virtual shared_ptr<Surface> GetSurface() { return shared_ptr<Surface>(); }
	
	static string serialize_typename();
	virtual void serialize(Archive * a);

    shared_ptr<CllSource> DiffuseSource() { return diffuseSource; }
    shared_ptr<CllSource> ShadowSource() { return shadowSource; }
    shared_ptr<CllSource> SpecularSource() { return specularSource; }
protected:
    shared_ptr<CllSource> shadowSource;
    shared_ptr<CllSource> diffuseSource;  
    shared_ptr<CllSource> specularSource;
    
};

#endif
