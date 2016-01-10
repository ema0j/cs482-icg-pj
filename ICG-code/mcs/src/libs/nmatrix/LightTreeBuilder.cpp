#include "LightTreeBuilder.h"

Vec6f LightTreeBuilder::ComputePoint( VLight *light, VL_TYPE type, float normScale )
{
    switch(type)
    {
    case OMNIDIR_LIGHT:
        {
            OmniDirLight* l = reinterpret_cast<OmniDirLight*>(light);
            return Vec6f(l->position, Vec3f::Zero());
        }
    case DIRECTIONAL_LIGHT:
        {
            DirLight* l = reinterpret_cast<DirLight*>(light);
            return Vec6f(l->normal, Vec3f::Zero());
        }
    case ORIENTED_LIGHT:
        {
            OrientedLight* l = reinterpret_cast<OrientedLight*>(light);
            return Vec6f(l->position, l->normal * normScale);
        }
    }
    return Vec6f();
}

OrientedLightTreeNode* LightTreeBuilder::_BuildOrtLight( vector<NodePoint>::iterator start, vector<NodePoint>::iterator end, LightList &lightList) 
{
    if (end - start == 1)
    {
        OrientedLightTreeNode *node = new OrientedLightTreeNode(*reinterpret_cast<OrientedLight*>(lightList.GetLight(start->idx)), start->idx);
        return node;
    }

    Range6f bbox;
    for (vector<NodePoint>::iterator it = start; it != end; it++)
        bbox.Grow(it->point);

    uint32_t dim = bbox.GetSize().MaxComponentIndex();
    vector<NodePoint>::iterator mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, [dim](const NodePoint &d1, const NodePoint &d2) { return d1.point[dim] == d2.point[dim] ? (d1.idx < d2.idx) : d1.point[dim] < d2.point[dim]; });

    OrientedLightTreeNode *node = new OrientedLightTreeNode();
    node->left = _BuildOrtLight(start, mid, lightList);
    node->right = _BuildOrtLight(mid, end, lightList);
    node->Update();
    return node;
}

DirectionalLightTreeNode* LightTreeBuilder::_BuildDirLight( vector<NodePoint>::iterator start, vector<NodePoint>::iterator end, LightList &lightList) 
{
    if (end - start == 1)
    {
        DirectionalLightTreeNode *node = new DirectionalLightTreeNode(*reinterpret_cast<DirLight*>(lightList.GetLight(start->idx)), start->idx);
        return node;
    }

    Range6f bbox;
    for (vector<NodePoint>::iterator it = start; it != end; it++)
        bbox.Grow(it->point);

    uint32_t dim = bbox.GetSize().MaxComponentIndex();
    vector<NodePoint>::iterator mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, [dim](const NodePoint &d1, const NodePoint &d2) { return d1.point[dim] == d2.point[dim] ? (d1.idx < d2.idx) : d1.point[dim] < d2.point[dim]; });

    DirectionalLightTreeNode *node = new DirectionalLightTreeNode();
    node->left = _BuildDirLight(start, mid, lightList);
    node->right = _BuildDirLight(mid, end, lightList);
    node->Update();
    return node;
}

shared_ptr<LightTree> LightTreeBuilder::Build(LightList &lightList, shared_ptr<RayEngine> engine, float normScale, float clamp)
{
    shared_ptr<LightTree> tree = shared_ptr<LightTree>(new LightTree(lightList, engine, clamp));
    vector<NodePoint> points;
    Range1u r = lightList.GetRange(ORIENTED_LIGHT);
    for (uint32_t i = r.GetMin(); i < r.GetMax(); i++)
    {
        NodePoint point;
        point.idx = i;
        point.point = ComputePoint(lightList.GetLight(i), lightList.GetLightType(i), normScale);
        points.push_back(point);
    }
    if (points.size() > 0)
        tree->_orientedRoot = _BuildOrtLight(points.begin(), points.end(), lightList);

    points.clear();
    r = lightList.GetRange(DIRECTIONAL_LIGHT);
    for (uint32_t i = r.GetMin(); i < r.GetMax(); i++) 
    {
        NodePoint point;
        point.idx = i;
        point.point = ComputePoint(lightList.GetLight(i), lightList.GetLightType(i), normScale);
        points.push_back(point);
    }
    if (points.size() > 0)
        tree->_directionalRoot = _BuildDirLight(points.begin(), points.end(), lightList);

    return tree;
}