template<typename NodeType>
BuilderKdTree<NodeType>::BuilderKdTree(vector<BuildDataType> &inputs, float c, uint32_t ts) : _c(c), _ts(ts)
{
    _root = _Build(inputs.begin(), inputs.end());
    _size = inputs.size();
    _anyNode = inputs[0].node;
}

struct CompareOp
{
    CompareOp(uint32_t d) : dim(d) {}
    template<typename NodeType>
    bool operator()(const BuilderKdTreeData<NodeType> &d1, const BuilderKdTreeData<NodeType> &d2) const 
    {
        return d1.point[dim] == d2.point[dim] ? (d1.node < d2.node) : d1.point[dim] < d2.point[dim];
    }
    uint32_t dim;
};

template<typename NodeType>
BuilderKdNode<NodeType>* BuilderKdTree<NodeType>::_Build(BuildDataIter start, BuildDataIter end) 
{
    if (end - start <= BUILDER_KD_BUCKET_SIZE)
    {
        return _MakeLeafNode(start, end);
    }
    Range6f box = Range6f::Empty();
    for (BuildDataIter it = start; it != end; it++)
        box.Grow(it->point);
    //#ifdef _DEBUG
    //        vector<BuilderKdTreeData> whole(start, end);
    //#endif // _DEBUG
    uint32_t splitDim = box.GetSize().MaxComponentIndex();
    typename vector<BuildDataType>::iterator mid = start + (end - start) / 2;
    std::nth_element(start, mid, end, CompareOp(splitDim));
    //#ifdef _DEBUG
    //        vector<BuilderKdTreeData> left(start, mid);
    //        vector<BuilderKdTreeData> right(mid, end);
    //#endif // _DEBUG
    BuilderKdBranchNode<NodeType> *node = new BuilderKdBranchNode<NodeType>();
    node->splitAxis = splitDim;
    node->data = *mid;
    node->left = _Build(start, mid);
    node->right = _Build(mid + 1, end);
    return node;
}

template<typename NodeType>
BuilderKdNode<NodeType>* BuilderKdTree<NodeType>::_MakeLeafNode(BuildDataIter start, BuildDataIter end) 
{
    BuilderKdLeafNode<NodeType> *leaf = new BuilderKdLeafNode<NodeType>();
    for (BuildDataIter it = start; it != end; it++)
        leaf->dataBucket.push_back(*it);
    return leaf;
}

template<typename NodeType>
NodeType* BuilderKdTree<NodeType>::GetAnyElement() 
{
    assert(_anyNode);
    return _anyNode;
}

template<typename NodeType>
NodeType* BuilderKdTree<NodeType>::FindBestMatch( OrientedLightTreeNode<RepData> * node ) 
{
    Vec6f point = LightTreeUtil::GetApproxPoint(node, _c);
    float maxDist = FLT_MAX;
    return _LoopUp(_root, point, node, maxDist);
}

template<typename NodeType>
NodeType* BuilderKdTree<NodeType>::_LoopUp( BuilderKdNode<NodeType>* treenode, Vec6f point, OrientedLightTreeNode<RepData> * n, float &maxDist)
{
    OrientedLightTreeNode *bestMatch;
    if (treenode->IsLeaf())
    {
        vector<BuildDataType> &items = 
            static_cast<BuilderKdLeafNode<NodeType>*>(treenode)->dataBucket;
        for (uint32_t i = 0; i < items.size(); i++)
        {
            if (items[i].node == n)
                continue;
            float dist = LightTreeUtil::Distance(items[i].node, n, _c);
            if (maxDist >= dist)
            {
                maxDist = dist;
                bestMatch = items[i].node;
            }
        }
    }
    else
    {
        BuildDataType &item = 
            static_cast<BuilderKdBranchNode<NodeType>*>(treenode)->data;
        item.point;
    }
    return bestMatch;
}