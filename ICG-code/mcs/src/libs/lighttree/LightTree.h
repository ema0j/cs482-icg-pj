#ifndef _LIGHT_TREE_H_
#define _LIGHT_TREE_H_

#include <vector>
#include <vmath/fastcone.h>
#include <vmath/range3.h>
#include <vmath/vec6.h>
#include <lightgen/LightData.h>
#include <vmath/random.h>
//#define MULTI_REP

using std::vector;

enum LightTreeNodeType
{
    ORIENTED_NODE,
    DIRECT_NODE,
    POINT_NODE
};

struct LightTreeNode
{
	LightTreeNode() {}
	virtual ~LightTreeNode() {}
    Vec3f           L;
	virtual bool	IsLeaf() const = 0;
    virtual         LightTreeNodeType GetNodeType() const = 0;
};

struct OrientedLightTreeNode : public LightTreeNode
{
	typedef OrientedLight lighttype;
    OrientedLightTreeNode() : LightTreeNode(), light(NULL), left(0), right(0) {}
    OrientedLightTreeNode(OrientedLight &light);
	virtual ~OrientedLightTreeNode();
	virtual LightTreeNodeType           GetNodeType() const { return ORIENTED_NODE; }
	bool								IsLeaf() const { return left == NULL && right == NULL; }
	FastConef		                    cone;
	Range3f			                    bbox;
	OrientedLight                       *light;
	OrientedLightTreeNode				*left;
	OrientedLightTreeNode				*right;
};

struct DirectionalLightTreeNode : public LightTreeNode
{
	typedef DirLight lighttype;
    DirectionalLightTreeNode() : LightTreeNode(), light(NULL), left(0), right(0) {}
    DirectionalLightTreeNode(DirLight &light);
	virtual ~DirectionalLightTreeNode();
    virtual LightTreeNodeType           GetNodeType() const { return DIRECT_NODE; }
	bool								IsLeaf() const { return left == NULL && right == NULL; }
    Range3f			                    bbox;
    DirLight                            *light;
	DirectionalLightTreeNode			*left;
	DirectionalLightTreeNode			*right;
};

struct OmniDirLightTreeNode : public LightTreeNode
{
	typedef OmniDirLight lighttype;
    OmniDirLightTreeNode() : LightTreeNode(), left(0), right(0) {}
    OmniDirLightTreeNode(OmniDirLight &light);
	virtual ~OmniDirLightTreeNode();
    virtual LightTreeNodeType           GetNodeType() const { return POINT_NODE; }
	bool								IsLeaf() const { return left == NULL && right == NULL; }
    Range3f			                    bbox;
    OmniDirLight                        *light;
	OmniDirLightTreeNode				*left;
	OmniDirLightTreeNode				*right;
};

#ifdef MULTI_REP
const unsigned int MD_REP_SLOTS = 16;
struct MdOrientedLightTreeNode : public LightTreeNode
{
	typedef OrientedLight lighttype;
	MdOrientedLightTreeNode() : LightTreeNode(), left(0), right(0) { memset(lights, 0, sizeof(lights));}
	MdOrientedLightTreeNode(OrientedLight &light, uint32_t idx);
	virtual ~MdOrientedLightTreeNode();
	virtual LightTreeNodeType           GetNodeType() const { return ORIENTED_NODE; }
	bool								IsLeaf() const { return left == NULL && right == NULL; }
	FastConef		                    cone;
	Range3f			                    bbox;
	OrientedLight                       *lights[MD_REP_SLOTS];
	MdOrientedLightTreeNode				*left;
	MdOrientedLightTreeNode				*right;
};

struct MdDirectionalLightTreeNode : public LightTreeNode
{
	typedef DirLight lighttype;
	MdDirectionalLightTreeNode() : LightTreeNode(), left(0), right(0) { memset(lights, 0, sizeof(lights)); }
	MdDirectionalLightTreeNode(DirLight &light, uint32_t idx);
	virtual ~MdDirectionalLightTreeNode();
	virtual LightTreeNodeType           GetNodeType() const { return DIRECT_NODE; }
	bool								IsLeaf() const { return left == NULL && right == NULL; }
	Range3f			                    bbox;
	DirLight                            *lights[MD_REP_SLOTS];
	MdDirectionalLightTreeNode			*left;
	MdDirectionalLightTreeNode			*right;
};

struct MdOmniDirLightTreeNode : public LightTreeNode
{
	typedef OmniDirLight lighttype;
	MdOmniDirLightTreeNode() : LightTreeNode(), left(0), right(0) { memset(lights, 0, sizeof(lights)); }
	MdOmniDirLightTreeNode(OmniDirLight &light, uint32_t idx);
	virtual ~MdOmniDirLightTreeNode();
	virtual LightTreeNodeType           GetNodeType() const { return POINT_NODE; }
	bool								IsLeaf() const { return left == NULL && right == NULL; }
	Range3f			                    bbox;
	OmniDirLight                        *lights[MD_REP_SLOTS];
	MdOmniDirLightTreeNode				*left;
	MdOmniDirLightTreeNode				*right;
};
#else
	typedef OrientedLightTreeNode MdOrientedLightTreeNode;
	typedef DirectionalLightTreeNode MdDirectionalLightTreeNode;
	typedef OmniDirLightTreeNode MdOmniDirLightTreeNode;
#endif

template<typename OT, typename DT, typename PT>
class GenericLightTree
{
    friend class DivisiveMdLightTreeBuilder;
    friend class DivisiveLightTreeBuilder;
public:
    GenericLightTree(void) {}
	~GenericLightTree(void) { Clear(); }
	inline OT				*GetOrientedRoot() const { return _orientedRoot; }
	inline DT				*GetDirectionalRoot() const { return _directionalRoot; }

    inline uint32_t         OrientedLightNum() const { return static_cast<uint32_t>(_orientedLights.size()); }
    inline uint32_t         DirectionalLightNum() const { return static_cast<uint32_t>(_dirLights.size()); }
    inline uint32_t         OmniDirLightNum() const { return static_cast<uint32_t>(_omniDirLights.size()); }

    vector<OmniDirLight>&   OmniDirLights() { return _omniDirLights; }
    vector<DirLight>&       DirectionalLights() { return _dirLights; }
    vector<OrientedLight>&  OrientedLights() { return _orientedLights; }

    void                    Clear() { if (_orientedRoot) delete _orientedRoot; if (_directionalRoot) delete _directionalRoot;}

protected:
    vector<OmniDirLight>            _omniDirLights;
    vector<DirLight>                _dirLights;
    vector<OrientedLight>           _orientedLights;

    OT								*_orientedRoot;
    DT								*_directionalRoot;
	PT								*_pointRoot;
};


typedef GenericLightTree<OrientedLightTreeNode, DirectionalLightTreeNode, OmniDirLightTreeNode> LightTree;
#ifdef MULTI_REP
typedef GenericLightTree<MdOrientedLightTreeNode, MdDirectionalLightTreeNode, MdOmniDirLightTreeNode> MdLightTree;
#else
typedef LightTree MdLightTree;
#endif // MULTI_REP

#endif // _LIGHT_TREE_H_
