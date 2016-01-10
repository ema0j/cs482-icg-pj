#ifndef _SMOOTHMATERIAL_H_
#define _SMOOTHMATERIAL_H_

#include "material.h"
#include "reflectance.h"
#include "texture.h"

class LambertEmissionMaterial : public MaterialX {
public:
    LambertEmissionMaterial() : MaterialX(), Le(Vec3f::One()) { }
    LambertEmissionMaterial(const Vec3f& le) : MaterialX(), Le(le) { }

    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
        msu.Clear();
        msu.AddLambert(Vec3f::Zero());
        msu.emission = Le;
    }
    virtual bool IsDoubleSided() { return false; }
    virtual bool IsOpaque() { return true; }
    virtual Vec3f SampleOpacity(const DifferentialGeometry& dg) { return 1; }

    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(Le, Vec3f::Zero(), Vec3f::Zero(), 1); }
	static string serialize_typename();
	virtual void serialize(Archive * a);

protected:
    Vec3f                         Le;
};

class LambertMaterial : public MaterialX {
public:
    LambertMaterial() : MaterialX(), rd(Vec3f::One()) { }
    LambertMaterial(const Vec3f& rd) : MaterialX(), rd(rd) { }

    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
        msu.Clear();
        msu.AddLambert(rd);
    }

    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(rd/PIf); }
    Vec3f& Rd() { return rd; }

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    Vec3f                rd;
};

class TexturedLambertMaterial : public MaterialX {
public:
    TexturedLambertMaterial() : MaterialX(), rd(Vec3f::One()), rdTxt() { }
    TexturedLambertMaterial(const Vec3f& rd, shared_ptr<ImageTextureV> rdTxt) : MaterialX(), rd(rd), rdTxt(rdTxt) { }

    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
        msu.Clear();
		if(rdTxt) 
            msu.AddLambert(rd*rdTxt->Sample(dg.st));
		else 
            msu.AddLambert(rd);
    }

    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(rd*((rdTxt)?rdTxt->Average():Vec3f::One())/PIf); }
    virtual GLPhongApproximation ApprtoximateAsGLPhong(const DifferentialGeometry& dg) const
    {
        if(rdTxt) return GLPhongApproximation(rd*rdTxt->Sample(dg.st)/PIf);
        else return GLPhongApproximation(rd/PIf);
    }

    inline Vec3f& Rd() { return rd; }
	inline shared_ptr<TextureV> GetRdTxt() const { return rdTxt; }
	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    Vec3f                   rd;
	shared_ptr<TextureV>    rdTxt;
};

class PhongMaterial : public MaterialX {
public:
    PhongMaterial() : MaterialX(), rd(Vec3f(0.5f,0.5f,0.5f)), rs(Vec3f(0.5f,0.5f,0.5f)), n(100) { }
    PhongMaterial(const Vec3f& rd, const Vec3f& rs, float n) : MaterialX(), rd(rd), rs(rs), n(n) { }

    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
        msu.Clear();
        msu.AddLambert(rd);
        msu.AddPhong(rs, n);
    }

    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(rd/PIf, rs*(n+2)/(2*PIf), n); }
	
    Vec3f& Rd() { return rd; }
    Vec3f& Rs() { return rs; }
    float& N() { return n; }
	static string serialize_typename();
	virtual void serialize(Archive* a);
protected:
    Vec3f                 rd;
    Vec3f                 rs;
    float                 n;
};

//class BlinnPhongMaterial : public Material {
//public:
//    BlinnPhongMaterial() : rd(Vec3f(0.5f,0.5f,0.5f)), rs(Vec3f(0.5f,0.5f,0.5f)), n(100) { }
//    BlinnPhongMaterial(const Vec3f& rd, const Vec3f& rs, float n) : rd(rd), rs(rs), n(n) { }
//
//    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
//        msu = BxdfUnion::NormalizedBlinnPhong(rd,rs,n);
//    }
//
//    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(rd/PIf, rs*(n+2)/(2*PIf), n); }
//	
//    Vec3f& Rd() { return rd; }
//    Vec3f& Rs() { return rs; }
//    float& N() { return n; }
//
//	static string serialize_typename();
//	virtual void serialize(Archive* a);
//	
//protected:
//    Vec3f                 rd;
//    Vec3f                 rs;
//    float                 n;
//};

