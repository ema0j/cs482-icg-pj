#include "GatherTreeBuilder.h"
#include <scene/camera.h>
#include <vector>
#include <scene/background.h>
#include "vmath/range6.h"
#include "vmath/range3.h"

using std::vector;

GatherTreeBuilder::GatherTreeBuilder(Scene *scene, RayEngine *engine, uint32_t width, uint32_t height, uint32_t samples, float c) 
    : _scene(scene), _engine(engine), _width(width), _height(height), _samples(samples), _normalScale(c)
{
    _pixelSize = Vec2f(1.0f/_width, 1.0f/_height);
    _radius = (_engine->ComputeBoundingBox().Diagonal() / 2.0f);
}

GatherTreeBuilder::~GatherTreeBuilder(void)
{
}

GatherNode* GatherTreeBuilder::Build( uint32_t i, uint32_t j, uint64_t seed, vector<GatherPoint> &points, Vec3f *background)
{ 
    const float time = 0.0f;
	const float invSamples = 1.0f / _samples;
	StratifiedPathSamplerStd::Engine e(seed);
	StratifiedPathSamplerStd sampler(e);
    sampler.BeginPixel(_samples);
    for (uint32_t s = 0; s < _samples; s++)
    {
        Vec2i pixel(i,j);
        Vec2f puv = (Vec2f(pixel) + sampler.Pixel()) * _pixelSize;
        Ray ray = _scene->MainCamera()->GenerateRay(puv, sampler.Lens(), time);

        GatherPoint gp;
        Vec3f throughput = Vec3f::One();
        bool hit = false;
        uint32_t maxSpecDepth = 10;

        while(_engine->Intersect(ray, &gp.isect))
        {
            BxdfUnion msu;
            gp.isect.m->SampleReflectance(gp.isect.dp, msu);
            if(msu.HasDelta())
            {
                if (maxSpecDepth == 0)
                {
                    hit = false;
                    break;
                }
                maxSpecDepth--;
                BxdfSample sample = msu.SampleCos(DELTA_BXDF, -ray.D, gp.isect.dp, Vec2f::Zero(), 0.0f);
                throughput *= sample.brdfCos;
                ray = Ray(gp.isect.dp.P, sample.wi, gp.isect.rayEpsilon, RAY_INFINITY, ray.time);
                continue;
            }
            else if (msu.HasSmooth())
            {
                gp.emission = throughput * msu.Emission(gp.wo, gp.isect.dp);
                gp.pixel = pixel;
                gp.wo = -ray.D;
                gp.weight = throughput;
                points.push_back(gp);
                hit = true;
            }
            break;
        }
		if (!hit && maxSpecDepth != 0 && _scene->MainBackground())
			*background += throughput * _scene->MainBackground()->SampleBackground(ray.D, ray.time) * invSamples;

        sampler.NextPixelSample();
    }
    sampler.EndPixel();
    return _BuildTree(points, &sampler);
}

GatherNode* GatherTreeBuilder::_BuildTree( vector<GatherPoint> &points, PathSampler* sampler)
{
    if (points.size() == 0)
        return NULL;

    vector<GpKdItem> inputData;
    for (uint32_t s = 0; s < points.size(); s++)
    {
        GpKdItem item;
        item.idx = s;
        item.point = _ComputeKdPoint(points[s]);
        inputData.push_back(item);
    }

    GatherNode *node = _Build(inputData.begin(), inputData.end(), points, sampler);
    return node;
}

Vec6f GatherTreeBuilder::_ComputeKdPoint( GatherPoint &gp )
{
    GLPhongApproximation approx = gp.isect.m->ApprtoximateAsGLPhong(gp.isect.dp);
    Vec3f direction;
    if (approx.HasGlossy())
    {
        float scale = 0.0f;
        if(approx.n < 20.0f)
            scale = 1.0f;
        else if(approx.n < 40.0f)
            scale = 2.0f;
        else if(approx.n < 80.0f)
            scale = 3.0f;
        else
            scale = 4.0f;
        direction = ReflectanceUtils::MirrorDirection(gp.isect.dp.N, gp.wo) * scale  * _normalScale;
    }
    else
        direction = gp.isect.dp.N * _normalScale;

    return Vec6f(gp.isect.dp.P, direction);
}

