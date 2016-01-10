#ifndef _TREE_BUILDER_VIRTUAL_LIGHT_CACHE_H_
#define _TREE_BUILDER_VIRTUAL_LIGHT_CACHE_H_

template<typename T>
class BuilderLightCache : public VirtualLightCache
{
public:
    BuilderLightCache(T *lightTree)
        : VirtualLightCache(), _nDirect(0), _nIndirect(0), _lightTree(lightTree) {}
    ~BuilderLightCache(void) {}

    virtual void			Clear() { _nDirect = _nIndirect = 0; _lightTree->Clear(); }
    virtual void			AddOmniDirLight(const Vec3f &position, const Vec3f &intensity) 
    {     
        _lightTree->OmniDirLights().push_back(OmniDirLight(position, intensity));
        _nDirect++;
    }
    virtual void			AddDirLight(const Vec3f &normal, const Vec3f &le)
    {
        _lightTree->DirectionalLights().push_back(DirLight(normal, le));
        _nDirect++;
    }
    virtual void			AddOrientedLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
    {
        _lightTree->OrientedLights().push_back(OrientedLight(position, normal, le));
        _nDirect++;
    }
    virtual void			AddIndirectLight(const Vec3f& position, const Vec3f &normal, const Vec3f &le)
    {
        _lightTree->OrientedLights().push_back(OrientedLight(position, normal, le));
        _nIndirect++;
    }
    virtual uint32_t      DirectLightNum() const { return _nDirect; }
    virtual uint32_t      IndirectLightNum() const { return _nIndirect; }
private:
    uint32_t				    _nDirect;
    uint32_t				    _nIndirect;
    T							*_lightTree;
};

#endif // _TREE_BUILDER_VIRTUAL_LIGHT_CACHE_H_
