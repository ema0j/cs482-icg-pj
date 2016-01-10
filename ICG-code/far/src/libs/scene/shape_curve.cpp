#include "shape_curve.h"
#include "intersectionMethods.h"

CurveShape::CurveShape(shared_ptr<carray<Vec3f> > pos, shared_ptr<carray<Vec3f> > tangent, shared_ptr<carray<Vec2f> > texcoord, shared_ptr<carray<int> > idxs, 
    float radius, shared_ptr<carray<float> > vradius) {
    posArray = *pos;
    if(tangent) tangentArray = *tangent;
    if(idxs) {
        segmentArray.resize(idxs->size()/2); 
        for(uint32_t s = 0; s < segmentArray.size(); s ++) { segmentArray[s][0] = idxs->at(s*2+0); segmentArray[s][1] = idxs->at(s*2+1); }
    }
    if(texcoord) uvArray = *texcoord;
    this->radius = radius;
    if(vradius) radiusArray = *vradius;
}

CurveShape::CurveShape(const carray<Vec3f>& pos, const carray<Vec3f>& tangent, const carray<Vec2f>& texcoord, const carray<Vec2i>& idxs, 
    float radius, const carray<float>& vradius) {
    posArray = pos;
    tangentArray = tangent;
    segmentArray = idxs;
    uvArray = texcoord;
    this->radius = radius;
    radiusArray = vradius;
}

void CurveShape::CollectStats(StatsManager& stats) {
    StatsCounterVariable* curves = stats.GetVariable<StatsCounterVariable>("Shapes", "Curve Sets");
    StatsCounterVariable* segments = stats.GetVariable<StatsCounterVariable>("Shapes", "Curve Segments");
    StatsCounterVariable* vertices = stats.GetVariable<StatsCounterVariable>("Shapes", "Curve Segments Vertices");
    StatsCounterVariable* memory = stats.GetVariable<StatsCounterVariable>("Shapes", "Memory");

    curves->Increment();
    segments->Increment(NSegments());
    vertices->Increment(NVertices());
    memory->Increment(sizeof(*this) + 
        posArray.getMemoryAllocated() + 
        segmentArray.getMemoryAllocated() + 
        (HasVertexTangent())?tangentArray.getMemoryAllocated():0 +
        (HasVertexRadius())?radiusArray.getMemoryAllocated():0 +
        (HasVertexTexCoord())?uvArray.getMemoryAllocated():0);
}

void CurveShape::Tesselate(shared_ptr<TesselationCache> cache, float time) {
    if(!cache->SupportSegments()) { cerr << "cannot sample primitive" << endl; return; }

    cache->AddSegments(posArray, tangentArray, uvArray, radius, segmentArray);
}

float CurveShape::ComputeArea(float time) {
    float area = 0;
    for(int i = 0; i < NSegments(); i ++) area += ElementOperations::CylinderArea(Pos(i,0),Pos(i,1),Radius());
    return area;
}

Range3f CurveShape::ComputeBoundingBox(float time) {
    Range3f ret;
    for(int i = 0; i < NSegments(); i ++) ret.Grow(ElementOperations::CylinderBoundingBox(Pos(i,0),Pos(i,1),Radius()));
    return ret;
}

string CurveShape::serialize_typename() { return "CurveShape"; } 

void CurveShape::serialize(Archive* a) {
    a->member("pos", posArray);
    a->optional("tangent", tangentArray, carray<Vec3f>());
    a->member("segments", segmentArray);
    a->optional("uv", uvArray, carray<Vec2f>());
	a->optional("radius", radius, 1.0f);
	a->optional("vradius", radiusArray, carray<float>());
}

