#include "LightTree.h"

OrientedLightTreeNode::OrientedLightTreeNode(OrientedLight &l, uint32_t idx) : LightTreeNode(idx)
{
    light = &l;
    L = l.le;
    bbox = Range3f(l.position, l.position);
    cone = FastConef(l.normal);
}

void OrientedLightTreeNode::Update()
{
    OrientedLightTreeNode *l = static_cast<OrientedLightTreeNode*>(left);
    OrientedLightTreeNode *r = static_cast<OrientedLightTreeNode*>(right);
    bbox = Range3f::Union(l->bbox, r->bbox);
    cone = FastConef::Union(l->cone, r->cone);
    L = left->L + right->L;
    light = (Random01() < (l->L.Average() / L.Average())) ? l->light : l->light;
}

DirectionalLightTreeNode::DirectionalLightTreeNode(DirLight &l, uint32_t idx) : LightTreeNode(idx)
{
    light = &l;
    L = l.le;
    bbox = Range3f(l.normal, l.normal);
}

void DirectionalLightTreeNode::Update()
{
    DirectionalLightTreeNode *l = static_cast<DirectionalLightTreeNode*>(left);
    DirectionalLightTreeNode *r = static_cast<DirectionalLightTreeNode*>(right);
    bbox = Range3f::Union(l->bbox, r->bbox);
    L = left->L + right->L;
    light = (Random01() < (l->L.Average() / L.Average())) ? l->light : l->light;
}

OmniDirLightTreeNode::OmniDirLightTreeNode(OmniDirLight &l, uint32_t idx) : LightTreeNode(idx)
{
    light = &l;
    L = l.intensity;
    bbox = Range3f(l.position, l.position);
}

void OmniDirLightTreeNode::Update()
{
    OmniDirLightTreeNode *l = static_cast<OmniDirLightTreeNode*>(left);
    OmniDirLightTreeNode *r = static_cast<OmniDirLightTreeNode*>(right);
    bbox = Range3f::Union(l->bbox, r->bbox);
    L = left->L + right->L;
    light = (Random01() < (l->L.Average() / L.Average())) ? l->light : l->light;
}

LightTreeNode::~LightTreeNode()
{
    if(left) delete left;
    if(right) delete right;
}

void LightTree::Clear()
{
    if (_orientedRoot)
        delete _orientedRoot; 
    _orientedRoot = NULL;
    if (_directionalRoot)
        delete _directionalRoot;
    _directionalRoot = NULL;
}

struct CutItem
{
    CutItem(LightTreeNode *node, Vec3f ub, Vec3f est = Vec3f::Zero()) : _node(node), _estimate(est), _upperBound(ub)
    {
        _fbound = _upperBound.Average();
    }
    bool operator< (const CutItem &p2) const 
    {
        return _fbound == p2._fbound ? (_node < p2._node) : _fbound < p2._fbound;
    }
    LightTreeNode   *_node;
    Vec3f           _upperBound;
    float           _fbound;
    Vec3f           _estimate;
};

