#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "lens.h"
#include "xform.h"

struct GLCameraApproximation {
    Matrix4d view;
    Matrix4d proj;
    bool isOrtho;
    Vec3f posDir; // either pos or dir of the camera to compute rays
};

class Camera : public SceneObject {
public:
    Camera() { }
    Camera(shared_ptr<Xform> xf, shared_ptr<Lens> l, const string& n = "") : xform(xf), lens(l), name(n) { }

    virtual bool HasDepthOfField() { return lens->HasDepthOfField(); }

    virtual shared_ptr<Xform>& XformRef() { return xform; }
    virtual shared_ptr<Lens>& LensRef() { return lens; }
    virtual string& Name() { return name; }

    virtual Ray GenerateRay(const Vec2f& uv, float time);
    virtual Ray GenerateRay(const Vec2f& uv, const Vec2f& auv, float time);

    virtual GLCameraApproximation ApproximateAsGLCamera(float time);
    virtual Matrix4d ViewMatrixGL(float time) { return xform->GetInverseTransform(time); }
    virtual Matrix4d ProjectionMatrixGL(float time) { return lens->ProjectionMatrixGL(time); }

    virtual shared_ptr<Camera> Clone();

    virtual void CollectStats(StatsManager& stats);

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    shared_ptr<Xform> xform;
    shared_ptr<Lens> lens;

    string name;
};

#endif
