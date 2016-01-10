#include "MdLightcutter.h"
#include <scene/camera.h>
#include <scene/background.h>
#include <lighttree/GatherTreeBuilder.h>

MdLightcutter::MdLightcutter(MdLightTree *lightTree, Scene *scene, RayEngine *engine, uint32_t maxCutSize)
    : _scene(scene), _engine(engine), _maxCutSize(maxCutSize), _lightTree(lightTree) 
{
    float radius = (_engine->ComputeBoundingBox().Diagonal() / 2.0f) * 0.05f;
    _clamp = radius * radius;
}

MdLightcutter::~MdLightcutter(void) {}

void MdLightcutter::Lightcut(Image<Vec3f> *image, Image<uint32_t> *cutImage, uint32_t samples, ReportHandler *report)
{
    float normScale = _engine->ComputeBoundingBox().Diagonal() / 8.0f;

    shared_ptr<GatherTreeBuilder> gatherTreeBuilder = 
        shared_ptr<GatherTreeBuilder>(new GatherTreeBuilder(_scene, _engine, image->Width(), image->Height(), samples, normScale));

	Image<uint64_t> randSeeds(image->Width(), image->Height());
	for (uint32_t i = 0; i < randSeeds.Size(); i++)
	{
		static minstd_rand0 seeder;
		uint64_t seed = seeder();
		randSeeds.ElementAt(i) = seed; 
	}

    const float time = 0.0f; // lightcut do not support motion blur;
    if (report) report->beginActivity("Multi-dimensional Lightcutting..");
    for(uint32_t j = 0; j < image->Height(); j ++) 
    {
        for(uint32_t i = 0; i < image->Width(); i ++) 
        {
            vector<GatherPoint> points;
			Vec3f background;
            GatherNode *gpRoot = gatherTreeBuilder->Build(i, j, randSeeds.ElementAt(i, j), points, &background);
            uint32_t cutSize = 0;
            Vec3f L = _EvaluateLightcut(gpRoot, cutSize);
            image->ElementAt(i, image->Height() - j - 1) = background + L;
            cutImage->ElementAt(i, cutImage->Height() - j - 1) = cutSize;
            delete gpRoot;
            if(report) report->progress((float)j / (float)(image->Height()), 1);
        }
    }
    if (report) report->endActivity();
}

struct MdLightcutHeapItem
{
    MdLightcutHeapItem(LightTreeNode *node, GatherNode *gpNode, uint32_t ltIdx, const Vec3f &ub, const Vec3f &est, bool refineLight, uint32_t refineSeq) 
        : _ltNode(node), _gpNode(gpNode), _ltIdx(ltIdx), _estimate(est), _upperBound(ub), _refineLight(refineLight), _refineSeq(refineSeq) {
        _bound = _upperBound.Average();
    }

    bool operator< (const MdLightcutHeapItem &p2) const {
        return _bound == p2._bound ? (_ltNode < p2._ltNode) : _bound < p2._bound;
    }
    LightTreeNode   *_ltNode;
    GatherNode      *_gpNode;
	uint32_t		_ltIdx;
    Vec3f           _upperBound;
    float           _bound;
    Vec3f           _estimate;
    bool            _refineLight;
    uint32_t        _refineSeq;
};

