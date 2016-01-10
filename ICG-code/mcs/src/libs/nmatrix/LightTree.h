#ifndef _SMALL_LIGHT_TREE_H_
#define _SMALL_LIGHT_TREE_H_

#include <vector>
#include <vmath/fastcone.h>
#include <vmath/range3.h>
#include <vmath/vec6.h>
#include <lightgen/LightData.h>
#include <vmath/random.h>
#include "ray/rayEngine.h"
#include "gpshoot/GatherPointShooter.h"

enum LightTreeNodeType
{
    ORIENTED_NODE,
    DIRECT_NODE,
    POINT_NODE
};

struct LightTreeNode
{
    LightTreeNode() : left(NULL), right(NULL), index(-1) {}
    LightTreeNode(uint32_t idx) : left(NULL), right(NULL), index(idx) {}
    virtual ~LightTreeNode();
    Vec3f           L;
    virtual         LightTreeNodeType GetNodeType() = 0;
    inline bool     IsLeaf() const { return left == NULL && right == NULL; }
    float           Random01() const { static random_std_engine engine; return uniform1D01<float>(engine); }
    uint32_t        index;
    LightTreeNode   *left;
    LightTreeNode   *right;
};

class LightTree;

struct OrientedLightTreeNode : public LightTreeNode
{
    OrientedLightTreeNode() : LightTreeNode(), light(NULL) {}
    OrientedLightTreeNode(OrientedLight &light, uint32_t idx);
    virtual LightTreeNodeType           GetNodeType() { return ORIENTED_NODE; }
    OrientedLight&                      GetLight(LightTree *lightTree);
    void                                Update();
    inline OrientedLightTreeNode*       Left() const { return static_cast<OrientedLightTreeNode*>(left); }
    inline OrientedLightTreeNode*       Right() const { return static_cast<OrientedLightTreeNode*>(right); }
    FastConef		                    cone;
    Range3f			                    bbox;
    OrientedLight                       *light;
};

struct DirectionalLightTreeNode : public LightTreeNode
{
    DirectionalLightTreeNode() : LightTreeNode(), light(NULL) {}
    DirectionalLightTreeNode(DirLight &light, uint32_t idx);
    virtual LightTreeNodeType           GetNodeType() { return DIRECT_NODE; }
    DirLight &                          GetLight(LightTree *lightTree);
    void                                Update();
    inline DirectionalLightTreeNode*    Left() const { return static_cast<DirectionalLightTreeNode*>(left); }
    inline DirectionalLightTreeNode*    Right() const { return static_cast<DirectionalLightTreeNode*>(right); }
    Range3f			                    bbox;
    DirLight                            *light;
};

struct OmniDirLightTreeNode : public LightTreeNode
{
    OmniDirLightTreeNode() : LightTreeNode() {}
    OmniDirLightTreeNode(OmniDirLight &light, uint32_t idx);
    virtual LightTreeNodeType           GetNodeType() { return POINT_NODE; }
    OmniDirLight &                      GetLight(LightTree *lightTree);
    void                                Update();
    inline OmniDirLightTreeNode*        Left() const { return static_cast<OmniDirLightTreeNode*>(left); }
    inline OmniDirLightTreeNode*        Right() const { return static_cast<OmniDirLightTreeNode*>(right); }
    Range3f			                    bbox;
    OmniDirLight                        *light;
};


class LightTree
{
    friend class LightTreeBuilder;
public:
    LightTree(LightList &lightList, shared_ptr<RayEngine> engine, float clamp) 
        : _lightList(lightList), _engine(engine), _clamp(clamp), _orientedRoot(NULL), _directionalRoot(NULL) {}
    ~LightTree(void) { Clear(); }
    void Clear();
    uint32_t Lightcut( Image<Vec3f> & matrix, const uint32_t g, const GatherPoint & gp );
    Vec3f EvaluateNode( OrientedLightTreeNode * node, const GatherPoint & gp );
    Vec3f ComputeUpperBound( OrientedLightTreeNode * node, const GatherPoint & gp );
    Vec3f EvaluateNode( DirectionalLightTreeNode * node, const GatherPoint & gp );
    Vec3f ComputeUpperBound( DirectionalLightTreeNode * node, const GatherPoint & gp );
    Vec3f BoundMaterial( const Range3f &bbox, const Vec3f &wo, const DifferentialGeometry & dp, const Material *m, bool dirLight );
    void _SetMatrix( Image<Vec3f> & matrix, const uint32_t g, const Vec3f &estimate, LightTreeNode *node );
protected:
    LightList                       &_lightList;
    shared_ptr<RayEngine>           _engine;
    float                           _clamp;
    OrientedLightTreeNode           *_orientedRoot;
    DirectionalLightTreeNode        *_directionalRoot;
};

#endif // _SMALL_LIGHT_TREE_H_
