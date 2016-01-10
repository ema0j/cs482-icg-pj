#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include "sceneObject.h"
#include "light.h"

class Background : public SceneObject {
public:
	virtual Vec3f SampleBackground(const Vec3f& dir, float time) = 0;

    virtual void CollectStats(StatsManager& stats);
};

#endif
