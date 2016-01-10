#include "scene.h"
#include <misc/stats.h>
#include "camera.h"
#include "background.h"
#include "surface.h"
#include "instance.h"
#include "light.h"
#include "texture.h"

void Scene::Init() {
	// copy current surfaces/instances
    vector<shared_ptr<Surface> > currentSurfaces = surfaces;
    vector<shared_ptr<InstanceGroup> > currentInstances = instances;

    // compute shape usage references
    map<shared_ptr<Shape>, pair<int, shared_ptr<InstanceGroup> > > shapeToGroups;
    for(size_t s = 0; s < currentSurfaces.size(); s ++) {
		if (currentSurfaces[s]->ShapeRef())
			shapeToGroups[currentSurfaces[s]->ShapeRef()].first ++;
    }

    // build surface list
    surfaces.clear();
    for(size_t s = 0; s < currentSurfaces.size(); s ++) {
        if(currentSurfaces[s]->IsAreaLight()) 
			continue;
		if(!currentSurfaces[s]->ShapeRef()) 
			continue;
        if(shapeToGroups[currentSurfaces[s]->ShapeRef()].first > 1) 
			continue;
        surfaces.push_back(currentSurfaces[s]);
    }

    // add lights to surface list
    for(size_t l = 0; l < lights.size(); l ++) {
        shared_ptr<Surface> surface = lights[l]->GetSurface();
        if(surface) surfaces.push_back(surface);
    }

    // add instance gruops
    instances.clear();
    for(size_t i = 0; i < currentInstances.size(); i ++) {
        if(shapeToGroups.find(currentInstances[i]->ShapeRef()) != shapeToGroups.end()) { 
            cerr << "weird referenceing in instances" << endl; assert(0); continue; }
        instances.push_back(currentInstances[i]);
    }

	// group together shared shapes
    for(size_t s = 0; s < currentSurfaces.size(); s ++) {
        if(shapeToGroups[currentSurfaces[s]->ShapeRef()].first == 1) continue;
        if (currentSurfaces[s]->ShapeRef().get() == NULL) continue;
        if(!shapeToGroups[currentSurfaces[s]->ShapeRef()].second) {
            shared_ptr<InstanceGroup> group = shared_ptr<InstanceGroup>(new InstanceGroup());
            group->ShapeRef() = currentSurfaces[s]->ShapeRef();
            instances.push_back(group);
			shapeToGroups[currentSurfaces[s]->ShapeRef()].second = group;
		}
		shared_ptr<InstanceGroup> group = shapeToGroups[currentSurfaces[s]->ShapeRef()].second;
		group->XformArray().push_back(currentSurfaces[s]->XformRef());
		group->MaterialArray().push_back(currentSurfaces[s]->MaterialRef());
#ifdef _DEBUG
        cerr << "merging surface into instance group" << endl;
#endif // _DEBUG
    }
}

bool Scene::IsStatic() {
    for(int s = 0; s < (int)surfaces.size(); s ++) {
        if(!surfaces[s]->IsStatic()) return false;
    }
    return true;
}

Intervalf Scene::ComputeAnimationInterval() {
    Intervalf ret = Intervalf::Invalid();
    if(!IsStatic()) { 
        for(int s = 0; s < (int)surfaces.size(); s ++) {
            ret.Include(surfaces[s]->ComputeAnimationInterval());
        }
    }
    return ret;
}

Range3f Scene::ComputeBoundingBox(const Intervalf& time, int approximationSamples) {
    Range3f bbox;
    for(size_t s = 0; s < surfaces.size(); s ++) {
        bbox.Grow(surfaces[s]->ComputeBoundingBox(time, approximationSamples));
    }
    return bbox;
}

void Scene::FlattenXform() {
    for(int s = 0; s < (int)surfaces.size(); s ++) {
        surfaces[s]->FlattenXform();
    }
}

void Scene::CollectStats(StatsManager& stats) {
    if(mainCamera) mainCamera->CollectStats(stats);
	if(background) background->CollectStats(stats);
    for(size_t i = 0; i < lights.size(); i ++) lights[i]->CollectStats(stats);
    for(size_t i = 0; i < surfaces.size(); i ++) surfaces[i]->CollectStats(stats);
}

void Scene::serialize(Archive* a) {
	if(a->isreading()) {
		vector<shared_ptr<Material> >               reading_materials;
		vector<shared_ptr<Shape> >                  reading_shapes;
		vector<shared_ptr<SceneObject> >            reading_textures;
		vector<shared_ptr<CubeTexture> >            reading_cubes;

		a->optional("mainCamera", mainCamera, shared_ptr<Camera>());

		a->iterate("texture", reading_textures); // fake for dangling references
		a->iterate("cubetexture", reading_cubes); // fake for dangling references
		a->iterate("material", reading_materials); // fake for dangling references
		a->iterate("shape", reading_shapes); // fake for dangling references

		a->iterate("light", lights);
		a->optional("background", background, shared_ptr<Background>());
		a->iterate("surface", surfaces);
		a->iterate("instanceGroup", instances);

        Init();
	} else {
		vector<shared_ptr<Material> >           reading_materials;
		vector<shared_ptr<Shape> >              reading_shapes;
		vector<shared_ptr<SceneObject> >        reading_textures;
		vector<shared_ptr<CubeTexture> >        reading_cubes;
        
		a->optional("mainCamera", mainCamera, shared_ptr<Camera>()); // feel free to output empty scene
        
		a->iterate("texture", reading_textures); // fake for dangling references
		a->iterate("cubetexture", reading_cubes); // fake for dangling references
		a->iterate("material", reading_materials); // fake for dangling references
		a->iterate("shape", reading_shapes); // fake for dangling references
		
		a->iterate("light", lights);
		a->optional("background", background, shared_ptr<Background>());
		a->iterate("surface", surfaces);
		a->iterate("instanceGroup", instances);
	}
}
