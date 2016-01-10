#include "material_delta.h"
#include "sampling.h"

string PerfectReflectionMaterial::serialize_typename() { return "PerfectReflectionMaterial"; } 
void PerfectReflectionMaterial::serialize(Archive* a) {
	a->member("Kr", kr);
}

string SimpleRefractionMaterial::serialize_typename() { return "SimpleRefractionMaterial"; } 
void SimpleRefractionMaterial::serialize(Archive* a) {
	a->member("Kt", kt);
	a->member("index", index);
}

