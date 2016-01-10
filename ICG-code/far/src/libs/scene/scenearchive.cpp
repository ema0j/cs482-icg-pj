#include "scenearchive.h"

#include "background.h"
#include "camera.h"
#include "source_delta.h"
#include "source_area.h"
#include "material_smooth.h"
#include "material_delta.h"
#include "material_uber.h"
#include "shape_mesh.h"
#include "shape_subdiv.h"
#include "shape_deformedmesh.h"
#include "shape_sphere.h"
#include "shape_curve.h"
#include "shape_subdiv.h"
#include "xform.h"
#include "lens.h"
#include "lens_standard.h"
#include "instance.h"
#include "background_standard.h"

#include <scene/xform_static.h>
#include <scene/xform_dynamic.h>


void SceneArchive::save(const string& filename, shared_ptr<Scene> scene) {
    ArchiveHelper::save<Scene>("scene", filename, scene, registerSceneTypes);
}

void SceneArchive::prettyprint(shared_ptr<Scene> scene) {
    ArchiveHelper::prettyprint<Scene>("scene", scene, registerSceneTypes);
}

shared_ptr<Scene> SceneArchive::load(const string& filename) {
	shared_ptr<Scene> scene;
    ArchiveHelper::load<Scene>("scene", filename, scene, registerSceneTypes);
	return scene;
}

void SceneArchive::load(const string& filename, shared_ptr<Scene>& scene) {
	ArchiveHelper::load<Scene>("scene", filename, scene, registerSceneTypes);
}

void SceneArchive::registerSceneTypes() { 
	static bool done = false;
	if(done) return;
	
	// scene
	ArchiveHelper::registerType<Scene>();
	
	// background
	ArchiveHelper::registerType<LightBackground>();
	
	// cameras
	ArchiveHelper::registerType<Camera>();

    // lens
    ArchiveHelper::registerType<PinholeLens>();
    ArchiveHelper::registerType<ThinLens>();
	
	// surface
	ArchiveHelper::registerType<Surface>();
	
	// instance group
	ArchiveHelper::registerType<InstanceGroup>();
	
	// shapes
	ArchiveHelper::registerType<MeshShape>();
	ArchiveHelper::registerType<SphereShape>();
	ArchiveHelper::registerType<CurveShape>();
	ArchiveHelper::registerType<BlendedMeshShape>();
	ArchiveHelper::registerType<SkinnedMeshShape>();
	ArchiveHelper::registerType<KeyframedMeshShape>();
	ArchiveHelper::registerType<CatmullClarkShape>();
	
	// materials
	ArchiveHelper::registerType<LambertEmissionMaterial>();
	ArchiveHelper::registerType<LambertMaterial>();
    ArchiveHelper::registerType<UberMaterial>();
	ArchiveHelper::registerType<TexturedLambertMaterial>();
	ArchiveHelper::registerType<PhongMaterial>();
	//ArchiveHelper::registerType<BlinnPhongMaterial>();
    ArchiveHelper::registerType<TexturedPhongMaterial>();
	ArchiveHelper::registerType<PerfectReflectionMaterial>();
	ArchiveHelper::registerType<SimpleRefractionMaterial>();
	//ArchiveHelper::registerType<KajiyaHairMaterial>();
	//ArchiveHelper::registerType<MappedKajiyaHairMaterial>();
	//ArchiveHelper::registerType<WardIsotropicMaterial>();
	ArchiveHelper::registerType<CookTorranceMaterial>();
	
	// lights
	ArchiveHelper::registerType<Light>();
	
	// sources
	ArchiveHelper::registerType<PointSource>();
	ArchiveHelper::registerType<DirectionalSource>();
	ArchiveHelper::registerType<SpotSource>();
	ArchiveHelper::registerType<CosineSource>();
	ArchiveHelper::registerType<CosineClampedSource>();
	ArchiveHelper::registerType<QuadAreaSource>();
    // ArchiveHelper::registerType<ShapeAreaSource>();
	ArchiveHelper::registerType<EnvCubeSource>();
	ArchiveHelper::registerType<EnvLatLongSource>();
	
	// textures
    ArchiveHelper::registerType<ImageTextureV>();
    ArchiveHelper::registerType<ConstTextureV>();
    ArchiveHelper::registerType<ScaleTextureV>();
    ArchiveHelper::registerType<ImageTextureF>();
    ArchiveHelper::registerType<ConstTextureF>();
    ArchiveHelper::registerType<ScaleTextureF>();
    ArchiveHelper::registerType<ScaleTextureVF>();
    ArchiveHelper::registerType<CubeTexture>();

	
	// xforms
	ArchiveHelper::registerType<XformIdentity>();
	ArchiveHelper::registerType<XformStatic>();
	ArchiveHelper::registerType<XformStaticStack>();
	ArchiveHelper::registerType<XformStaticTranslation>();
	ArchiveHelper::registerType<XformStaticRotation>();
    ArchiveHelper::registerType<XformStaticXYZRotation>();
    ArchiveHelper::registerType<XformStaticMatrix>();
	ArchiveHelper::registerType<XformStaticScale>();
	ArchiveHelper::registerType<XformStaticLookAt>();
	ArchiveHelper::registerType<XformKeyframed>();
	ArchiveHelper::registerType<XformMatKeyframed>();
	
	// done
	done = true;
}
