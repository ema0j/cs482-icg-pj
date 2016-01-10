#ifndef _MATERIAL_UBER_H_
#define _MATERIAL_UBER_H_
#include "material.h"
#include "texture.h"

enum UberDiffuseModel
{
    UBER_NONE_DIFFUSE,
    UBER_LAMBERT,
    UBER_OREN_NAYER
};

enum UberSpecularModel
{
    UBER_NONE_SPECULAR,
    UBER_PHONG,
    UBER_COOKTORR,
};

class UberMaterial : public MaterialX
{
public:
    UberMaterial();
    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu);
    virtual GLPhongApproximation ApprtoximateAsGLPhong() const;
    virtual GLPhongApproximation ApprtoximateAsGLPhong(const DifferentialGeometry& dg) const;

    static string           serialize_typename() { return "UberMaterial"; };
    virtual void            serialize(Archive* a);

    shared_ptr<TextureV>    _diffuse;
    shared_ptr<TextureV>    _specular;
    shared_ptr<TextureF>    _hard;

    shared_ptr<TextureV>    _transmission;
    shared_ptr<TextureV>    _reflection;

    shared_ptr<TextureF>    _eta;

    UberDiffuseModel        _diffuseModel;
    UberSpecularModel       _specularModel;
protected:
    void                    _SetModel( string diffMod, string specMod, bool shadowIsect, bool singleSided);
    void                    _GetModel( string &diffMod, string &specMod, bool &shadowIsect, bool &singleSided );
};

#endif // _MATERIAL_UBER_H_
