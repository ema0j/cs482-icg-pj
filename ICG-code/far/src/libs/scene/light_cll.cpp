#include "light_cll.h"

#include "source_delta.h"
#include "source_area.h"


ShadowSample CllDecoupLight::SampleShadow(const Vec3f& P, const Vec2f& s, float time)
{
    //assert(false);
    return ShadowSample();
}


string CllDecoupLight::serialize_typename() { return "CllDecoupLight"; } 
void CllDecoupLight::serialize(Archive * a) 
{
    a->optional("name", name, string(""));
    a->optional("source", source, shared_ptr<Source>());
    a->optional("xform", xform, XformIdentity::IdentityXform());
    a->member("specular_source", specularSource);
    a->member("shadow_source", shadowSource);
    a->member("diffuse_source", diffuseSource);
}

