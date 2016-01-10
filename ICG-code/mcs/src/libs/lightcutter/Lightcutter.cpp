#include "Lightcutter.h"
#include <scene/camera.h>
#include <scene/background.h>

#define MAX_RAYTRACE_DEPTH 10

struct LightcutHeapItem
{
    LightcutHeapItem(LightTreeNode *node, Vec3f ub, Vec3f est = Vec3f::Zero()) : _node(node), _estimate(est), _upperBound(ub)
    {
        _e = _upperBound.Average();
    }
    bool operator< (const LightcutHeapItem &p2) const 
    {
        return _e == p2._e ? (_node < p2._node) : _e < p2._e;
    }
    LightTreeNode   *_node;
    Vec3f           _upperBound;
    float           _e;
    Vec3f           _estimate;
};


Lightcutter::Lightcutter(LightTree *lightTree, Scene *scene, RayEngine *engine, float error, uint32_t maxCutSize) 
    : _scene(scene), _engine(engine), _maxCutSize(maxCutSize), _lightTree(lightTree), _error(error)
{
    float radius = (_engine->ComputeBoundingBox().Diagonal() / 2.0f) * 0.05f;
    _clamp = radius * radius;
}

Lightcutter::~Lightcutter(void)
{
}

void Lightcutter::Lightcut(Image<Vec3f> *image, uint32_t samples, Image<uint32_t> *cutImage, ReportHandler *report)
{
    const float time = 0.0f; // lightcut do not support motion blur;
    if (report) report->beginActivity("Lightcutting..");
    Vec2f pixelSize(1.0f/image->Width(), 1.0f/image->Height());

	StratifiedPathSamplerStd sampler;
    for(uint32_t j = 0; j < image->Height(); j ++) 
    {
        for(uint32_t i = 0; i < image->Width(); i ++) 
        {
            Vec2f pixel(Vec2i(i,j));
            sampler.BeginPixel(samples);
            uint32_t cutSize = 0;
            Vec3f L;
            for (uint32_t s = 0; s < samples; s++)
            {
                uint32_t cs = 0;
                Vec2f puv = (pixel + ((samples == 1) ? Vec2f(0.5f, 0.5f) : sampler.Pixel())) * pixelSize;
                Ray ray = _scene->MainCamera()->GenerateRay(puv, time);
                L += EvaluateLightcut(ray, cs) / static_cast<float>(samples);
                cutSize += cs;
                sampler.NextPixelSample();
            }
            sampler.EndPixel();
            image->ElementAt(i, image->Height() - j - 1) = L;
            if(cutImage) cutImage->ElementAt(i, cutImage->Height() - j - 1) = cutSize;
            if(report) report->progress((float)j / (float)(image->Height()), 1);
        }
    }
    if (report) report->endActivity();
}

Vec3f Lightcutter::EvaluateLightcut( const Ray &r, uint32_t &cutSize )
{
	Ray ray = r;
	Vec3f throughput = Vec3f::One();
	Intersection isect;
	uint32_t depth = 0;
	while(true)
	{
		if(!_engine->Intersect(ray, &isect)) {
			if(_scene->MainBackground()) 
				return throughput * _scene->MainBackground()->SampleBackground(ray.D,ray.time);
			else 
				return Vec3f();
		}

		BxdfUnion msu;
        isect.m->SampleReflectance(isect.dp, msu);

		Vec3f wo = -ray.D;
		DifferentialGeometry &dp = isect.dp;

        if (msu.HasDelta())
        {
            if (depth > MAX_RAYTRACE_DEPTH)
                return Vec3f();
            BxdfSample bxdfSample = msu.SampleCos(DELTA_BXDF, wo, dp, Vec2f::Zero(), 0.0f);
            if (!bxdfSample.Valid())
                return Vec3f();
            throughput *= bxdfSample.brdfCos;
            ray = Ray(dp.P, bxdfSample.wi, isect.rayEpsilon, RAY_INFINITY, ray.time);
            depth++;
            continue;
        }
        else
		{
			uint32_t cs = 0;
			Vec3f Ld = Vec3f::Zero();
            Ld += _EvaluateCut(dp, wo, isect.rayEpsilon, isect.m, cs);
			cutSize += cs;

			Vec3f Li = msu.Emission(wo,dp) + Ld;
			return throughput * Li;
		}
	}
}

