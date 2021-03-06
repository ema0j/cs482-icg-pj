#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <vmath/vec6.h>
#include "vmath/range6.h"

typedef uint32_t u_int;

// compressed kdtree from pbrt
struct KdNode {
	void init(float p, u_int a) {
		splitPos = p;
		splitAxis = a;
		rightChild = ~((u_int)0);
		hasLeftChild = 0;
	}
	void initLeaf() {
		splitAxis = 6;
		rightChild = ~((u_int)0);
		hasLeftChild = 0;
	}
	// KdNode Data
	float splitPos;
	u_int splitAxis;
	u_int hasLeftChild;
	u_int rightChild;
};

template <class NodeData> 
class KdTree {
public:
	// KdTree Public Methods
	KdTree(const vector<NodeData> &data);
	~KdTree() {
		delete[] nodes;
		delete[] nodeData;
	}
	void recursiveBuild(u_int nodeNum, typename vector<const NodeData*>::iterator start, 
		typename vector<const NodeData *>::iterator end);

    template<typename LookupProc>
	void Lookup(const Vec6f &p, const LookupProc &process,
			float &maxDistSquared) const;
	inline const NodeData* GetNodeDataPtr() const { return nodeData; }
	inline uint32_t NodeDataSize() const { return nNodes; }
    inline Range3f GetBoundingBox() const { return cacheBBox; }
private:
	// KdTree Private Methods
    template<typename LookupProc>
	void privateLookup(u_int nodeNum, const Vec6f &p,
		const LookupProc &process, float &maxDistSquared) const;
	// KdTree Private Data
	KdNode *nodes;
	NodeData *nodeData;
	u_int nNodes, nextFreeNode;
    Range6f cacheBBox;
};

template<class NodeData> 
struct CompareNode {
	CompareNode(int a) { axis = a; }
	int axis;
	bool operator()(const NodeData *d1,
			const NodeData *d2) const {
		return d1->p[axis] == d2->p[axis] ? (d1 < d2) :
			d1->p[axis] < d2->p[axis];
	}
};

// KdTree Method Definitions
template <typename NodeData>
KdTree<NodeData>::KdTree(const vector<NodeData> &d) {
	nNodes = (uint32_t)d.size();
	nextFreeNode = 1;
	nodes = new KdNode[nNodes];
	nodeData = new NodeData[nNodes];
	vector<const NodeData *> buildNodes;
    cacheBBox = Range6f::Empty();
	for (u_int i = 0; i < nNodes; ++i)
    {
		buildNodes.push_back(&d[i]);
        cacheBBox.Grow(d[i].p);
    }
	// Begin the KdTree building process
	recursiveBuild(0, buildNodes.begin(), buildNodes.end());
}

template <class NodeData> 
void KdTree<NodeData>::recursiveBuild(u_int nodeNum,
		typename vector<const NodeData *>::iterator start, 
		typename vector<const NodeData *>::iterator end) 
{
	// Create leaf node of kd-tree if we've reached the bottom
	if (start + 1 == end) {
		nodes[nodeNum].initLeaf();
		nodeData[nodeNum] = **start;
		return;
	}
	// Choose split direction and partition data
	// Compute bounds of data from _start_ to _end_
	Range6f bbox;
    for (typename vector<const NodeData *>::iterator i = start; i < end; ++i) 
		bbox.Grow((*i)->p);

    int splitAxis = bbox.GetSize().MaxComponentIndex();
	typename vector<const NodeData *>::iterator splitPos = start + (end - start)/2;
	std::nth_element(start, splitPos,
		end, CompareNode<NodeData>(splitAxis));
	// Allocate kd-tree node and continue recursively
	nodes[nodeNum].init((*splitPos)->p[splitAxis],
		splitAxis);
	nodeData[nodeNum] = *(*splitPos);
	if (start < splitPos) {
		nodes[nodeNum].hasLeftChild = 1;
		u_int childNum = nextFreeNode++;
		recursiveBuild(childNum, start, splitPos);
	}
	if (splitPos+1 < end) {
		nodes[nodeNum].rightChild = nextFreeNode++;
		recursiveBuild(nodes[nodeNum].rightChild, splitPos+1, end);
	}
}

template <typename NodeData>
template <typename LookupProc>
void KdTree<NodeData>::Lookup(const Vec6f &p,
		const LookupProc &proc,
		float &maxDistSquared) const {
	privateLookup(0, p, proc, maxDistSquared);
}

template <typename NodeData>
template <typename LookupProc>
void KdTree<NodeData>::privateLookup(u_int nodeNum,
		const Vec6f &p,	const LookupProc &process,
		float &maxDistSquared) const {
	KdNode *node = &nodes[nodeNum];
	// Process kd-tree node's children
	int axis = node->splitAxis;
	if (axis != 6) {
		float dist2 = (p[axis] - node->splitPos) *
			(p[axis] - node->splitPos);
		if (p[axis] <= node->splitPos) {
			if (node->hasLeftChild)
				privateLookup(nodeNum+1, p,
				              process, maxDistSquared);
			if (dist2 < maxDistSquared &&
			    node->rightChild < nNodes)
				privateLookup(node->rightChild,
				              p,
							  process,
							  maxDistSquared);
		}
		else {
			if (node->rightChild < nNodes)
				privateLookup(node->rightChild,
				              p,
							  process,
							  maxDistSquared);
			if (dist2 < maxDistSquared && node->hasLeftChild)
				privateLookup(nodeNum+1,
				              p,
							  process,
							  maxDistSquared);
		}
	}
	// Hand kd-tree node to processing function
	float dist2 = (nodeData[nodeNum].p - p).GetLengthSqr();
	if (dist2 < maxDistSquared)
		process(nodeData[nodeNum], dist2, maxDistSquared);
}

#endif // PBRT_KDTREE_H
