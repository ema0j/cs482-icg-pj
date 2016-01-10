#include "material.h"
#include "material_smooth.h"

void Material::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Materials");
    stat->Increment();
}

shared_ptr<Material> Material::DefaultMaterial() { 
	static shared_ptr<Material> defaultMaterial = shared_ptr<Material>();
    if(!defaultMaterial) { 
        defaultMaterial = shared_ptr<Material>(new LambertMaterial(1)); 
    } 
    return defaultMaterial; 
}

GLPhongApproximation Material::ApprtoximateAsGLPhong(const DifferentialGeometry& dg) const
{
    return ApprtoximateAsGLPhong();
}

bool MaterialX::HasAlphaMap() const
{
    return (_alpha.get() != NULL);
}

bool MaterialX::CheckAlpha( const Vec2f &uv, float rng ) const
{
    if (_alpha.get() == NULL)
        return true;

    float alpha = _alpha->Sample(uv);
    return (rng <= alpha);
}

void MaterialX::_BumpMap( shared_ptr<TextureF> _bump, const DifferentialGeometry& dgs, DifferentialGeometry &dg )
{
    if (_bump.get() == NULL)
        dg = dgs;
    else
        dg = dgs;
}
