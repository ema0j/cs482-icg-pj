#include "rayListEngineBuilder.h"
#include <scene/shape_sphere.h>
#include <scene/shape_curve.h>
#include "rayListEngineData.h"

RayEngine* RayListEngineBuilder::Build(const vector<shared_ptr<Surface> >& surfaces, 
    const vector<shared_ptr<InstanceGroup> >& instances, 
    const Intervalf& time, int timeSamples)
{

    RayEngineX<ListEngineGroupNode>* engine = new RayEngineX<ListEngineGroupNode>(ListEngineGroupNode());
    for(int i = 0; i < (int)surfaces.size(); i ++)
    {
        shared_ptr<Surface> surface = surfaces[i];
        RayEngine* e = Build(surface, time, timeSamples);
        if (e)
        {
            engine->t._groups.push_back(e);
        }
    }

    for(int i = 0; i < (int)instances.size(); i ++) 
    {
        shared_ptr<InstanceGroup> instance = instances[i];
        vector<RayEngine*> es;
        Build(instance, time, timeSamples, es);
    }
    return engine;
}

template<>
RayEngine* RayListEngineBuilder::MakeNode(shared_ptr<MeshShape> shape, shared_ptr<Material> material, shared_ptr<Xform> xform)
{
    RayEngine* rayEngine = NULL;
    if (xform->IsStatic())
    {
        shared_ptr<TriangleArray> data = shared_ptr<TriangleArray>(new TriangleArray(shape->PosArray(), shape->NormalArray(), shape->UvArray(), shape->FaceArray()));
        if (!shape->NormalArray().size() && !shape->UvArray().size())
        {
            ListEngineMeshNode<WithoutNormal, WithoutUv> t(xform.get(), material.get(), data);
            rayEngine = new RayEngineX<ListEngineMeshNode<WithoutNormal, WithoutUv> >(t);
        }
        else if(shape->NormalArray().size() && !shape->UvArray().size())
        {
            ListEngineMeshNode<WithNormal, WithoutUv> t(xform.get(), material.get(), data);
            rayEngine = new RayEngineX<ListEngineMeshNode<WithNormal, WithoutUv> >(t);
        }
        else if (!shape->NormalArray().size() && shape->UvArray().size())
        {
            ListEngineMeshNode<WithoutNormal, WithUv> t(xform.get(), material.get(), data);
            rayEngine = new RayEngineX<ListEngineMeshNode<WithoutNormal, WithUv> >(t);
        }
        else if (shape->NormalArray().size() && shape->UvArray().size())
        {
            rayEngine = new RayEngineX<ListEngineMeshNode<WithNormal, WithUv> >(
                ListEngineMeshNode<WithNormal, WithUv>(xform.get(), material.get(), data));
        }
    }
    else
    {

    }
    return rayEngine;
}

template<>
RayEngine* RayListEngineBuilder::MakeNode(shared_ptr<SphereShape> shape, shared_ptr<Material> material, shared_ptr<Xform> xform)
{
    RayEngine* rayEngine = NULL;
    if (xform->IsStatic())
    {
        shared_ptr<SphereArray> data = shared_ptr<SphereArray>(new SphereArray(shape->GetCenter(), shape->GetRadius()));
        rayEngine = new RayEngineX<ListEngineSphereNode>(ListEngineSphereNode(xform.get(), material.get(), data));
    }
    return rayEngine;
}

template<>
RayEngine* RayListEngineBuilder::MakeNode(shared_ptr<CurveShape> shape, shared_ptr<Material> material, shared_ptr<Xform> xform)
{
    RayEngine* rayEngine = NULL;
    if (xform->IsStatic())
    {
        shared_ptr<SegmentArray> data = 
            shared_ptr<SegmentArray>(new SegmentArray(shape->Radius(), shape->PosArray(), shape->TangentArray(), shape->UvArray(), shape->RadiusArray(), shape->SegmentArray()));
        if (!shape->HasVertexTangent() && !shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //000
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithoutSegRadius> >(
                ListEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(!shape->HasVertexTangent() && !shape->HasVertexTexCoord() && shape->HasVertexRadius()) //001
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithSegRadius> >(
                ListEngineSegmentNode<WithoutSegTangent, WithoutSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
        else if(!shape->HasVertexTangent() && shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //010
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithoutSegTangent, WithSegUv, WithoutSegRadius> >(
                ListEngineSegmentNode<WithoutSegTangent, WithSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(!shape->HasVertexTangent() && shape->HasVertexTexCoord() && shape->HasVertexRadius()) //011
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithoutSegTangent, WithSegUv, WithSegRadius> >(
                ListEngineSegmentNode<WithoutSegTangent, WithSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && !shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //100
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithSegTangent, WithoutSegUv, WithoutSegRadius> >(
                ListEngineSegmentNode<WithSegTangent, WithoutSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && !shape->HasVertexTexCoord() && shape->HasVertexRadius()) //101
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithSegTangent, WithoutSegUv, WithSegRadius> >(
                ListEngineSegmentNode<WithSegTangent, WithoutSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && shape->HasVertexTexCoord() && !shape->HasVertexRadius()) //110
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithSegTangent, WithSegUv, WithoutSegRadius> >(
                ListEngineSegmentNode<WithSegTangent, WithSegUv, WithoutSegRadius>(xform.get(), material.get(), data));
        }
        else if(shape->HasVertexTangent() && shape->HasVertexTexCoord() && shape->HasVertexRadius()) //111
        {
            rayEngine = new RayEngineX<ListEngineSegmentNode<WithSegTangent, WithSegUv, WithSegRadius> >(
                ListEngineSegmentNode<WithSegTangent, WithSegUv, WithSegRadius>(xform.get(), material.get(), data));
        }
    }
    return rayEngine;
}


RayEngine* RayListEngineBuilder::Build(shared_ptr<Surface> surface, const Intervalf& time, int timeSamples)
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

RayEngine* RayListEngineBuilder::Build(shared_ptr<InstanceGroup> surface, const Intervalf& time, int timeSamples, vector<RayEngine*> &es)
{
    return NULL;
}
