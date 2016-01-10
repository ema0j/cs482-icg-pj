#ifndef _MATRIX_DATA_H_
#define _MATRIX_DATA_H_

struct GatherPoint 
{
    bool			hit;
    Vec3f			emission;
    Vec3f			weight;
    Vec2i           pixel;
    Intersection    isect;
    Vec3f           wo;
};

struct BackgroundPixel 
{
    Vec3f			background;
    Vec2i           pixel;
};

struct GatherGroup
{
    uint32_t			repIdx;
    vector<uint32_t>	indices;
};

#endif // _MATRIX_DATA_H_