Vec3f Lightcutter::_EvalutateNode(const OrientedLightTreeNode *node, DifferentialGeometry& dp, const Vec3f &wo, float rayEpsilon, Material *ms)
{
    OrientedLight& light = *(node->light);
    Vec3f wi = (light.position - dp.P).GetNormalized();
    float maxDist = (light.position - dp.P).GetLength();
    float cosAngle = max(0.0f, light.normal % (-wi));
    float lenSqrEst = max(_clamp, (light.position - dp.P).GetLengthSqr());
    //float lenSqrEst = (light.position - dp.P).GetLengthSqr();
    BxdfUnion msu;
    ms->SampleReflectance(dp, msu);
    Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
    Vec3f estimate = brdf * cosAngle / lenSqrEst;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, rayEpsilon, maxDist, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
        {
            return estimate;
        }
    }
    return Vec3f::Zero();
}

Vec3f Lightcutter::_EvalutateNode(const DirectionalLightTreeNode *node, DifferentialGeometry& dp, const Vec3f &wo, float rayEpsilon, Material *ms)
{
    DirLight* light = node->light;
    Vec3f wi = -light->normal;
    BxdfUnion msu;
    ms->SampleReflectance(dp, msu);
    Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
    Vec3f estimate = brdf;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, rayEpsilon, RAY_INFINITY, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
        {
            return estimate;
        }
    }
    return Vec3f::Zero();
}

Vec3f Lightcutter::_ComputeUpperBound(const OrientedLightTreeNode *node, DifferentialGeometry & dp, const Vec3f &wo, Material *ms)
{
    OrientedLight* light = node->light;
    Vec3f wi = (light->position - dp.P).GetNormalized();

    const Vec3f &m = node->bbox.GetMin();
    const Vec3f &M = node->bbox.GetMax();
    Vec3f bp = dp.P.Clamp(m, M);
    float lenSqr = max(_clamp, (dp.P - bp).GetLengthSqr());

    float cosBound = 1.0f;
    float cosHalfAngle = node->cone.GetAngleCos();
    if (cosHalfAngle <= 0.0f)
        cosBound = 1.0f;
    else
    {
        Vec3f vv = (Vec3f::Z() ^ node->cone.GetAxis()).GetNormalized();
        Matrix4f mt = Matrix4f::Rotation(vv, -acosf(Vec3f::Z() % node->cone.GetAxis()));

        Vec3f corners[8];
        Range3f xbox = Range3f::Empty();
        node->bbox.GetCorners(corners);
        for (int i = 0; i < 8; i++)
            xbox.Grow(mt.TransformPoint(dp.P - corners[i]));


        Vec3f &xm = xbox.GetMinRef();
        Vec3f &xM = xbox.GetMaxRef();

        float cosTheta;
        if (xM.z > 0)
        {
            float minx2, miny2;
            if (xm.x * xM.x <= 0)
                minx2 = 0.0f;
            else
                minx2 = min(xm.x * xm.x, xM.x * xM.x);

            if (xm.y * xM.y <= 0)
                miny2 = 0.0f;
            else
                miny2 = min(xm.y * xm.y, xM.y * xM.y);
            float maxz2 = xM.z * xM.z;
            cosTheta = xM.z / sqrt(minx2 + miny2 + maxz2);
        }
        else
            cosTheta = 0;
        
        cosTheta = clamp(cosTheta, 0.0f, 1.0f);

        if (cosTheta > cosHalfAngle)
            cosBound = 1.0f;
        else
        {
            float sinHalfAngle = sqrt(1 - cosHalfAngle * cosHalfAngle);
            float sinTheta = sqrt(1 - cosTheta * cosTheta);
            cosBound = clamp(cosTheta * cosHalfAngle + sinTheta * sinHalfAngle, 0.0f, 1.0f);
            assert(cosBound >= 0.0f && cosBound <= 1.0f);
        }
    }
    return (node->L * _BoundMaterial(node->bbox, wo, dp, ms, false) * (cosBound / lenSqr)).Abs();
}

