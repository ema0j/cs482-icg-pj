#ifndef _MATERIAL_DELTA_H_
#define _MATERIAL_DELTA_H_

#include "material.h"
#include "reflectance.h"
#include "texture.h"

class PerfectReflectionMaterial : public MaterialX {
public:
	PerfectReflectionMaterial() : MaterialX(), kr(Vec3f::One()) { }
	PerfectReflectionMaterial(const Vec3f &kr_) : MaterialX(), kr(kr_) { }

	virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu)
    {
        msu.Clear();
		msu.AddReflect(kr, 1.0f);
	}

	virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(Vec3f(kr.x, kr.y, kr.z)); }

	Vec3f& Kr() { return kr; }
	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
	Vec3f                kr;
};

class SimpleRefractionMaterial : public MaterialX {
public:
	SimpleRefractionMaterial(const Vec3f &kt_ = Vec3f::One(), float idx = 1.5f) : MaterialX(), kt(kt_), index(idx) 
    {
        _isectOpt |= IOPT_SINGLE_SIDED;
    }

	virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) 
    {
        msu.Clear();
        msu.AddTransmit(kt, index);
	}

	virtual GLPhongApproximation ApprtoximateAsGLPhong() const { return GLPhongApproximation(Vec3f(kt.x, kt.y, kt.z)); }

	Vec3f& Kt() { return kt; }
	static string serialize_typename();
	virtual void serialize(Archive* a);
protected:
	Vec3f               kt;
	float				index;
};



#endif
