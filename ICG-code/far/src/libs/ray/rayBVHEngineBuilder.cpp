#include "rayBVHEngineBuilder.h"
#include <scene/shape_sphere.h>
#include <scene/shape_curve.h>
#include "rayListEngine.h"

RayEngine* RayBVHEngineBuilder::Build(const vector<shared_ptr<Surface> >& surfaces, 
    const vector<shared_ptr<InstanceGroup> >& instances, 
    const Intervalf& time, int timeSamples)
{
    shared_ptr<GroupBvh> bvh = shared_ptr<GroupBvh>(new GroupBvh());

    for(int i = 0; i < (int)surfaces.size(); i ++)
    {
        shared_ptr<Surface> surface = surfaces[i];
        RayEngine* e = Build(surface, time, timeSamples);
        if (e)
            bvh->groups.push_back(e);
    }

    for(int i = 0; i < (int)instances.size(); i ++) 
    {
        shared_ptr<InstanceGroup> instance = instances[i];
        vector<RayEngine*> es;
        Build(instance, time, timeSamples, es);

        for (uint32_t j = 0; j < es.size(); j++)
        {
            if (es[j])
                bvh->groups.push_back(es[j]);
        }
    }

    bvh->BuildBVH();
    RayEngineX<BvhEngineGroupNode>* engine = new RayEngineX<BvhEngineGroupNode>(BvhEngineGroupNode(bvh));
    return engine;
}

template<>
RayEngine* RayBVHEngineBuilder::MakeNode(shared_ptr<MeshShape> shape, shared_ptr<Material> material, shared_ptr<Xform> xform)
{
    RayEngine* rayEngine = NULL;
    if (xform->IsStatic())
    {
        shared_ptr<TriangleBvh> data = shared_ptr<TriangleBvh>(new TriangleBvh(shape->PosArray(), shape->NormalArray(), shape->UvArray(), shape->FaceArray()));
        if (!shape->NormalArray().size() && !shape->UvArray().size())
        {
            rayEngine = new RayEngineX<BvhEngineMeshNode<WithoutNormal, WithoutUv> >(
                BvhEngineMeshNode<WithoutNormal, WithoutUv>(xform.get(), material.get(), data));
        }
        else if(shape->NormalArray().size() && !shape->UvArray().size())
        {
            rayEngine = new RayEngineX<BvhEngineMeshNode<WithNormal, WithoutUv> >(
                BvhEngineMeshNode<WithNormal, WithoutUv>(xform.get(), material.get(), data));
        }
        else if (!shape->NormalArray().size() && shape->UvArray().size())
        {
            rayEngine = new RayEngineX<BvhEngineMeshNode<WithoutNormal, WithUv> >(
                BvhEngineMeshNode<WithoutNormal, WithUv>(xform.get(), material.get(), data));
        }
        else if (shape->NormalArray().size() && shape->UvArray().size())
        {
            rayEngine = new RayEngineX<BvhEngineMeshNode<WithNormal, WithUv> >(
                BvhEngineMeshNode<WithNormal, WithUv>(xform.get(), material.get(), data));
        }
    }
    else
    {

    }
    return rayEngine;
}

template<>
RayEngine* RayBVHEngineBuilder::MakeNode(shared_ptr<SphereShape> shape, shared_ptr<Material> material, shared_ptr<Xform> xform)
{
    RayEngine* rayEngine = NULL;
    if (xform->IsStatic())
    {
        shared_ptr<SphereBvh> data = shared_ptr<SphereBvh>(new SphereBvh(shape->GetCenter(), shape->GetRadius()));
        rayEngine = new RayEngineX<BvhEngineSphereNode>(BvhEngineSphereNode(xform.get(), material.get(), data));
    }
    return rayEngine;
}

