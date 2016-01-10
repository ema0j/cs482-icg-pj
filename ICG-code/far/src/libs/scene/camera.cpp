#include "camera.h"

GLCameraApproximation Camera::ApproximateAsGLCamera(float time) {
    GLCameraApproximation approx;
    approx.view = ViewMatrixGL(time);
    approx.proj = ProjectionMatrixGL(time);
    approx.isOrtho = approx.proj[3][3] > 0;
    if(!approx.isOrtho) {
        approx.posDir = xform->GetTransform(time).TransformPoint<float>(Vec3f(0,0,0));
    } else {
        approx.posDir = xform->GetTransform(time).TransformVector<float>(Vec3f(0,0,-1)).GetNormalized();
    }
    return approx;
}

Ray Camera::GenerateRay(const Vec2f& uv, float time) {
    Ray ray = lens->GenerateRay(uv, time);
    if(!xform->IsIdentity()) {
        Matrix4d m = xform->GetTransform(time);
        ray = Ray(m.TransformPoint(ray.E), m.TransformVector(ray.D).GetNormalized(), ray.time);
    }
    return ray;
}

Ray Camera::GenerateRay(const Vec2f& uv, const Vec2f& auv, float time) {
    Ray ray = lens->GenerateRay(uv, auv, time);
    if(!xform->IsIdentity()) {
        Matrix4d m = xform->GetTransform(time);
        ray = Ray(m.TransformPoint(ray.E), m.TransformVector(ray.D).GetNormalized(), ray.time);
    }
    return ray;
}

shared_ptr<Camera> Camera::Clone() { 
    return shared_ptr<Camera>(new Camera(xform->Clone(), lens->Clone(), name+"_cloned")); 
}

void Camera::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Cameras");
    stat->Increment();
    xform->CollectStats(stats);
    lens->CollectStats(stats);
}

string Camera::serialize_typename() { return "Camera"; } 
void Camera::serialize(Archive* a) {
    a->optional("name", name, string(""));
	a->member("lens", lens);
	a->optional("xform", xform, XformIdentity::IdentityXform());
}
