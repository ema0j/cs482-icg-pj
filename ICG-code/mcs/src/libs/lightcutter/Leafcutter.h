#include <scene/scene.h>
#include <ray/rayEngine.h>
#include <sampler/pathSampler.h>
#include <lighttree/LightTree.h>
#include <misc/report.h>
#include <image/image.h>
#include <pthread/pthread.h>
#include <vmath/streamMethods.h>

#ifndef _LEAF_CUTTER_H_
#define _LEAF_CUTTER_H_

class Leafcutter
{
public:
    Leafcutter(shared_ptr<Scene> scene, shared_ptr<RayEngine> engine, shared_ptr<PathSampler> sampler);
    ~Leafcutter(void);

    virtual void leafcut(shared_ptr<Image<Vec4f> > image, shared_ptr<LightTree > lightTree, uint32_t samples, shared_ptr<ReportHandler> report = shared_ptr<ReportHandler>());
    virtual void leafcut(shared_ptr<Image<Vec4f> > image, shared_ptr<LightTree > lightTree, DifferentialGeometry & dp, Vec3f wo, Material *m, uint32_t line);
protected:

    static void* TreeWalkThread(void *ptr);
    Vec3f EvalutateLight(const OrientedLight& light, DifferentialGeometry& dp, const Vec3f &wo, Material *ms);
    Vec3f EvalutateLight(const DirLight& light, DifferentialGeometry& dp, const Vec3f &wo, Material *ms);
    void TreeWalk();
    void EvaluateRow( Ray &ray, uint32_t j );
    template<typename T>
    void EvaluateTree( T *node, DifferentialGeometry & dp, Vec3f wo, Material *m, uint32_t &idx, uint32_t line );
    template<typename T>
    void EvaluateTree( shared_ptr<Image<Vec4f> > image, shared_ptr<LightTree > lightTree, T *node, DifferentialGeometry & dp, Vec3f wo, Material *m, uint32_t &idx, uint32_t line );
    shared_ptr<PathSampler> _sampler;
    shared_ptr<Scene>       _scene;
    shared_ptr<RayEngine>   _engine;
    shared_ptr<ReportHandler> _report;
    shared_ptr<LightTree >   _lightTree;
    shared_ptr<Image<Vec4f> > _image;
    uint32_t            _curLine;
    pthread_mutex_t         _mutex;

    uint32_t            _nCore;
};

template<typename T>
void Leafcutter::EvaluateTree( T *node, DifferentialGeometry & dp, Vec3f wo, Material *m, uint32_t &idx, uint32_t line )
{
    if (node == NULL)
        return;

    if (node->IsLeaf())
    {
        Vec3f L = EvalutateLight(*(node->light), dp, wo, m);
        _image->ElementAt(idx, line) = Vec4f(L, 1.0f);
        idx++;
    }
    else
    {
        if (node->left)
            EvaluateTree(node->Left(), dp, wo, m, idx, line);
        if (node->right)
            EvaluateTree(node->Right(), dp, wo, m, idx, line);
    }
}

template<typename T>
void Leafcutter::EvaluateTree( shared_ptr<Image<Vec4f> > image, shared_ptr<LightTree > lightTree, T *node, DifferentialGeometry & dp, Vec3f wo, Material *m, uint32_t &idx, uint32_t line )
{
    if (node == NULL)
        return;

    if (node->IsLeaf())
    {
        Vec3f L = EvalutateLight(*(node->light), dp, wo, m);
        image->ElementAt(idx, line) = Vec4f(L, 1.0f);
        idx++;
    }
    else
    {
        if (node->left)
            EvaluateTree(image, lightTree, node->Left(), dp, wo, m, idx, line);
        if (node->right)
            EvaluateTree(image, lightTree, node->Right(), dp, wo, m, idx, line);
    }
}

#endif // _LEAF_CUTTER_H_

