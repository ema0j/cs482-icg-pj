#ifndef _SCENE_H_
#define _SCENE_H_

#include "sceneObject.h"
#include "smath.h"

class Scene : public SceneObject {
public:
    vector<shared_ptr<Surface> >& Surfaces() { return surfaces; }
    vector<shared_ptr<InstanceGroup> >& Instances() { return instances; }
    vector<shared_ptr<Light> >& Lights() { return lights; }
    shared_ptr<Camera>& MainCamera() { return mainCamera; }
	shared_ptr<Background>& MainBackground() { return background; }

    void Init();

    bool IsStatic();
    Intervalf ComputeAnimationInterval();
    Range3f ComputeBoundingBox(const Intervalf& time, int approximationSamples = 16);

    void FlattenXform();

    virtual void CollectStats(StatsManager& stats);

    static string serialize_typename() { return "Scene"; } 
	
	virtual void serialize(Archive* a);

protected:
    vector<shared_ptr<Surface> >                surfaces;
    vector<shared_ptr<InstanceGroup> >          instances;
    vector<shared_ptr<Light> >                  lights;

    shared_ptr<Camera>                          mainCamera;
	shared_ptr<Background>                      background;
};

#endif

