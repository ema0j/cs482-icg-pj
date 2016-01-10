#ifndef _RAY_LIST_ENGINE_BUILDER_H_
#define _RAY_LIST_ENGINE_BUILDER_H_
#include "rayListEngine.h"

class RayListEngineBuilder
{
public:
    RayEngine* Build(const vector<shared_ptr<Surface> >& surfaces, const vector<shared_ptr<InstanceGroup> >& instances, 
        const Intervalf& time, int timeSamples);
    RayEngine* Build(shared_ptr<Surface> surface, const Intervalf& time, int timeSamples);
    RayEngine* Build(shared_ptr<InstanceGroup> surface, const Intervalf& time, int timeSamples, vector<RayEngine*> &es);
private:
    template<typename T>
    RayEngine* MakeNode(shared_ptr<T> shape, shared_ptr<Material> material, shared_ptr<Xform> xform);
};


#endif // _RAY_LIST_ENGINE_BUILDER_H_
