#ifndef _BUILDER_KD_TREE_H_
#define _BUILDER_KD_TREE_H_
#include <vmath/vec6.h>
#include <vmath/range6.h>
#include "LightTree.h"
#include <stdexcept>

#define BUILDER_KD_BUCKET_SIZE 16
template<typename NodeType>
struct BuilderKdTreeData
{
    BuilderKdTreeData() : node(NULL) {}
    BuilderKdTreeData(NodeType *n, float c)
    {
        assert(n);
        node = n;
        point = LightTreeUtil::GetApproxPoint(n, c);
    }
    NodeType                *node;
    Vec6f                   point;
};

template<typename NodeType>
struct BuilderKdNode
{
    BuilderKdNode() {}
    virtual ~BuilderKdNode() { }
    virtual bool IsLeaf() = 0;
};

template<typename NodeType>
struct BuilderKdBranchNode : public BuilderKdNode<NodeType>
{
    BuilderKdBranchNode() : BuilderKdNode(), left(NULL), right(NULL), splitAxis(0) {}
    virtual ~BuilderKdBranchNode() { delete left; delete right; }
    virtual bool IsLeaf() { return false; }
    BuilderKdTreeData<NodeType>     data;
    uint32_t                    splitAxis;
    BuilderKdNode                   *left;
    BuilderKdNode                   *right;
};

template<typename NodeType>
struct BuilderKdLeafNode : public BuilderKdNode<NodeType>
{
    BuilderKdLeafNode() : BuilderKdNode(){}
    virtual ~BuilderKdLeafNode() {}
    virtual bool IsLeaf() { return true; }
    virtual vector<BuilderKdTreeData<NodeType> > &GetBucket() { return dataBucket; }
    vector<BuilderKdTreeData<NodeType> >          dataBucket;
};


template<typename NodeType>
class BuilderKdTree
{
    typedef BuilderKdTreeData<NodeType> BuildDataType;
    typedef typename vector<BuildDataType>::iterator BuildDataIter;
public:
    BuilderKdTree(vector<BuildDataType > &inputs, float c, uint32_t ts);
    ~BuilderKdTree() { delete _root; }
    inline uint32_t Size() const { return _size; }
    NodeType* GetAnyElement() ;
    NodeType* FindBestMatch( OrientedLightTreeNode<RepData> * A );
protected:
    BuilderKdNode<NodeType>* _Build(BuildDataIter start, BuildDataIter end);
    BuilderKdNode<NodeType>* _MakeLeafNode(BuildDataIter start, BuildDataIter end);
    NodeType* _LoopUp( BuilderKdNode<NodeType>* treenode, Vec6f point, OrientedLightTreeNode<RepData> * n, float &maxDist);
private:
    BuilderKdNode<NodeType>*    _root;
    uint32_t                _size;
    NodeType*                   _anyNode;
    float                       _c;
    uint32_t                _ts;
};


#include "BUilderKdTreeImpl.h"
#endif // _BUILDER_KD_TREE_H_