class TexturedPhongMaterial : public MaterialX {
public:
    TexturedPhongMaterial() : MaterialX(), n(100) { }
    TexturedPhongMaterial(const Vec3f& rd, shared_ptr<ImageTextureV> rdTxt, 
        const Vec3f& rs, shared_ptr<ImageTextureV> rsTxt, 
        float n) : MaterialX(), rd(rd), rdTxt(rdTxt), rs(rs), rsTxt(rsTxt), n(n) { }

    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
        msu.Clear();
        msu.AddLambert(rd*((rdTxt)?rdTxt->Sample(dg.st):Vec3f::One()));
        msu.AddPhong(rs*((rsTxt)?rsTxt->Sample(dg.st):Vec3f::One()), n);
    }

    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { 
        return GLPhongApproximation(rd*((rdTxt)?rdTxt->Average():Vec3f::One())/PIf, 
            rs*((rsTxt)?rsTxt->Average():Vec3f::One())*(n+2)/(2*PIf), n); 
    }
    virtual GLPhongApproximation ApprtoximateAsGLPhong(const DifferentialGeometry& dg) const
    {
        return GLPhongApproximation(rd*((rdTxt)?rdTxt->Sample(dg.st):Vec3f::One())/PIf, 
            rs*((rsTxt)?rsTxt->Sample(dg.st):Vec3f::One())*(n+2)/(2*PIf), n); 
    }
    static string serialize_typename();
    virtual void serialize(Archive* a);

    //these methods should not exist. should do this retrieval more intelligently
    shared_ptr<TextureV> GetRdTxt(){ return rdTxt; }
    shared_ptr<TextureV> GetRsTxt(){ return rsTxt; }
    Vec3f Rd() { return rd; }
    Vec3f Rs() { return rs; }
    float& N() { return n; }

protected:
    Vec3f                               rd;
    shared_ptr<TextureV>                rdTxt;
    Vec3f                               rs;
    shared_ptr<TextureV>                rsTxt;
    float                               n;
};

//class WardIsotropicMaterial : public Material {
//public:
//    WardIsotropicMaterial() : rd(Vec3f(0.5f,0.5f,0.5f)), rs(Vec3f(0.5f,0.5f,0.5f)), a(0.01f) { }
//    WardIsotropicMaterial(const Vec3f& rd, const Vec3f& rs, float a) : rd(rd), rs(rs), a(a) { }
//
//    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
//        msu = BxdfUnion::WardIsotropic(rd,rs,a);
//    }
//
//	//bad approximation
//    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(rd/PIf, rs*((1/a)+2)/(2*PIf), 1.0f/a); }
//	
//    Vec3f& Rd() { return rd; }
//    Vec3f& Rs() { return rs; }
//    float& A() { return a; }
//
//	static string serialize_typename();
//	virtual void serialize(Archive* a);
//	
//protected:
//    Vec3f                 rd;
//    Vec3f                 rs;
//    float                 a;
//};

class CookTorranceMaterial : public MaterialX {
public:
    CookTorranceMaterial() : MaterialX(), rd(Vec3f(0.5f,0.5f,0.5f)), rs(Vec3f(0.5f,0.5f,0.5f)), f0(0.5f), sigma(0.05f) { }
    CookTorranceMaterial(const Vec3f& rd, const Vec3f& rs, float f0, float sigma) : MaterialX(), rd(rd), rs(rs), f0(f0), sigma(sigma) { }

    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
        msu.Clear();
        msu.AddLambert(rd);
        msu.AddCookTorr(rs, sigma, f0);
    }

	//bad approximation
    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(rd/PIf, rs*((1/sigma)+2)/(2*PIf), 1.0f/sigma); }
	
    Vec3f& Rd() { return rd; }
    Vec3f& Rs() { return rs; }
    float& F0() { return f0; }
	float& Sigma() { return sigma; }
	static string serialize_typename();
	virtual void serialize(Archive* a);
	
protected:
    Vec3f                 rd;
    Vec3f                 rs;
    float                 f0, sigma;
};

//class KajiyaHairMaterial : public Material {
//public:
//    KajiyaHairMaterial() { Kd = Vec3f(0.5f,0.5f,0.5f); Ks = Vec3f(0.5f,0.5f,0.5f); n = 100; }
//    KajiyaHairMaterial(const Vec3f& kd, const Vec3f& ks, float nn) : Kd(kd), Ks(ks), n(nn) { }
//
//    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
//        msu = BxdfUnion::KajiyaHair(Kd,Ks,n);
//    }
//
//    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(Kd, Ks, n); }
//
//	static string serialize_typename();
//	virtual void serialize(Archive* a);
//	
//protected:
//    Vec3f                   Kd;
//    Vec3f                   Ks;
//    float                   n;
//};

//class MappedKajiyaHairMaterial : public Material {
//public:
//    MappedKajiyaHairMaterial() : n(100) { }	
//    MappedKajiyaHairMaterial(shared_ptr<ImageTextureV> kdTxt, shared_ptr<ImageTextureV> ksTxt, float nn) : KdTxt(kdTxt), KsTxt(ksTxt), n(nn) { }
//
//    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) {
//        msu = BxdfUnion::KajiyaHair(KdTxt->Sample(dg.st),KsTxt->Sample(dg.st),n);
//    }
//
//    virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(KdTxt->Average(), KsTxt->Average(), n); }
//    virtual GLPhongApproximation ApprtoximateAsGLPhong(const DifferentialGeometry& dg) const { return GLPhongApproximation(KdTxt->Sample(dg.st), KsTxt->Sample(dg.st), n); }
//
//	static string serialize_typename();
//	virtual void serialize(Archive* a);
//	
//protected:
//    shared_ptr<ImageTextureV>            KdTxt;
//    shared_ptr<ImageTextureV>            KsTxt;
//    float                                n;
//};


#endif