uint32_t LightTree::Lightcut( Image<Vec3f> & matrix, const uint32_t g, const GatherPoint & gp )
{
    Vec3f totalEstL;
    uint32_t cs = 0;
    vector<CutItem> lightcut;
    lightcut.reserve(1024);
    if (_orientedRoot != NULL)
    {
        Vec3f estimate = EvaluateNode(_orientedRoot, gp);
        Vec3f bound = ComputeUpperBound(_orientedRoot, gp);
        totalEstL += _orientedRoot->L * estimate;
        lightcut.push_back(CutItem(_orientedRoot, bound, estimate));
        cs++;
    }

    if (_directionalRoot != NULL)
    {
        Vec3f estimate = EvaluateNode(_directionalRoot, gp);
        Vec3f bound = ComputeUpperBound(_directionalRoot, gp);
        totalEstL += _directionalRoot->L * estimate;
        lightcut.push_back(CutItem(_directionalRoot, bound, estimate));
        cs++;
    }

    while(lightcut.size() > 0)
    {
        CutItem cutItem = lightcut.front();
        float error = cutItem._upperBound.IsZero() ? 0.0f : (cutItem._upperBound / totalEstL).Average();
        if (error <= 0.001f)
            break;

        pop_heap(lightcut.begin(), lightcut.end());
        lightcut.pop_back();

        if (cutItem._node == NULL || cutItem._node->IsLeaf())
        {
            assert(false);
            cout << "error leaf node" << endl;
            continue;
        }

        if (OrientedLightTreeNode *node = dynamic_cast<OrientedLightTreeNode*>(cutItem._node))
        {
            OrientedLightTreeNode *leftNode = node->Left();
            Vec3f leftEst = (node->light == leftNode->light) ? cutItem._estimate : EvaluateNode(leftNode, gp);
            Vec3f lL = leftNode->L * leftEst;

            OrientedLightTreeNode *rightNode = node->Right();
            Vec3f rightEst = (node->light == rightNode->light) ? cutItem._estimate : EvaluateNode(rightNode, gp);
            Vec3f rL = rightNode->L * rightEst;

            totalEstL -= node->L * cutItem._estimate;
            totalEstL += lL + rL;

            if (!leftNode->IsLeaf())
            {
                Vec3f leftErr = ComputeUpperBound(leftNode, gp);
                lightcut.push_back(CutItem(leftNode, leftErr, leftEst));
                push_heap(lightcut.begin(), lightcut.end());
            }
            else
                matrix.ElementAt(leftNode->index, g) = lL;

            if (!rightNode->IsLeaf())
            {
                Vec3f rightErr = ComputeUpperBound(rightNode, gp);
                lightcut.push_back(CutItem(rightNode, rightErr, rightEst));
                push_heap(lightcut.begin(), lightcut.end());
            }
            else
                matrix.ElementAt(rightNode->index, g) = rL;
            cs++;
        }
        else if (DirectionalLightTreeNode *node = dynamic_cast<DirectionalLightTreeNode*>(cutItem._node))
        {
            DirectionalLightTreeNode *leftNode = node->Left();
            Vec3f leftEst = (node->light == leftNode->light) ? cutItem._estimate : EvaluateNode(leftNode, gp);
            Vec3f lL = leftNode->L * leftEst;

            DirectionalLightTreeNode *rightNode = node->Right();
            Vec3f rightEst = (node->light == rightNode->light) ? cutItem._estimate : EvaluateNode(rightNode, gp);
            Vec3f rL = rightNode->L * rightEst;

            totalEstL -= node->L * cutItem._estimate;
            totalEstL += lL + rL;

            if (!leftNode->IsLeaf())
            {
                Vec3f leftErr = ComputeUpperBound(leftNode, gp);
                lightcut.push_back(CutItem(leftNode, leftErr, leftEst));
                push_heap(lightcut.begin(), lightcut.end());
            }
            else
                matrix.ElementAt(leftNode->index, g) = lL;

            if (!rightNode->IsLeaf())
            {
                Vec3f rightErr = ComputeUpperBound(rightNode, gp);
                lightcut.push_back(CutItem(rightNode, rightErr, rightEst));
                push_heap(lightcut.begin(), lightcut.end());
            }
            else
                matrix.ElementAt(rightNode->index, g) = rL;

            cs++;
        }
    }

    for (uint32_t i = 0; i < lightcut.size(); i++)
    {
        CutItem &item = lightcut[i];
        _SetMatrix(matrix, g, item._estimate, item._node);
    }
    return cs;
}

void LightTree::_SetMatrix( Image<Vec3f> & matrix, const uint32_t g, const Vec3f &estimate, LightTreeNode *node )
{
    if (node->IsLeaf())
    {
        matrix.ElementAt(node->index, g) = estimate * node->L;
    }
    else
    {
        _SetMatrix(matrix, g, estimate, node->left);
        _SetMatrix(matrix, g, estimate, node->right);
    }
}

Vec3f LightTree::EvaluateNode(OrientedLightTreeNode * node, const GatherPoint & gp )
{
        OrientedLight& light = *(node->light);
        const DifferentialGeometry &dp = gp.isect.dp;
        const Vec3f &wo = gp.wo;
        Material *ms = gp.isect.m;
    
        Vec3f wi = (light.position - dp.P).GetNormalized();
        float maxDist = (light.position - dp.P).GetLength();
        float cosAngle = max(0.0f, light.normal % (-wi));
        float lenSqrEst = max(_clamp, (light.position - dp.P).GetLengthSqr());
        //float lenSqrEst = (light.position - dp.P).GetLengthSqr();
        BxdfUnion msu;
        ms->SampleReflectance(dp, msu);
        Vec3f brdf = ReflectanceUtils::PosCos(wi, dp);//msu.EvalSmoothCos(wo, wi, dp);
        Vec3f estimate = brdf * cosAngle / lenSqrEst;
    
        if(!estimate.IsZero()) 
        {
            Ray shadowRay(dp.P, wi, gp.isect.rayEpsilon, maxDist, 0.0f);
            if(!_engine->IntersectAny(shadowRay))
                return estimate;
        }
        return Vec3f::Zero();
}