void GatherTreeBuilder::_UpdateNode(GatherNode* node, PathSampler *sampler)
{
	GatherNode *left = node->left;
	GatherNode *right = node->right;
	node->bbox = Range3f::Union(left->bbox, right->bbox);
	node->normalCone = FastConef::Union(left->normalCone, right->normalCone);
	node->hasGlossy = left->hasGlossy || right->hasGlossy;
	if (left->hasGlossy && right->hasGlossy)
		node->mirrorCone = FastConef::Union(left->mirrorCone, right->mirrorCone);
	else if (right->hasGlossy == node->hasGlossy)
		node->mirrorCone = right->mirrorCone;
	else if (left->hasGlossy == node->hasGlossy)
		node->mirrorCone = left->mirrorCone;

	node->strength = left->strength + right->strength;
	node->emission = left->emission + right->emission;

	node->mat.Kd = Vec3f::Max(left->mat.Kd, right->mat.Kd);
	node->mat.Ks = Vec3f::Max(left->mat.Ks, right->mat.Ks);
	node->mat.n = max(left->mat.n, right->mat.n);


	if (left->gp && right->gp)
		node->gp = sampler->Next1D() > 0.5f ? left->gp : right->gp;
	else if (left->gp) node->gp = left->gp;
	else if (right->gp) node->gp = right->gp;
}


GatherNode* GatherTreeBuilder::_Build( vector<GpKdItem>::iterator start, vector<GpKdItem>::iterator end, vector<GatherPoint> &points, PathSampler* sampler)
{
    assert(end > start);
    if (end - start == 1)
        return _MakeLeaf(start, points);

    Range6f bbox = Range6f::Empty();
    for (vector<GpKdItem>::iterator it = start; it != end; it++)
        bbox.Grow(it->point);

    uint32_t dim = bbox.GetSize().MaxComponentIndex();
    vector<GpKdItem>::iterator mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, [dim](const GpKdItem &d1, const GpKdItem &d2) -> bool {
		return d1.point[dim] < d2.point[dim];
	});

    GatherNode* node = new GatherNode();
    node->left = _Build(start, mid, points, sampler);
    node->right = _Build(mid, end, points, sampler);
	_UpdateNode(node, sampler);
    return node;
}

GatherNode* GatherTreeBuilder::_MakeLeaf( vector<GpKdItem>::iterator it, vector<GatherPoint> &points)
{
    GatherNode *node = new GatherNode();
    GatherPoint &gp = points[it->idx];
    GLPhongApproximation m = gp.isect.m->ApprtoximateAsGLPhong(gp.isect.dp);
    node->gp = &gp;
    node->strength = 1.0f / _samples;
    node->hasGlossy = false;
    node->bbox = Range3f(gp.isect.dp.P, gp.isect.dp.P);
    node->emission = gp.emission;
    node->mat = m;
    if(!m.Kd.IsZero())
        node->normalCone = FastConef(gp.isect.dp.N);
    if (!m.Ks.IsZero() && m.n > 0.0f)
    {
        node->hasGlossy = true;
        node->mirrorCone = FastConef(ReflectanceUtils::MirrorDirection(gp.isect.dp.N, gp.wo));
    }
    return node;
}

float GatherTreeBuilder::_BoundGlossyCos( const Range3f &gbox )
{
    const Vec3f &gm = gbox.GetMinRef();
    const Vec3f &gM = gbox.GetMaxRef();
    float glossyCosBound = 1.0f;
    if (gM.z > 0)
    {
        float minx2, miny2;
        if (gm.x * gM.x <= 0)
            minx2 = 0.0f;
        else
            minx2 = min(gm.x * gm.x, gM.x * gM.x);

        if (gm.y * gM.y <= 0)
            miny2 = 0.0f;
        else
            miny2 = min(gm.y * gm.y, gM.y * gM.y);
        float maxz2 = gM.z * gM.z;
        glossyCosBound = gM.z / sqrt(minx2 + miny2 + maxz2);
    }
    else
        glossyCosBound = 0.0f;

    glossyCosBound = clamp(glossyCosBound, 0.0f, 1.0f);
    assert(glossyCosBound <= 1.0f);
    return glossyCosBound;
}

