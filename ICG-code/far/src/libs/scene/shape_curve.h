#ifndef _CURVE_H_
#define _CURVE_H_

#include "shape.h"
#include <misc/arrays.h>

class CurveShape : public Shape {
public:
    CurveShape() { radius = 1; }
	
    CurveShape(shared_ptr<carray<Vec3f> > pos, shared_ptr<carray<Vec3f> > tangent, shared_ptr<carray<Vec2f> > texcoord, shared_ptr<carray<int> > idxs, 
        float radius, shared_ptr<carray<float> > vradius);
    CurveShape(const carray<Vec3f>& pos, const carray<Vec3f>& tangent, const carray<Vec2f>& texcoord, const carray<Vec2i>& idxs, 
        float radius, const carray<float>& vradius);

    virtual void CollectStats(StatsManager& stats);

    virtual bool IsStatic() { return true; }
    virtual Intervalf ComputeAnimationInterval() { return Intervalf::Invalid(); }

    uint64_t NSegments() { return segmentArray.size(); }
    uint64_t NVertices() { return posArray.size(); }

	bool HasVertexTangent() { return !tangentArray.empty(); }
	bool HasVertexTexCoord() { return !uvArray.empty(); }
	bool HasVertexRadius() { return !radiusArray.empty(); }

    carray<Vec3f>& PosArray() { return posArray; }
    carray<Vec3f>& TangentArray() { return tangentArray; }
    carray<Vec2f>& UvArray() { return uvArray; }
    carray<Vec2i>& SegmentArray() { return segmentArray; }
    carray<float>& RadiusArray() { return radiusArray; }
    float& Radius() { return radius; }

    Vec3f Pos(int segment, int vertex) { return posArray[segmentArray[segment][vertex]]; }
    Vec3f Tangent(int segment, int vertex) { return tangentArray[segmentArray[segment][vertex]]; }
    Vec2f Uv(int segment, int vertex) { return uvArray[segmentArray[segment][vertex]]; }
	float Radius(int segment, int vertex) { if(!radiusArray.empty()) return radius * radiusArray[segmentArray[segment][vertex]]; else return radius; }
    Vec3f SegmentTangent(int segment) { 
        return (Pos(segment,1) - Pos(segment,0)).GetNormalized();
    }

    virtual float ComputeArea(float time);
    virtual Range3f ComputeBoundingBox(float time);

    virtual bool ApplyXform(shared_ptr<Xform> xform) { return false; }

    virtual void Tesselate(shared_ptr<TesselationCache> cache, float time);
    virtual void Tesselate(shared_ptr<TesselationCache> cache, const Intervalf& time, int timeSamples) { Tesselate(cache,time.Middle()); }

	static string serialize_typename();
	virtual void serialize(Archive* a);

protected:
    carray<Vec3f> posArray; 
    carray<Vec3f> tangentArray; 
    carray<Vec2i> segmentArray;
    carray<float> radiusArray; 
    float radius; // scales the radiusArray if not present
    carray<Vec2f> uvArray; 
};

#endif
