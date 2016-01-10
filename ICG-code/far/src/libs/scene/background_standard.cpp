#include "background_standard.h"

string LightBackground::serialize_typename() { return "LightBackground"; }
void LightBackground::serialize(Archive * a) {
	a->member("light", light);
}