template<>
RayEngine* RayBVHEngineBuilder::MakeNode(shared_ptr<CurveShape> shape, shared_ptr<Material> material, shared_ptr<Xform> xform)
{
    RayEngine* rayEngine = NULL;
    if (xform->IsStatic())
    {
        shared_ptr<SegmentBvh> data = 
            shared_ptr<SegmentBvh>(new SegmentBvh(shape->Radius(), shape->PosArray(), shape->TangentArray(), shape->UvArray(), shape->RadiusArray(), shape->SegmentArray()));
        if (!shape->HasVertexTangent() && !shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //000
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithoutSegRadius> >(
                BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(!shape->HasVertexTangent() && !shape->HasVertexTexCoord() && shape->HasVertexRadius()) //001
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithSegRadius> >(
                BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
        else if(!shape->HasVertexTangent() && shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //010
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithoutSegRadius> >(
                BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(!shape->HasVertexTangent() && shape->HasVertexTexCoord() && shape->HasVertexRadius()) //011
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithSegRadius> >(
                BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && !shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //100
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithoutSegRadius> >(
                BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && !shape->HasVertexTexCoord() && shape->HasVertexRadius()) //101
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithSegRadius> >(
                BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //110
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithoutSegRadius> >(
                BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && shape->HasVertexTexCoord() && shape->HasVertexRadius()) //111
        {
            rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithSegRadius> >(
                BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
    }
    return rayEngine;
}


template<>
void RayBVHEngineBuilder::MakeNodes(shared_ptr<MeshShape> shape, vector<shared_ptr<Material> > &materials, vector<shared_ptr<Xform> > &xforms, vector<RayEngine*> &es)
{
    shared_ptr<TriangleBvh> data = shared_ptr<TriangleBvh>(new TriangleBvh(shape->PosArray(), shape->NormalArray(), shape->UvArray(), shape->FaceArray()));
    assert(materials.size() == xforms.size());
    RayEngine* rayEngine = NULL;
    for (uint32_t i = 0; i < materials.size(); i++)
    {
        shared_ptr<Material> material = materials[i];
        shared_ptr<Xform> xform = xforms[i];
        if (xform->IsStatic())
        {
            if (!shape->NormalArray().size() && !shape->UvArray().size())
            {
                rayEngine = new RayEngineX<BvhEngineMeshNode<WithoutNormal, WithoutUv> >(
                    BvhEngineMeshNode<WithoutNormal, WithoutUv>(xform.get(), material.get(), data));
            }
            else if(shape->NormalArray().size() && !shape->UvArray().size())
            {
                rayEngine = new RayEngineX<BvhEngineMeshNode<WithNormal, WithoutUv> >(
                    BvhEngineMeshNode<WithNormal, WithoutUv>(xform.get(), material.get(), data));
            }
            else if (!shape->NormalArray().size() && shape->UvArray().size())
            {
                rayEngine = new RayEngineX<BvhEngineMeshNode<WithoutNormal, WithUv> >(
                    BvhEngineMeshNode<WithoutNormal, WithUv>(xform.get(), material.get(), data));
            }
            else if (shape->NormalArray().size() && shape->UvArray().size())
            {
                rayEngine = new RayEngineX<BvhEngineMeshNode<WithNormal, WithUv> >(
                    BvhEngineMeshNode<WithNormal, WithUv>(xform.get(), material.get(), data));
            }
        }
        else
        {
            cerr << "animation xform not supported" << endl;
        }
        if (rayEngine)
            es.push_back(rayEngine);
    }
}

template<>
void RayBVHEngineBuilder::MakeNodes(shared_ptr<SphereShape> shape, vector<shared_ptr<Material> > &materials, vector<shared_ptr<Xform> > &xforms, vector<RayEngine*> &es)
{
    shared_ptr<SphereBvh> data = shared_ptr<SphereBvh>(new SphereBvh(shape->GetCenter(), shape->GetRadius()));
    assert(materials.size() == xforms.size());
    RayEngine* rayEngine = NULL;
    for (uint32_t i = 0; i < materials.size(); i++)
    {
        shared_ptr<Material> material = materials[i];
        shared_ptr<Xform> xform = xforms[i];
        if (xform->IsStatic())
        {
            rayEngine = new RayEngineX<BvhEngineSphereNode>(BvhEngineSphereNode(xform.get(), material.get(), data));
        }
        else
        {
            cerr << "animation xform not supported" << endl;
        }
        if (rayEngine)
            es.push_back(rayEngine);
    }
}

template<>
void RayBVHEngineBuilder::MakeNodes(shared_ptr<CurveShape> shape, vector<shared_ptr<Material> > &materials, vector<shared_ptr<Xform> > &xforms, vector<RayEngine*> &es)
{
    RayEngine* rayEngine = NULL;
    shared_ptr<SegmentBvh> data = 
        shared_ptr<SegmentBvh>(new SegmentBvh(shape->Radius(), shape->PosArray(), shape->TangentArray(), shape->UvArray(), shape->RadiusArray(), shape->SegmentArray()));
    assert(materials.size() == xforms.size());
    for (uint32_t i = 0; i < materials.size(); i++)
    {
        shared_ptr<Material> material = materials[i];
        shared_ptr<Xform> xform = xforms[i];
        if (xform->IsStatic())
        {

            if (!shape->HasVertexTangent() && !shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //000
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithoutSegRadius> >(
                    BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
            }
            else if(!shape->HasVertexTangent() && !shape->HasVertexTexCoord() && shape->HasVertexRadius()) //001
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithSegRadius> >(
                    BvhEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithSegRadius>(xform.get(), material.get(), data));
            }
            else if(!shape->HasVertexTangent() && shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //010
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithoutSegRadius> >(
                    BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
            }
            else if(!shape->HasVertexTangent() && shape->HasVertexTexCoord() && shape->HasVertexRadius()) //011
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithSegRadius> >(
                    BvhEngineSegmentNode<WithoutSegTangent, WithSegUv, WithSegRadius>(xform.get(), material.get(), data));
            }
            else if(shape->HasVertexTangent() && !shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //100
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithoutSegRadius> >(
                    BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
            }
            else if(shape->HasVertexTangent() && !shape->HasVertexTexCoord() && shape->HasVertexRadius()) //101
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithSegRadius> >(
                    BvhEngineSegmentNode<WithSegTangent, WithoutSegUv, WithSegRadius>(xform.get(), material.get(), data));
            }
            else if(shape->HasVertexTangent() && shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //110
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithoutSegRadius> >(
                    BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
            }
            else if(shape->HasVertexTangent() && shape->HasVertexTexCoord() && shape->HasVertexRadius()) //111
            {
                rayEngine = new RayEngineX<BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithSegRadius> >(
                    BvhEngineSegmentNode<WithSegTangent, WithSegUv, WithSegRadius>(xform.get(), material.get(), data));
            }
            if (rayEngine)
                es.push_back(rayEngine);
        }
        else
        {
            cerr << "animation xform not supported" << endl;
        }
    }
}

RayEngine* RayBVHEngineBuilder::Build(shared_ptr<Surface> surface, const Intervalf& time, int timeSamples)
{
    if (shared_ptr<MeshShape> shape = dynamic_pointer_cast<MeshShape>(surface->ShapeRef()))
    {
        return MakeNode(shape, surface->MaterialRef(), surface->XformRef());
    }
    else if (shared_ptr<SphereShape> shape = dynamic_pointer_cast<SphereShape>(surface->ShapeRef()))
    {
        return MakeNode(shape, surface->MaterialRef(), surface->XformRef());
    }
    else if (shared_ptr<CurveShape> shape = dynamic_pointer_cast<CurveShape>(surface->ShapeRef()))
    {
        return MakeNode(shape, surface->MaterialRef(), surface->XformRef());
    }
    else
    {
        cerr << "shape type not supported" << endl;
        return NULL;
    }
}

void RayBVHEngineBuilder::Build(shared_ptr<InstanceGroup> surface, const Intervalf& time, int timeSamples, vector<RayEngine*> &es)
{
    if (shared_ptr<MeshShape> shape = dynamic_pointer_cast<MeshShape>(surface->ShapeRef()))
    {
        MakeNodes(shape, surface->MaterialArray(), surface->XformArray(), es);
    }
    else if (shared_ptr<SphereShape> shape = dynamic_pointer_cast<SphereShape>(surface->ShapeRef()))
    {
        MakeNodes(shape, surface->MaterialArray(), surface->XformArray(), es);
    }
    else if (shared_ptr<CurveShape> shape = dynamic_pointer_cast<CurveShape>(surface->ShapeRef()))
    {
        MakeNodes(shape, surface->MaterialArray(), surface->XformArray(), es);
    }
    else
    {
        cerr << "shape type not supported" << endl;
    }
}
