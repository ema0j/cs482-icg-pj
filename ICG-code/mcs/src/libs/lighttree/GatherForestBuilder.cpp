#include "GatherForestBuilder.h"
#include <tracer/pathSampler.h>
#include <scene/camera.h>
#include <vector>

using std::vector;


float GatherForestBuilder::__rand()
{
    static random_std_engine eng;
    return uniform1D01<float>(eng);
}

GatherForestBuilder::GatherForestBuilder(void)
{
}

GatherForestBuilder::~GatherForestBuilder(void)
{
}

shared_ptr<Image<shared_ptr<GatherTree> > > GatherForestBuilder::Build( shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, uint32_t width, uint32_t height, uint32_t samples, carray<float> &timeInstantces, shared_ptr<ReportHandler> report /*= shared_ptr<ReportHandler>()*/ )
{
    shared_ptr<Image<shared_ptr<GatherTree> > > forest = 
        shared_ptr<Image<shared_ptr<GatherTree> > >(new Image<shared_ptr<GatherTree> >(width, height));
    StratifiedPathSamplerMT sampler;

    Vec2f pixelSize(1.0f/width, 1.0f/height);

    _tsample = timeInstantces.size();

    if(report) report->beginActivity("Build Gather Forest");

    for(uint32_t j = 0; j < height; j ++) 
    {
        for(uint32_t i = 0; i < width; i ++) 
        {
            vector<GatherPoint> points;
            sampler.BeginPixel(_tsample);
            for (uint32_t s = 0; s < samples; s++)
            {
                Vec2f puv = (Vec2f(Vec2i(i,j)) + sampler.Pixel()) * pixelSize;
                uint32_t ts = clamp<uint32_t>(static_cast<uint32_t>(sampler.Time() * _tsample), 0, _tsample - 1);
                Ray ray = scene->MainCamera()->GenerateRay(puv, sampler.Lens(), timeInstantces[ts]);

                Intersection isect;
                if(engine->Intersect(ray, &isect))
                {
                    GatherPoint point;
                    point.position = isect.dp.P;
                    point.normal = isect.dp.N;
                    point.ms = isect.m;
                    point.timeInst = s;
                    points.push_back(point);
                }
                sampler.NextPixelSample();
            }
            sampler.EndPixel();
            shared_ptr<GatherTree> tree = _BuildTree(points);
            if(report) report->progress((float)j / (float)(height));
        }
    }
    if(report) report->endActivity();
    return forest;
}

shared_ptr<GatherTree> GatherForestBuilder::_BuildTree( vector<GatherPoint> &points )
{
    shared_ptr<GatherTree> tree = shared_ptr<GatherTree>(new GatherTree());
    tree->_gps.reserve(points.size());
    tree->_tree.reserve(2 * points.size());
    _SplitPoints(tree->_tree, tree->_gps, points.begin(), points.end());
    return tree;
}

template<class NodeData> 
struct CompareNode {
    CompareNode(bool t, uint32_t d) { type = t; dim = d; }
    bool type;
    uint32_t dim;

    bool operator()(const NodeData &d1, const NodeData &d2) const 
    {
        if (type)
            return d1.position[dim] == d2.position[dim] ? (&d1 < &d2) :
            d1.position[dim] < d2.position[dim];
        else
            return d1.normal[dim] == d2.normal[dim] ? (&d1 < &d2) :
            d1.normal[dim] < d2.normal[dim];
    }
};

uint32_t GatherForestBuilder::_SplitPoints(vector<GatherNode> &tree, vector<GatherPoint> &gps, vector<GatherPoint>::iterator start, vector<GatherPoint>::iterator end )
{
    Range3f pBox(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    Range3f nBox(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    Conef   cone;

    vector<GatherPoint>::iterator it = start;
    while (it != end)
    {
        pBox.Grow(it->position);
        nBox.Grow(it->normal);
        if (it == start)
            cone = Conef(it->normal);
        else
            cone.Grow(it->normal);
        it++;
    }

    GatherNode node(_tsample);
    node.bbox = pBox;
    node.cone = cone;
    tree.push_back(node);
    uint32_t curIdx = tree.size() - 1;

#ifdef _DEBUG
    vector<GatherPoint> gp(start, end);
#endif // _DEBUG
    if (end - start > 2)
    {
        Vec3f nbSize = nBox.GetSize();
        Vec3f pbSize = pBox.GetSize();

        bool splitType;
        uint32_t splitDim;
        if (pbSize.MaxComponent() > nbSize.MaxComponent())
        {
            splitType = true;
            splitDim = pbSize.MaxComponentIndex();
        }
        else
        {
            splitType = false;
            splitDim = nbSize.MaxComponentIndex();
        }
        vector<GatherPoint>::iterator splitPos = start + (end - start) / 2;
        std::nth_element(start, splitPos, end, CompareNode<GatherPoint>(splitType, splitDim));
#ifdef _DEBUG
        vector<GatherPoint> gp1(start, splitPos);
        vector<GatherPoint> gp2(splitPos, end);
#endif // _DEBUG

        uint32_t leftIdx  = _SplitPoints(tree, gps, start, splitPos);
        uint32_t rightIdx = _SplitPoints(tree, gps, splitPos, end);

        GatherNode &curNode = tree[curIdx];
        curNode.leftIdx = leftIdx;
        curNode.rightIdx = rightIdx;
        vector<uint32_t> &reps = curNode.reps;
        vector<uint32_t> &lreps = tree[leftIdx].reps;
        vector<uint32_t> &rreps = tree[rightIdx].reps;
        
        
        for (uint32_t i = 0; i < reps.size(); i++)
        {
            if (lreps[i] != -1 && rreps[i] != -1)
            {
                //GatherPoint &p1 = gps[lreps[i]];
                //GatherPoint &p2 = gps[reps[i]];
                reps[i] = (__rand() > 0.5f) ? lreps[i] : rreps[i];
            }
            else
            {
                reps[i] = lreps[i] + 1 + rreps[i];
            }
        }
    }
    else
    {
        vector<GatherPoint>::iterator it = start;
        while (it != end)
        {
            gps.push_back(*it);
            tree[curIdx].reps[it->timeInst] = gps.size() - 1;
            it++;
        }
    }
    
    return curIdx;
}
