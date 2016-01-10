#ifndef _TESSELATION_H_
#define _TESSELATION_H_

#include "element.h"


// arrays of values sampled at equally-spaced times
// packed such that all times for a value are packed close to each other
// sampling include interval.begin() and interval.end()
template<class T>
class tarray {
public:
	tarray() { }
    tarray(uint64_t values, uint64_t times, const Intervalf& interval) { resize(values,times,interval); }
	tarray(const tarray<T>& vv) { i = vv.i; v = vv.v; }

	tarray<T>& operator = (const tarray<T>& vv) { i = vv.i; v = vv.v; return *this; }

    void resize(uint64_t values, uint64_t times, const Intervalf& interval) { i = interval; v.resize(times,values); }
    void clear() { i = Intervalf(); v.clear(); }

    uint64_t size() const { return v.size(); }
    uint64_t times() const { return v.width(); }
    uint64_t values() const { return v.height(); }
    Intervalf interval() const { return i; }
    bool empty() const { return v.empty(); }
    T* data() { return v.data(); }

    T& at(uint64_t value, uint64_t time) { return v.at(time,value); }
    const T& at(uint64_t value, uint64_t time) const { return v.at(time,value); }

    void setAtTime(int time, const carray<T>& vv) { 
        for(int value = 0; value < values(); value ++) at(value,time) = vv[value];
    }
    carray<T> atTime(uint64_t time) const {
        carray<T> vv(values());
        for(int value = 0; value < values(); value ++) vv[value] = at(value,time);
        return vv;
    }

    T interpolate(uint64_t value, float time) const {
        // check if outside of the interal
        if(time <= i.Begin()) { return v[0]; }
        if(time >= i.End()) { return v[v.size()-1]; }
		
        uint64_t segs =  times()-1;
		uint64_t k = (uint64_t) (time * segs);
		
		float t = time * segs - k;

        return at(value,k)*(1-t)+at(value,k+1)*t;
    }

protected:
    Intervalf i;
    carray2<T> v;
};

// support adding simple primitives to a renderer
class TesselationCache {
public:
    virtual bool SupportSpheres() = 0;
    virtual bool SupportSegments() = 0;
    virtual bool SupportTimeSampledTriangles() = 0;

    // if normal is empty, use face normals | if uv is empty, use 0.5
    virtual void AddTriangles(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec3i>& face) = 0;
    // if normal is empty, use face normals | if uv is empty, use 0.5
    virtual void AddQuads(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec4i>& face) = 0;
    // only call if support is provided, might be removed later on
    virtual void AddSpheres(const carray<Vec3f>& pos, const carray<float>& radius) = 0;
    // if tangent is empty, use segment tangents | if us is empty, use 0.5
    virtual void AddSegments(const carray<Vec3f>& pos, const carray<Vec3f>& tangent, const carray<Vec2f>& uv, float radius, const carray<Vec2i>& segment) = 0;

    // if normal is empty, use face normals | if uv is empty, use 0.5
    // in memory, time samples are arranged as attribute[timeSample][id]
    virtual void AddTimeSampledTriangles(const tarray<Vec3f>& pos, const tarray<Vec3f>& normal, const tarray<Vec2f>& uv, const carray<Vec3i>& face) = 0;
};

// includes tessealated objects that vary in time
class IntervalTesselationCache : public TesselationCache {
public:
    virtual bool SupportSampledTriangles() = 0;

    // if normal is empty, use face normals | if uv is empty, use 0.5
    virtual void AddSampledTriangles(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec3i>& face) = 0;
};

// a simple tesselation context that supports only triangles with shared vertex data
// face normals are handled by duplication
// face smoothing is disabled
class TriangleTesselationCache : public TesselationCache {
public:
    carray<Vec3f>& Pos() { return pos; }
    carray<Vec2f>& Uv() { return uv; }
    carray<Vec3f>& Normal() { return normal; }
    carray<Vec3i>& Triangles() { return triangles; }

    virtual Range3f BoundingBox() { return bbox; }

    virtual bool SupportSpheres() { return false; }
    virtual bool SupportSegments() { return false; }
    virtual bool SupportTimeSampledTriangles() { return false; }

    virtual void AddTriangles(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec3i>& triangles);
    virtual void AddQuads(const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2f>& uv, const carray<Vec4i>& quads);
    virtual void AddSpheres(const carray<Vec3f>& pos, const carray<float>& radius) { assert(0); }
    virtual void AddSegments(const carray<Vec3f>& pos, const carray<Vec3f>& tangent, const carray<Vec2f>& uv, float radius, const carray<Vec2i>& segment) { assert(0); }
    virtual void AddTimeSampledTriangles(const tarray<Vec3f>& pos, const tarray<Vec3f>& normal, const tarray<Vec2f>& uv, const carray<Vec3i>& face) { assert(0); }

protected:
    carray<Vec3f> pos;
    carray<Vec2f> uv;
    carray<Vec3f> normal;
    carray<Vec3i> triangles;
    Range3f bbox;

    void _UpdateBBox();
};

#endif