Vec3f Lightcutter::_ComputeUpperBound(const DirectionalLightTreeNode *node, DifferentialGeometry & dp, const Vec3f &wo, Material *ms)
{
    DirLight* light = node->light;
    Vec3f wi = -light->normal;
    return (node->L * _BoundMaterial(node->bbox, wo, dp, ms, true)).Abs();
}


Vec3f Lightcutter::_BoundMaterial( const Range3f &bbox, const Vec3f &wo, const DifferentialGeometry & dp, const Material *m, bool dirLight )
{
    GLPhongApproximation matApprox = m->ApprtoximateAsGLPhong(dp);
    Vec3f vv = (Vec3f::Z() ^ dp.N).GetNormalized();
    Matrix4f mt = Matrix4f::Rotation(vv, -acosf(Vec3f::Z() % dp.N));

    Vec3f corners[8];
    Range3f xbox = Range3f::Empty();
    bbox.GetCorners(corners);
    for (int i = 0; i < 8; i++)
    {
        if (!dirLight)
            corners[i] -= dp.P;
        else
            corners[i] *= -1;
        xbox.Grow(mt.TransformPoint(corners[i]));
    }

    Vec3f &xm = xbox.GetMinRef();
    Vec3f &xM = xbox.GetMaxRef();

    float cosTheta;
    if (xM.z > 0)
    {
        float minx2, miny2;
        if (xm.x * xM.x <= 0)
            minx2 = 0.0f;
        else
            minx2 = min(xm.x * xm.x, xM.x * xM.x);

        if (xm.y * xM.y <= 0)
            miny2 = 0.0f;
        else
            miny2 = min(xm.y * xm.y, xM.y * xM.y);
        float maxz2 = xM.z * xM.z;
        cosTheta = xM.z / sqrt(minx2 + miny2 + maxz2);
    }
    else
        cosTheta = 0.0f;

    assert(cosTheta >= 0.0f && cosTheta <= 1.0f);
    cosTheta = clamp(cosTheta, 0.0f, 1.0f);

    if (!matApprox.Ks.IsZero() && matApprox.n != 0)
    {
        //Phong material
        float glossyCosBound = 1.0f;
        Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
        if (R == wo || dp.N == wo)
        {
            R = wo;
        }

        Vec3f gv = (Vec3f::Z() ^ R).GetNormalized();
        Matrix4f gmt = Matrix4f::Rotation(gv, -acosf(Vec3f::Z() % R));

        Range3f gbox = Range3f::Empty();
        for (int i = 0; i < 8; i++)
            gbox.Grow(gmt.TransformPoint(corners[i]));

        Vec3f &gm = gbox.GetMinRef();
        Vec3f &gM = gbox.GetMaxRef();

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

        return (matApprox.Kd + (matApprox.Ks * pow(glossyCosBound, matApprox.n))) * cosTheta;
    }

    return matApprox.Kd * cosTheta;
}