Vec3f MdLightcutter::_EvaluateLightcut(GatherNode *gpRoot, uint32_t &cutSize)
{
    if (gpRoot->gp == 0)
        return Vec3f::Zero();

    vector<MdLightcutHeapItem> lightcut;
    lightcut.reserve(_maxCutSize);
    cutSize = 0;

    Vec3f totalEstL, estimate, bound;
    if (_lightTree->GetOrientedRoot())
    {
		bool heuristic;
		MdOrientedLightTreeNode *root = _lightTree->GetOrientedRoot();
#ifdef MULTI_REP
		uint32_t index = _SelectRandomLight(root->lights);
		estimate = _EvalutateNode(root->lights[index], gpRoot->gp);
#else
		uint32_t index = 0;
		estimate = _EvalutateNode(root->light, gpRoot->gp);
#endif // MULTI_REP

        bound = _ComputeUpperBound(root, gpRoot, heuristic);
        totalEstL += root->L * estimate * gpRoot->strength;
        cutSize++;
        lightcut.push_back(MdLightcutHeapItem(root, gpRoot, index, bound, estimate, heuristic, 0));
        push_heap(lightcut.begin(), lightcut.end());
    }

    if (_lightTree->GetDirectionalRoot())
    {
		MdDirectionalLightTreeNode *root = _lightTree->GetDirectionalRoot();
        bool heuristic;
#ifdef MULTI_REP
		uint32_t index = _SelectRandomLight(root->lights);
        estimate = _EvalutateNode(root->lights[index], gpRoot->gp);
#else
		uint32_t index = 0;
		estimate = _EvalutateNode(root->light, gpRoot->gp);
#endif
        bound = _ComputeUpperBound(root, gpRoot, heuristic);
        totalEstL += root->L * estimate * gpRoot->strength;
        cutSize++;
        lightcut.push_back(MdLightcutHeapItem(root, gpRoot, index, bound, estimate, heuristic, 0));
        push_heap(lightcut.begin(), lightcut.end());
    }


    while(lightcut.size() > 0 && cutSize < _maxCutSize)
    {
		MdLightcutHeapItem cutItem = lightcut.front();
		float error = cutItem._upperBound.IsZero() ? 0.0f : (cutItem._upperBound / totalEstL).Average();
		if (error <= 0.001f)
			break;

		pop_heap(lightcut.begin(), lightcut.end());
        lightcut.pop_back();

        LightTreeNode* node = cutItem._ltNode;
        GatherNode* gpNode = cutItem._gpNode;
		uint32_t index = cutItem._ltIdx;

        if (node == NULL || gpNode == NULL)
            cout << "null error" << endl;

        const GatherPoint *gp = gpRoot->gp;

        // Refine heuristic
        bool refineLightNode = cutItem._refineLight;
        if (cutItem._refineSeq > 4 && !gpNode->IsLeaf())
            refineLightNode = false;

        if (refineLightNode && !node->IsLeaf())
        {
            if (node->GetNodeType() == ORIENTED_NODE)
            {
				MdOrientedLightTreeNode *ltNode = static_cast<MdOrientedLightTreeNode*>(node);
				Vec3f leftEst, rightEst;
				uint32_t lIdx = 0;
				uint32_t rIdx = 0;
				bool lHeur, rHeur;

				uint32_t seq = cutItem._refineSeq >= 0 ? cutItem._refineSeq + 1 : 0; 
				MdOrientedLightTreeNode *leftNode = ltNode->left;
				MdOrientedLightTreeNode *rightNode = ltNode->right;

#ifdef MULTI_REP
				if(ltNode->lights[index] == leftNode->lights[index]) {
					lIdx = index;
					leftEst = cutItem._estimate;
				} else {
					lIdx = _SelectRandomLight(leftNode->lights);
					leftEst = _EvalutateNode(leftNode->lights[lIdx], gpNode->gp);
				}
#else
				leftEst = (ltNode->light == leftNode->light) ? cutItem._estimate : _EvalutateNode(leftNode->light, gpNode->gp);
#endif
                Vec3f leftBound = _ComputeUpperBound(leftNode, gpNode, lHeur) * gpNode->strength;
                lightcut.push_back(MdLightcutHeapItem(leftNode, gpNode, lIdx, leftBound, leftEst, lHeur, seq));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f lL = leftNode->L * leftEst;

#ifdef MULTI_REP
                
				if(ltNode->lights[index] == rightNode->lights[index]) {
					rIdx = index;
					rightEst = cutItem._estimate;
				} else {
					rIdx = _SelectRandomLight(rightNode->lights);
					rightEst = _EvalutateNode(rightNode->lights[rIdx], gpNode->gp);
				}
#else
				rightEst = (ltNode->light == rightNode->light) ? cutItem._estimate : _EvalutateNode(rightNode->light, gpNode->gp);
#endif

				Vec3f rightBound = _ComputeUpperBound(rightNode, gpNode, rHeur) * gpNode->strength;
                lightcut.push_back(MdLightcutHeapItem(rightNode, gpNode, rIdx, rightBound, rightEst, rHeur, seq));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f rL = rightNode->L * rightEst;

				totalEstL -= (ltNode->L * cutItem._estimate) * gpNode->strength;
				totalEstL += (lL + rL) * gpNode->strength;
				cutSize++;
            }
            else if (node->GetNodeType() == DIRECT_NODE)
            {
				MdDirectionalLightTreeNode *ltNode = static_cast<MdDirectionalLightTreeNode*>(node);
				Vec3f leftEst, rightEst;
				uint32_t lIdx = 0, rIdx = 0;
				bool lHeur, rHeur;
				uint32_t seq = cutItem._refineSeq >= 0 ? cutItem._refineSeq + 1 : 0; 

				MdDirectionalLightTreeNode *leftNode = ltNode->left;
				MdDirectionalLightTreeNode *rightNode = ltNode->right;
#ifdef MULTI_REP
				if(ltNode->lights[index] == leftNode->lights[index]) {
					lIdx = index;
					leftEst = cutItem._estimate;
				} else {
					lIdx = _SelectRandomLight(leftNode->lights);
					leftEst = _EvalutateNode(leftNode->lights[lIdx], gpNode->gp);
				}
#else
				leftEst = (ltNode->light == leftNode->light) ? cutItem._estimate : _EvalutateNode(leftNode->light, gpNode->gp);
#endif

                Vec3f leftBound = _ComputeUpperBound(leftNode, gpNode, lHeur) * gpNode->strength;
                lightcut.push_back(MdLightcutHeapItem(leftNode, gpNode, lIdx, leftBound, leftEst, lHeur, seq));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f lL = leftNode->L * leftEst;

#ifdef MULTI_REP
				if(ltNode->lights[index] == rightNode->lights[index]) {
					rIdx = index;
					rightEst = cutItem._estimate;
				} else {
					rIdx = _SelectRandomLight(rightNode->lights);
					rightEst = _EvalutateNode(rightNode->lights[rIdx], gpNode->gp);
				}
#else
				rightEst = (ltNode->light == rightNode->light) ? cutItem._estimate : _EvalutateNode(rightNode->light, gpNode->gp);
#endif

                Vec3f rightBound = _ComputeUpperBound(rightNode, gpNode, rHeur) * gpNode->strength;
                lightcut.push_back(MdLightcutHeapItem(rightNode, gpNode, rIdx, rightBound, rightEst, rHeur, seq));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f rL = rightNode->L * rightEst;

				totalEstL -= (ltNode->L * cutItem._estimate) * gpNode->strength;
				totalEstL += (lL + rL) * gpNode->strength;
				cutSize++;
			}
        }
        else if (!gpNode->IsLeaf())
        {
            if (node->GetNodeType() == ORIENTED_NODE)
            {
                MdOrientedLightTreeNode *ltNode = static_cast<MdOrientedLightTreeNode*>(node);
                Vec3f leftEst, rightEst;
				bool lHeur, rHeur;
                uint32_t seq = cutItem._refineSeq <= 0 ? cutItem._refineSeq - 1 : 0;
#ifdef MULTI_REP
				OrientedLight* light = ltNode->lights[index];
#else
				OrientedLight* light = ltNode->light;
#endif

                GatherNode *leftNode = gpNode->left;
                leftEst = gpNode->gp == leftNode->gp ? cutItem._estimate : _EvalutateNode(light, leftNode->gp);
                Vec3f leftBound = _ComputeUpperBound(ltNode, leftNode, lHeur) * leftNode->strength;
                lightcut.push_back(MdLightcutHeapItem(ltNode, leftNode, index, leftBound, leftEst, lHeur, 0));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f lL = ltNode->L * leftEst;

                GatherNode *rightNode = gpNode->right;
                rightEst = gpNode->gp == rightNode->gp ? cutItem._estimate : _EvalutateNode(light, rightNode->gp);
                Vec3f rightBound = _ComputeUpperBound(ltNode, rightNode, rHeur) * rightNode->strength;
                lightcut.push_back(MdLightcutHeapItem(ltNode, rightNode, index, rightBound, rightEst, rHeur, 0));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f rL = ltNode->L * rightEst;

                Vec3f t = totalEstL;
                totalEstL -= (ltNode->L * cutItem._estimate) * gpNode->strength;
                totalEstL += lL * leftNode->strength + rL * rightNode->strength;
                cutSize++;
            }
            else if (node->GetNodeType() == DIRECT_NODE)
            {
				MdDirectionalLightTreeNode *ltNode = static_cast<MdDirectionalLightTreeNode*>(node);
				Vec3f leftEst, rightEst;
				bool lHeur, rHeur;
				uint32_t seq = cutItem._refineSeq <= 0 ? cutItem._refineSeq - 1 : 0; 
#ifdef MULTI_REP
				DirLight* light = ltNode->lights[index];
#else
				DirLight* light = ltNode->light;
#endif

                GatherNode *leftNode = gpNode->left;
                leftEst = gpNode->gp == leftNode->gp ? cutItem._estimate : _EvalutateNode(light, leftNode->gp);
                Vec3f leftBound = _ComputeUpperBound(ltNode, leftNode, lHeur) * leftNode->strength;
                lightcut.push_back(MdLightcutHeapItem(ltNode, leftNode, index, leftBound, leftEst, lHeur, 0));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f lL = ltNode->L * leftEst;

                GatherNode *rightNode = gpNode->right;
                rightEst = gpNode->gp == rightNode->gp ? cutItem._estimate : _EvalutateNode(light, rightNode->gp);
                Vec3f rightBound = _ComputeUpperBound(ltNode, rightNode, rHeur) * rightNode->strength;
                lightcut.push_back(MdLightcutHeapItem(ltNode, rightNode, index, rightBound, rightEst, rHeur, 0));
                push_heap(lightcut.begin(), lightcut.end());
                Vec3f rL = ltNode->L * rightEst;

                Vec3f t = totalEstL;
                totalEstL -= (ltNode->L * cutItem._estimate) * gpNode->strength;
                totalEstL += lL * leftNode->strength + rL * rightNode->strength;
                cutSize++;
            }
        }
        else
            cout << "leaf error" << endl;
    }
    return gpRoot->emission + totalEstL;
}