Vec3f LightTree::EvaluateNode( DirectionalLightTreeNode * node, const GatherPoint & gp )
{
    DirLight& light = *(node->light);
    const DifferentialGeometry &dp = gp.isect.dp;
    const Vec3f &wo = gp.wo;
    Material *ms = gp.isect.m;

    Vec3f wi = -light.normal;
    BxdfUnion msu;
    ms->SampleReflectance(dp, msu);
    Vec3f estimate = ReflectanceUtils::PosCos(wi, dp);
    //Vec3f brdf = msu.EvalSmoothCos(wo, wi, dp);
    //Vec3f estimate = brdf;

    if(!estimate.IsZero()) 
    {
        Ray shadowRay(dp.P, wi, gp.isect.rayEpsilon, RAY_INFINITY, 0.0f);
        if(!_engine->IntersectAny(shadowRay))
            return estimate;
    }
    return Vec3f::Zero();
}

Vec3f LightTree::ComputeUpperBound( OrientedLightTreeNode * node, const GatherPoint & gp )
{
    OrientedLight& light = *(node->light);
    const DifferentialGeometry &dp = gp.isect.dp;

    Vec3f wi = (light.position - dp.P).GetNormalized();

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
    return (node->L * BoundMaterial(node->bbox, gp.wo, dp, gp.isect.m, false) * (cosBound / lenSqr)).Abs();
}

Vec3f LightTree::ComputeUpperBound( DirectionalLightTreeNode * node, const GatherPoint & gp )
{
    DirLight& light = *(node->light);
    const DifferentialGeometry &dp = gp.isect.dp;
    Vec3f wi = -light.normal;
    return (node->L * BoundMaterial(node->bbox, gp.wo, dp, gp.isect.m, true)).Abs();
}

Vec3f LightTree::BoundMaterial( const Range3f &bbox, const Vec3f &wo, const DifferentialGeometry & dp, const Material *m, bool dirLight )
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

    return cosTheta;

    //if (!matApprox.Ks.IsZero() && matApprox.n != 0)
    //{
    //    //Phong material
    //    float glossyCosBound = 1.0f;
    //    Vec3f R = ReflectanceUtils::MirrorDirection(dp.N, wo);
    //    if (R == wo || dp.N == wo)
    //    {
    //        R = wo;
    //    }

    //    Vec3f gv = (Vec3f::Z() ^ R).GetNormalized();
    //    Matrix4f gmt = Matrix4f::Rotation(gv, -acosf(Vec3f::Z() % R));

    //    Range3f gbox = Range3f::Empty();
    //    for (int i = 0; i < 8; i++)
    //        gbox.Grow(gmt.TransformPoint(corners[i]));

    //    Vec3f &gm = gbox.GetMinRef();
    //    Vec3f &gM = gbox.GetMaxRef();

    //    if (gM.z > 0)
    //    {
    //        float minx2, miny2;
    //        if (gm.x * gM.x <= 0)
    //            minx2 = 0.0f;
    //        else
    //            minx2 = min(gm.x * gm.x, gM.x * gM.x);

    //        if (gm.y * gM.y <= 0)
    //            miny2 = 0.0f;
    //        else
    //            miny2 = min(gm.y * gm.y, gM.y * gM.y);
    //        float maxz2 = gM.z * gM.z;
    //        glossyCosBound = gM.z / sqrt(minx2 + miny2 + maxz2);
    //    }
    //    else
    //        glossyCosBound = 0.0f;

    //    glossyCosBound = clamp(glossyCosBound, 0.0f, 1.0f);
    //    assert(glossyCosBound <= 1.0f);

    //    return (matApprox.Kd + (matApprox.Ks * pow(glossyCosBound, matApprox.n))) * cosTheta;
    //}

    //return matApprox.Kd * cosTheta;
}

