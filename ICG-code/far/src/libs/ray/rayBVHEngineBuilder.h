#ifndef _RAY_BVH_ENGINE_BUILDER_H_
#define _RAY_BVH_ENGINE_BUILDER_H_
#include "rayBVHEngine.h"

class RayBVHEngineBuilder
{
public:
    RayEngine*  Build(const vector<shared_ptr<Surface> >& surfaces, const vector<shared_ptr<InstanceGroup> >& instances, 
        const Intervalf& time, int timeSamples);
    RayEngine*  Build(shared_ptr<Surface> surface, const Intervalf& time, int timeSamples);
    void        Build(shared_ptr<InstanceGroup> surface, const Intervalf& time, int timeSamples, vector<RayEngine*> &es);
private:
    template<typename T>
    RayEngine*  MakeNode(shared_ptr<T> shape, shared_ptr<Material> material, shared_ptr<Xform> xform);
    template<typename T>
    void        MakeNodes(shared_ptr<T> shape, vector<shared_ptr<Material> > &materials, vector<shared_ptr<Xform> > &xforms, vector<RayEngine*> &es);
};


#endif // _RAY_BVH_ENGINE_BUILDER_H_