Vec3f MdLightcutter::_EvalutateNode(const OrientedLight* l, const GatherPoint* g )
{
    if (l == NULL)
        return Vec3f::Zero();
    if (g == 0)
        return Vec3f::Zero();

    const OrientedLight& light = *l;
	const GatherPoint& gp = *(g);

    const DifferentialGeometry &dp = gp.isect.dp;
    Vec3f wi = (light.position - dp.P).GetNormalized();
    float maxDist = (light.position - dp.P).GetLength();
    float cosAngle = max(0.0f, light.normal % (-wi));
    float lenSqrEst = max(_clamp, (light.position - dp.P).GetLengthSqr());
    BxdfUnion msu;
    gp.isect.m->SampleReflectance(dp, msu);
    Vec3f brdf = msu.EvalSmoothCos(gp.wo, wi, dp);
    Vec3f estimate = brdf * cosAngle / lenSqrEst * gp.weight;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, gp.isect.rayEpsilon, maxDist, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
            return estimate;
    }
    return Vec3f::Zero();
}

Vec3f MdLightcutter::_EvalutateNode(const DirLight* l, const GatherPoint* g )
{
    if (l == NULL)
        return Vec3f::Zero();
    if (g == 0)
        return Vec3f::Zero();

    const DirLight& light = *l;
    const GatherPoint& gp = *(g);

    const DifferentialGeometry &dp = gp.isect.dp;
    Vec3f wi = -light.normal;
    BxdfUnion msu;
    gp.isect.m->SampleReflectance(dp, msu);
    Vec3f brdf = msu.EvalSmoothCos(gp.wo, wi, dp);
    Vec3f estimate = brdf * gp.weight;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, gp.isect.rayEpsilon, RAY_INFINITY, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
            return estimate;
    }
    return Vec3f::Zero();
}

