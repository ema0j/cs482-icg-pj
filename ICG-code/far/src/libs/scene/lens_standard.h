#ifndef _LENS_STANDARD_H_
#define _LENS_STANDARD_H_

#include "lens.h"

class PinholeLens : public Lens {
public:
	PinholeLens() { fovDegree = Vec2f(45, 45); _Init(); }
	PinholeLens(const Vec2f& fovDegree);

    virtual bool HasDepthOfField() { return false; }

    virtual Ray GenerateRay(const Vec2f& uv, float time);
    virtual Ray GenerateRay(const Vec2f& uv, const Vec2f& auv, float time) ;

    virtual Matrix4d ProjectionMatrixGL(float time);

    Vec2f& FovDegree() { return fovDegree; } // DANGEROUS!!! call _Init if this changes
    void InitFromRefs() { _Init(); }
	
    virtual shared_ptr<Lens> Clone();

	static string serialize_typename();
	virtual void serialize(Archive * a);

protected:
	Vec3f _ImagePlanePoint(const Vec2f& uv);
	void _Init();

protected:
    Vec2f         fovDegree;
    Vec2f         imagePlaneSize;
};

class ThinLens : public Lens {
public:
	ThinLens() { fovDegree = Vec2f(45, 45); focalDistance = 1; apertureSize = 0; _Init(); }
	ThinLens(const Vec2f& fovDegree, float focalDistance, float apertureSize);

    virtual bool HasDepthOfField() { return apertureSize > 0; }

    virtual Ray GenerateRay(const Vec2f& uv, float time);
    virtual Ray GenerateRay(const Vec2f& uv, const Vec2f& auv, float time);

    virtual Matrix4d ProjectionMatrixGL(float time);

    Vec2f& FovDegree() { return fovDegree; } // DANGEROUS!!! call _Init if this changes
    float& FocalDistance() { return focalDistance; } // DANGEROUS!!! call _Init if this changes
    float& ApertureSize() { return apertureSize; } // DANGEROUS!!! call _Init if this changes
    void InitFromRefs() { _Init(); }

    virtual shared_ptr<Lens> Clone();

	static string serialize_typename();
	virtual void serialize(Archive * a);
	
protected:
    Vec3f _FocalPlanePoint(const Vec2f& uv);
	Vec3f _LensPlanePoint(const Vec2f& uv);

	void _Init();

    Vec2f         fovDegree;
    float         focalDistance;
    float         apertureSize;

    Vec2f         imagePlaneSize;
    Vec2f         lensPlaneSize;
};

#endif
