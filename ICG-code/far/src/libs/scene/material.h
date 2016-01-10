#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "sceneObject.h"
#include <scene/smath.h>
#include <misc/stats.h>
#include <scene/reflectance_union.h>
#include "texture.h"

struct GLPhongApproximation {
    GLPhongApproximation(const Vec3f& Kd) : Ke(Vec3f::Zero()), Kd(Kd), Ks(Vec3f::Zero()), n(1) { }
    GLPhongApproximation(const Vec3f& Kd, const Vec3f& Ks, float n) : Ke(Vec3f::Zero()), Kd(Kd), Ks(Ks), n(n) { }
    GLPhongApproximation(const Vec3f& Ke, const Vec3f& Kd, const Vec3f& Ks, float n) : Ke(Ke), Kd(Kd), Ks(Ks), n(n) { }

    bool HasGlossy() { return (!Ks.IsZero()) && (n != 0);}
    Vec3f Ke; // emission
    Vec3f Kd;
    Vec3f Ks;
    float n;
};

enum INTERSECT_OPTION
{
    IOPT_SINGLE_SIDED       = 1,
    IOPT_IGNORE_SHADOW    = 2,
    IOPT_NO_GATHER_ISECT    = 4,
};

class Material : public SceneObject {
public:
    virtual void SampleReflectance(const DifferentialGeometry& dg, BxdfUnion &msu) = 0;
    virtual GLPhongApproximation ApprtoximateAsGLPhong() const = 0;
    virtual GLPhongApproximation ApprtoximateAsGLPhong(const DifferentialGeometry& dg) const;
    virtual void CollectStats(StatsManager& stats);
    virtual uint32_t IntersectOption() { return 0; }
    virtual bool HasAlphaMap() const = 0;
    virtual bool CheckAlpha(const Vec2f &uv, float rng) const = 0;
    static shared_ptr<Material> DefaultMaterial();
};

class MaterialX : public Material
{
public:
    MaterialX() : _isectOpt(0) { }
    virtual uint32_t        IntersectOption() { return _isectOpt; }
    virtual bool            HasAlphaMap() const;
    virtual bool            CheckAlpha(const Vec2f &uv, float rng) const;
    shared_ptr<TextureF>    _bump;
    shared_ptr<TextureF>    _alpha;
    uint32_t                _isectOpt;
protected:
    void                    _BumpMap( shared_ptr<TextureF> _bump, const DifferentialGeometry& dgs, DifferentialGeometry &dg );
};

#endif
