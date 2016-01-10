#include "material_uber.h"

UberMaterial::UberMaterial() 
    : MaterialX(), _diffuseModel(UBER_LAMBERT), _specularModel(UBER_NONE_SPECULAR) {}

GLPhongApproximation UberMaterial::ApprtoximateAsGLPhong() const
{
    Vec3f rd = _diffuse.get() != NULL ? _diffuse->Average() : Vec3f::Zero();
    Vec3f rs = _specular.get() != NULL ? _specular->Average() : Vec3f::Zero();
    float n = _hard.get() != NULL ? _hard->Average() : 0.0f;
    return GLPhongApproximation(rd / PIf, rs * (n + 2) / (2 * PIf), n);
}

GLPhongApproximation UberMaterial::ApprtoximateAsGLPhong( const DifferentialGeometry& dg ) const
{
    Vec3f rd = _diffuse.get() != NULL ? _diffuse->Sample(dg.st) : Vec3f::Zero();
    Vec3f rs = _specular.get() != NULL ?_specular->Sample(dg.st) : Vec3f::Zero();
    float n = _hard.get() != NULL ? _hard->Sample(dg.st) : 0.0f;
    return GLPhongApproximation(rd / PIf, rs * (n + 2) / (2 * PIf), n);
}

void UberMaterial::serialize( Archive* a )
{
    string diffMod;
    string specMod;
    bool shadowIsect;
    bool singleSided;

    if(!a->isreading())
        _GetModel(diffMod, specMod, shadowIsect, singleSided);

	a->optional("shadowIsect", shadowIsect, true);
	a->optional("singleSided", singleSided, false);

    a->optional("diffShader", diffMod, string("Lambert"));
    a->optional("diff", _diffuse, shared_ptr<TextureV>());
 
    a->optional("specShader", specMod, string("None"));
    a->optional("spec", _specular, shared_ptr<TextureV>());

    a->optional("hard", _hard, shared_ptr<TextureF>());

    a->optional("transmit", _transmission, shared_ptr<TextureV>());
    a->optional("reflect", _reflection, shared_ptr<TextureV>());

    a->optional("eta", _eta, shared_ptr<TextureF>());
    a->optional("bump", _bump, shared_ptr<TextureF>());

    a->optional("alpha", _alpha, shared_ptr<TextureF>());

    if(a->isreading())
        _SetModel(diffMod, specMod, shadowIsect, singleSided);
}

void UberMaterial::SampleReflectance( const DifferentialGeometry& dgs, BxdfUnion &msu)
{
    DifferentialGeometry dg;
    _BumpMap(_bump, dgs, dg);

    msu.Clear();
    if (_diffuseModel == UBER_LAMBERT)
        msu.AddLambert(_diffuse->Sample(dg.st));

    if (_specularModel == UBER_PHONG)
        msu.AddPhong(_specular->Sample(dg.st), _hard->Sample(dg.st));

    if (_transmission && _eta)
        msu.AddTransmit(_transmission->Sample(dg.st), _eta->Sample(dg.st));

    if (_reflection)
        msu.AddReflect(_reflection->Sample(dg.st), _eta->Sample(dg.st));
}

void UberMaterial::_SetModel( string diffMod, string specMod, bool shadowIsect, bool singleSided)
{
    if (!shadowIsect)
        _isectOpt |= IOPT_IGNORE_SHADOW;

    if (singleSided)
        _isectOpt |= IOPT_SINGLE_SIDED;

    if (diffMod == "Lambert")
    {
        _diffuseModel = UBER_LAMBERT;
        if (_diffuse.get() == NULL)
            _diffuse = shared_ptr<ConstTextureV>(new ConstTextureV(Vec3f::Zero()));
    }
	else
		_diffuseModel = UBER_NONE_DIFFUSE;

    if (specMod.empty())
    {
        _specularModel = UBER_NONE_SPECULAR;
    }
    if (specMod == "Phong")
    {
        _specularModel = UBER_PHONG;
        if (_specular.get() == NULL)
            _specular = shared_ptr<ConstTextureV>(new ConstTextureV(Vec3f::Zero()));
        if (_hard.get() == NULL)
            _hard = shared_ptr<ConstTextureF>(new ConstTextureF(0.0f));
    }
    else if (specMod == "CookTorr")
    {
        _specularModel = UBER_COOKTORR;
        if (_specular.get() == NULL)
            _specular = shared_ptr<ConstTextureV>(new ConstTextureV(Vec3f::Zero()));
        if (_hard.get() == NULL)
            _hard = shared_ptr<ConstTextureF>(new ConstTextureF(0.0f));
    }

    if (_transmission.get() != NULL || _reflection.get() != NULL)
    {
        if (_eta.get() == NULL)
            _eta = shared_ptr<ConstTextureF>(new ConstTextureF(1.0f));
    }
}

void UberMaterial::_GetModel( string &diffMod, string &specMod, bool &shadowIsect, bool &singleSided)
{
    shadowIsect = !(_isectOpt & IOPT_IGNORE_SHADOW);
    singleSided = (_isectOpt & IOPT_SINGLE_SIDED);

    if (_diffuseModel == UBER_LAMBERT)
        diffMod = "Lambert";
	else
		diffMod = "None";

    if (_specularModel == UBER_NONE_SPECULAR)
        specMod = "None";
    else if (_specularModel == UBER_PHONG)
        specMod = "Phong";
    else if(_specularModel == UBER_COOKTORR)
        specMod = "CookTorr";

}