Vec3f Lightcutter::_EvaluateCut( DifferentialGeometry &dp, const Vec3f &wo, float rayEpsilon, Material *m, uint32_t &cs )
{
    vector<LightcutHeapItem> lightcut;
    lightcut.reserve(_maxCutSize);

    Vec3f totalEstL, estimate, bound;
    if (_lightTree->GetOrientedRoot())
    {
        estimate = _EvalutateNode(_lightTree->GetOrientedRoot(), dp, wo, rayEpsilon, m);
        bound = _ComputeUpperBound(_lightTree->GetOrientedRoot(), dp, wo, m);
        totalEstL += _lightTree->GetOrientedRoot()->L * estimate;
        cs++;
        lightcut.push_back(LightcutHeapItem(_lightTree->GetOrientedRoot(), bound, estimate));
        push_heap(lightcut.begin(), lightcut.end());
    }

    if (_lightTree->GetDirectionalRoot())
    {
        estimate = _EvalutateNode(_lightTree->GetDirectionalRoot(), dp, wo, rayEpsilon, m);
        bound = _ComputeUpperBound(_lightTree->GetDirectionalRoot(), dp, wo, m);
        totalEstL += _lightTree->GetDirectionalRoot()->L * estimate;
        cs++;
        lightcut.push_back(LightcutHeapItem(_lightTree->GetDirectionalRoot(), bound, estimate));
        push_heap(lightcut.begin(), lightcut.end());
    }

    vector<Vec3f> leafContrib;
    while(lightcut.size() > 0 && cs < _maxCutSize)
    {
        pop_heap(lightcut.begin(), lightcut.end());
        LightcutHeapItem cutItem = lightcut.back();
        //float error = (cutItem._upperBound / totalEstL).Average();
        float error = cutItem._upperBound.IsZero() ? 0.0f : (cutItem._upperBound / totalEstL).Average();
        if (error <= _error)
            break;
        lightcut.pop_back();

        if (cutItem._node == NULL || cutItem._node->IsLeaf())
            continue;

        if(cutItem._node->GetNodeType() == ORIENTED_NODE)
        {
            OrientedLightTreeNode *node = static_cast<OrientedLightTreeNode*>(cutItem._node);
            OrientedLightTreeNode *leftNode = node->left;
            Vec3f leftEst = (node->light == leftNode->light) ? cutItem._estimate : _EvalutateNode(leftNode, dp, wo, rayEpsilon, m);
            Vec3f lL = leftNode->L * leftEst;

            OrientedLightTreeNode *rightNode = node->right;
            Vec3f rightEst = (node->light == rightNode->light) ? cutItem._estimate : _EvalutateNode(rightNode, dp, wo, rayEpsilon, m);
            Vec3f rL = rightNode->L * rightEst;

            totalEstL -= node->L * cutItem._estimate;
            totalEstL += lL + rL; 
            cs++;

            if (leftNode->IsLeaf())
                leafContrib.push_back(lL);
            else
            {
                Vec3f leftErr = _ComputeUpperBound(leftNode, dp, wo, m);
                lightcut.push_back(LightcutHeapItem(leftNode, leftErr, leftEst));
                push_heap(lightcut.begin(), lightcut.end());
            }

            if (rightNode->IsLeaf())
                leafContrib.push_back(rL);
            else
            {
                Vec3f rightErr = _ComputeUpperBound(rightNode, dp, wo, m);
                lightcut.push_back(LightcutHeapItem(rightNode, rightErr, rightEst));
                push_heap(lightcut.begin(), lightcut.end());
            }
        }
        else if(cutItem._node->GetNodeType() == DIRECT_NODE)
        {
            DirectionalLightTreeNode *node = static_cast<DirectionalLightTreeNode*>(cutItem._node);
            DirectionalLightTreeNode *leftNode = node->left;
            Vec3f leftEst = (node->light == leftNode->light) ? cutItem._estimate : _EvalutateNode(leftNode, dp, wo, rayEpsilon, m);
            Vec3f lL = leftNode->L * leftEst;

            DirectionalLightTreeNode *rightNode = node->right;
            Vec3f rightEst = (node->light == rightNode->light) ? cutItem._estimate : _EvalutateNode(rightNode, dp, wo, rayEpsilon, m);
            Vec3f rL = rightNode->L * rightEst;

            totalEstL -= node->L * cutItem._estimate;
            totalEstL += lL + rL; 
            cs++;

            if (leftNode->IsLeaf())
                leafContrib.push_back(lL);
            else
            {
                Vec3f leftErr = _ComputeUpperBound(leftNode, dp, wo, m);
                lightcut.push_back(LightcutHeapItem(leftNode, leftErr, leftEst));
                push_heap(lightcut.begin(), lightcut.end());
            }

            if (rightNode->IsLeaf())
                leafContrib.push_back(rL);
            else
            {
                Vec3f rightErr = _ComputeUpperBound(rightNode, dp, wo, m);
                lightcut.push_back(LightcutHeapItem(rightNode, rightErr, rightEst));
                push_heap(lightcut.begin(), lightcut.end());
            }
        }
    }

    //clamping
    //Vec3f threshold = totalEstL * 0.01f;
    //for (uint32_t i = 0; i < leafContrib.size(); i++)
    //{
    //    totalEstL += Vec3f::Min(leafContrib[i], threshold) - leafContrib[i];
    //}
    return totalEstL;
}