Vec3f MdLightcutter::_ComputeUpperBound( MdOrientedLightTreeNode* ltNode, GatherNode *gpNode, bool &refineLight )
{
    if (ltNode->IsLeaf() && gpNode->IsLeaf())
        return Vec3f::Zero();

    const GatherPoint& gp = *gpNode->gp;
	if (!gp.isect.m) {
		refineLight = true;
		return Vec3f::Zero();
	}

    const DifferentialGeometry &dp = gp.isect.dp;

    const Range3f &ltBBox = ltNode->bbox;
    const Range3f &gpBBox = gpNode->bbox;

    refineLight = true;
    float lenSqr = Range3f::DistanceSqr(ltBBox, gpBBox);

    float cosBound = 1.0f;
    float cosHalfAngle = ltNode->cone.GetAngleCos();
    if (cosHalfAngle <= 0.0f)
        cosBound = 1.0f;
    else
    {
        Vec3f vv = (Vec3f::Z() ^ ltNode->cone.GetAxis()).GetNormalized();
        Matrix4f mt = Matrix4f::Rotation(vv, -acosf(Vec3f::Z() % ltNode->cone.GetAxis()));

        Range3f xbox(gpBBox.GetMinRef() - ltBBox.GetMaxRef(), gpBBox.GetMaxRef() - ltBBox.GetMinRef());
        Range3f tbox = mt.TransformBBox(xbox);

		Vec3f &xm = tbox.GetMinRef();
		Vec3f &xM = tbox.GetMaxRef();

		float cosTheta;
		if (xM.z > 0)
		{
			float minx2 = (xm.x * xM.x <= 0) ? 0.0f : min(xm.x * xm.x, xM.x * xM.x);
			float miny2 = (xm.y * xM.y <= 0) ? 0.0f : min(xm.y * xm.y, xM.y * xM.y);
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

    Vec3f MatBound = _BoundMaterialOrientedLight(ltNode, gpNode);
    Vec3f upperBound = (MatBound * (cosBound / max(_clamp, lenSqr))).Abs();

	// compute heuristic
	if (Range3f::Overlap(ltBBox, gpBBox))
	{
		refineLight = ltBBox.Diagonal() > gpBBox.Diagonal();
	}
	else
	{
		float ltHeur = 0.0f;
		float gpHeur = 0.0f;
		// light heuristic
		if (!ltNode->IsLeaf())
		{
			float lmin = Range3f::DistanceSqr(ltNode->left->bbox, gpBBox);
			float rmin = Range3f::DistanceSqr(ltNode->right->bbox, gpBBox);

			float redDist = min(lmin, rmin) / lenSqr;
			float redMat = (1.0f - ltNode->cone.GetAngleCos()) * ltNode->bbox.Diagonal() / sqrt(lenSqr);
			ltHeur = redDist * redMat;
		}

		if (!gpNode->IsLeaf())
		{
			float lmin = Range3f::DistanceSqr(ltBBox, gpNode->left->bbox);
			float rmin = Range3f::DistanceSqr(ltBBox, gpNode->right->bbox);

			float redDist = min(lmin, rmin) / lenSqr;
			float redMat = (1.0f - gpNode->normalCone.GetAngleCos()) * gpNode->bbox.Diagonal() / sqrt(lenSqr);

			gpHeur = redDist * redMat;
		}
		refineLight = gpHeur <= ltHeur;
	}

    return (ltNode->L * upperBound).Abs();
}

Vec3f MdLightcutter::_ComputeUpperBound( MdDirectionalLightTreeNode* ltNode, GatherNode *gpNode, bool &refineLight )
{
    if (ltNode->IsLeaf() && gpNode->IsLeaf())
        return Vec3f::Zero();

	const GatherPoint& gp = *gpNode->gp;
	if (!gp.isect.m) {
		refineLight = true;
		return Vec3f::Zero();
	}

    const DifferentialGeometry &dp = gp.isect.dp;
    const Range3f &ltBBox = ltNode->bbox;
    const Range3f &gpBBox = gpNode->bbox;

    refineLight = true;
    float lenSqr = Range3f::DistanceSqr(ltBBox, gpBBox);

	Vec3f MatBound = _BoundMaterialDirLight(ltNode, gpNode);
	Vec3f upperBound = MatBound.Abs();

    // compute heuristic
	if (Range3f::Overlap(ltBBox, gpBBox))
	{
		refineLight = ltBBox.Diagonal() > gpBBox.Diagonal();
	}
	else
	{
		float ltHeur = 0.0f;
		float gpHeur = 0.0f;
		// light heuristic
		if (!ltNode->IsLeaf())
		{
			float lmin = Range3f::DistanceSqr(ltNode->left->bbox, gpBBox);
			float rmin = Range3f::DistanceSqr(ltNode->right->bbox, gpBBox);

			float redMat = ltNode->bbox.Diagonal() / sqrt(lenSqr);
			ltHeur = redMat;
		}

		if (!gpNode->IsLeaf())
		{
			float lmin = Range3f::DistanceSqr(ltBBox, gpNode->left->bbox);
			float rmin = Range3f::DistanceSqr(ltBBox, gpNode->right->bbox);

			float redMat = gpNode->bbox.Diagonal() / sqrt(lenSqr);
			gpHeur = redMat;
		}
		refineLight = gpHeur <= ltHeur;
	}

    return (ltNode->L * upperBound).Abs();
}

Vec3f MdLightcutter::_BoundMaterialOrientedLight( MdOrientedLightTreeNode* ltNode, GatherNode *gpNode)
{
	const GatherPoint& gp = *(gpNode->gp);
    const DifferentialGeometry &dp = gp.isect.dp;

    const Range3f &ltBBox = ltNode->bbox;
    const Range3f &gpBBox = gpNode->bbox;
    const GLPhongApproximation &matApprox = gpNode->mat;

    Range3f xbox(ltBBox.GetMinRef() - gpBBox.GetMaxRef(), ltBBox.GetMaxRef() - gpBBox.GetMinRef());
    float cosBound = 1.0f;
    float cosHalfAngle = gpNode->normalCone.GetAngleCos();
    if (cosHalfAngle <= 0.0f)
        cosBound = 1.0f;
    else
    {
        Vec3f vv = (Vec3f::Z() ^ gpNode->normalCone.GetAxis()).GetNormalized();
        Matrix4f mt = Matrix4f::Rotation(vv, -acosf(Vec3f::Z() % gpNode->normalCone.GetAxis()));

        Range3f tbox = mt.TransformBBox(xbox);

        Vec3f &xm = tbox.GetMinRef();
        Vec3f &xM = tbox.GetMaxRef();

        float cosTheta;
        if (xM.z > 0)
        {
			float minx2 = (xm.x * xM.x <= 0) ? 0.0f : min(xm.x * xm.x, xM.x * xM.x);
			float miny2 = (xm.y * xM.y <= 0) ? 0.0f : min(xm.y * xm.y, xM.y * xM.y);
			float maxz2 = xM.z * xM.z;
            cosTheta = xM.z / sqrt(minx2 + miny2 + maxz2);
        }
        else
            cosTheta = 0.0f;

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

    if (!matApprox.Ks.IsZero() && matApprox.n != 0)
    {
        //Phong material
        float glossyCosBound;
        float glossyCosTheta = 1.0f;
        Vec3f R = gpNode->mirrorCone.GetAxis();

        float glossyCosHalfAngle = gpNode->mirrorCone.GetAngleCos();
        if (glossyCosHalfAngle <= 0.0f)
            glossyCosBound = 1.0f;
        else
        {
            Vec3f gv = (Vec3f::Z() ^ R).GetNormalized();
            Matrix4f gmt = Matrix4f::Rotation(gv, -acosf(Vec3f::Z() % R));

            Range3f gbox = gmt.TransformBBox(xbox);

            Vec3f &gm = gbox.GetMinRef();
            Vec3f &gM = gbox.GetMaxRef();

            if (gM.z > 0)
            {
				float minx2 = (gm.x * gM.x <= 0) ? 0.0f : min(gm.x * gm.x, gM.x * gM.x);
				float miny2 = (gm.y * gM.y <= 0) ? 0.0f : min(gm.y * gm.y, gM.y * gM.y);
                float maxz2 = gM.z * gM.z;
                glossyCosTheta = gM.z / sqrt(minx2 + miny2 + maxz2);
            }
            else
                glossyCosTheta = 0.0f;

            glossyCosTheta = clamp(glossyCosTheta, 0.0f, 1.0f);
            assert(glossyCosTheta <= 1.0f);

            if (glossyCosTheta > glossyCosHalfAngle)
                glossyCosBound = 1.0f;
            else
            {
                float sinHalfAngle = sqrt(1 - glossyCosHalfAngle * glossyCosHalfAngle);
                float sinTheta = sqrt(1 - glossyCosTheta * glossyCosTheta);
                glossyCosBound = clamp(glossyCosTheta * glossyCosHalfAngle + sinTheta * sinHalfAngle, 0.0f, 1.0f);
                assert(glossyCosBound >= 0.0f && glossyCosBound <= 1.0f);
            }
        }
        return (matApprox.Kd + (matApprox.Ks * pow(glossyCosBound, matApprox.n))) * cosBound;
    }
    return matApprox.Kd * cosBound;
}

Vec3f MdLightcutter::_BoundMaterialDirLight( MdDirectionalLightTreeNode* ltNode, GatherNode *gpNode)
{
    const GatherPoint& gp = *(gpNode->gp);
    const DifferentialGeometry &dp = gp.isect.dp;
    //Material *m = gp.isect.m;

    const Range3f &ltBBox = ltNode->bbox;
    const Range3f &gpBBox = gpNode->bbox;

    const GLPhongApproximation &matApprox = gpNode->mat;

    Range3f xbox;
    xbox.Grow(-ltBBox.GetMinRef());
    xbox.Grow(-ltBBox.GetMaxRef());
    float cosBound = 1.0f;
    float cosHalfAngle = gpNode->normalCone.GetAngleCos();
    if (cosHalfAngle <= 0.0f)
        cosBound = 1.0f;
    else
    {
        Vec3f vv = (Vec3f::Z() ^ gpNode->normalCone.GetAxis()).GetNormalized();
        Matrix4f mt = Matrix4f::Rotation(vv, -acosf(Vec3f::Z() % gpNode->normalCone.GetAxis()));

        Range3f tbox = mt.TransformBBox(xbox);

        Vec3f &xm = tbox.GetMinRef();
        Vec3f &xM = tbox.GetMaxRef();

        float cosTheta;
        if (xM.z > 0)
        {
			float minx2 = (xm.x * xM.x <= 0) ? 0.0f : min(xm.x * xm.x, xM.x * xM.x);
			float miny2 = (xm.y * xM.y <= 0) ? 0.0f : min(xm.y * xm.y, xM.y * xM.y);
			float maxz2 = xM.z * xM.z;
            cosTheta = xM.z / sqrt(minx2 + miny2 + maxz2);
        }
        else
            cosTheta = 0.0f;

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

    if (!matApprox.Ks.IsZero() && matApprox.n != 0)
    {
        //Phong material
        float glossyCosBound;
        float glossyCosTheta = 1.0f;
        Vec3f R = gpNode->mirrorCone.GetAxis();

        float glossyCosHalfAngle = gpNode->mirrorCone.GetAngleCos();
        if (glossyCosHalfAngle <= 0.0f)
            glossyCosBound = 1.0f;
        else
        {
            Vec3f gv = (Vec3f::Z() ^ R).GetNormalized();
            Matrix4f gmt = Matrix4f::Rotation(gv, -acosf(Vec3f::Z() % R));

            Range3f gbox = gmt.TransformBBox(xbox);

            Vec3f &gm = gbox.GetMinRef();
            Vec3f &gM = gbox.GetMaxRef();

            if (gM.z > 0)
            {
				float minx2 = (gm.x * gM.x <= 0) ? 0.0f : min(gm.x * gm.x, gM.x * gM.x);
				float miny2 = (gm.y * gM.y <= 0) ? 0.0f : min(gm.y * gm.y, gM.y * gM.y);
                float maxz2 = gM.z * gM.z;
                glossyCosTheta = gM.z / sqrt(minx2 + miny2 + maxz2);
            }
            else
                glossyCosTheta = 0.0f;

            glossyCosTheta = clamp(glossyCosTheta, 0.0f, 1.0f);
            assert(glossyCosTheta <= 1.0f);

            if (glossyCosTheta > glossyCosHalfAngle)
                glossyCosBound = 1.0f;
            else
            {
                float sinHalfAngle = sqrt(1 - glossyCosHalfAngle * glossyCosHalfAngle);
                float sinTheta = sqrt(1 - glossyCosTheta * glossyCosTheta);
                glossyCosBound = clamp(glossyCosTheta * glossyCosHalfAngle + sinTheta * sinHalfAngle, 0.0f, 1.0f);
                assert(glossyCosBound >= 0.0f && glossyCosBound <= 1.0f);
            }
        }

        return (matApprox.Kd + (matApprox.Ks * pow(glossyCosBound, matApprox.n))) * cosBound;
    }
    return matApprox.Kd * cosBound;
}
