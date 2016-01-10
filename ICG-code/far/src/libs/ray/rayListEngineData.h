#ifndef _RAY_LIST_ENGINE_DATA_H_
#define _RAY_LIST_ENGINE_DATA_H_

struct TriangleArray
{
    TriangleArray(carray<Vec3f> &pos, carray<Vec3f> &norm, carray<Vec2f> &texcoord, carray<Vec3i> &fs)
        : positions(pos), faces(fs), normals(norm), uvs(texcoord) {}
    carray<Vec3f>   &positions;
    carray<Vec3i>   &faces;
    carray<Vec2f>   &uvs;
    carray<Vec3f>   &normals;
};

struct SphereArray
{
    SphereArray();
    SphereArray(const Vec3f &pos, float r)
        : centers(1, pos), radius(1, r) { assert(centers.size() == radius.size()); }
    SphereArray(const carray<Vec3f> &pos, const carray<float> &r)
        : centers(pos), radius(r) { assert(centers.size() == radius.size()); }
    carray<Vec3f>   centers;
    carray<float>   radius;
};

struct SegmentArray
{
    SegmentArray(float r, carray<Vec3f> &pos, carray<Vec3f> &tangs, carray<Vec2f> &texcoords, carray<float> &radiusArray, carray<Vec2i> &seg)
        : radius(r), positions(pos), tangents(tangs), uvs(texcoords), radiuss(radiusArray), segments(seg) {}
    float radius;
    carray<Vec3f>   &positions;
    carray<Vec3f>   &tangents;
    carray<Vec2f>   &uvs;
    carray<float>   &radiuss;
    carray<Vec2i>   &segments;
};

#endif // _RAY_LIST_ENGINE_DATA_H_
