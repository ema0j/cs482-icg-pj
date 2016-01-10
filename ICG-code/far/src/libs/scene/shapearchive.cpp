#include "shapearchive.h"

#include <serialization/archivehelper.h>

#include "shape_mesh.h"
#include "shape_subdiv.h"
#include "shape_deformedmesh.h"
#include "shape_sphere.h"
#include "shape_curve.h"
#include "shape_subdiv.h"

void ShapeArchive::save(const string& filename, shared_ptr<Shape> shape) {
    ArchiveHelper::save<Shape>("shape", filename, shape, registerSceneTypes);
}

void ShapeArchive::prettyprint(shared_ptr<Shape> shape) {
    ArchiveHelper::prettyprint<Shape>("shape", shape, registerSceneTypes);
}

void ShapeArchive::load(const string& filename, shared_ptr<Shape>& shape) {
    ArchiveHelper::load<Shape>("shape", filename, shape, registerSceneTypes);
}

void ShapeArchive::registerSceneTypes() { 
	static bool done = false;
	if(done) return;
	
	// shapes
	ArchiveHelper::registerType<MeshShape>();
	ArchiveHelper::registerType<SphereShape>();
	ArchiveHelper::registerType<CurveShape>();
	ArchiveHelper::registerType<BlendedMeshShape>();
	ArchiveHelper::registerType<SkinnedMeshShape>();
	ArchiveHelper::registerType<KeyframedMeshShape>();
	ArchiveHelper::registerType<CatmullClarkShape>();
	
	// done
	done = true;
}
