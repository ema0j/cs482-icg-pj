#ifndef _LENS_H_
#define _LENS_H_

#include <misc/stats.h>
#include <scene/smath.h>
#include <scene/sceneObject.h>

class Lens : public SceneObject {
public:
    virtual bool HasDepthOfField() = 0;

    virtual Ray GenerateRay(const Vec2f& uv, float time) = 0;
    virtual Ray GenerateRay(const Vec2f& uv, const Vec2f& auv, float time) = 0;

	virtual Matrix4d ProjectionMatrixGL(float time) = 0;

    virtual shared_ptr<Lens> Clone() = 0;

    virtual void CollectStats(StatsManager& stats);
};

#endif
