#include "material_smooth.h"
#include "sampling.h"

string LambertEmissionMaterial::serialize_typename() { return "LambertEmissionMaterial"; } 
void LambertEmissionMaterial::serialize(Archive * a) {
	a->member("Le", Le);
}

string LambertMaterial::serialize_typename() { return "LambertMaterial"; } 
void LambertMaterial::serialize(Archive* a) 
{
	a->member("rd", rd);
}

string TexturedLambertMaterial::serialize_typename() { return "TexturedLambertMaterial"; } 
void TexturedLambertMaterial::serialize(Archive* a) {
	a->optional("rd", rd, Vec3f::One());
	a->optional("rdTxt", rdTxt, shared_ptr<TextureV>());
}


string PhongMaterial::serialize_typename() { return "PhongMaterial"; } 
void PhongMaterial::serialize(Archive* a) {
	a->member("rd", rd);
	a->member("rs", rs);
	a->member("n", n);
}

//string BlinnPhongMaterial::serialize_typename() { return "BlinnPhongMaterial"; } 
//void BlinnPhongMaterial::serialize(Archive* a) {
//	a->member("rd", rd);
//	a->member("rs", rs);
//	a->member("n", n);
//}
string TexturedPhongMaterial::serialize_typename() { return "TexturedPhongMaterial"; } 
void TexturedPhongMaterial::serialize(Archive* a) {
    a->optional("rd", rd, Vec3f::One());
    a->optional("rdTxt", rdTxt, shared_ptr<TextureV>());
    a->optional("rs", rs, Vec3f::One());
    a->optional("rsTxt", rsTxt, shared_ptr<TextureV>());
    a->optional("n", n, 100.0f);
}

//string WardIsotropicMaterial::serialize_typename() { return "WardIsotropicMaterial"; } 
//void WardIsotropicMaterial::serialize(Archive* a) {
//	a->member("rd", rd);
//	a->member("rs", rs);
//	a->member("a", this->a);
//}

string CookTorranceMaterial::serialize_typename() { return "CookTorranceMaterial"; } 
void CookTorranceMaterial::serialize(Archive* a) {
	a->member("rd", rd);
	a->member("rs", rs);
	a->member("f0", this->f0);
	a->member("sigma", this->sigma);
}

//string KajiyaHairMaterial::serialize_typename() { return "KajiyaHairMaterial"; } 
//void KajiyaHairMaterial::serialize(Archive* a) {
//	a->member("Kd", Kd);
//	a->member("Ks", Ks);
//	a->member("n", n);
//}
//
//string MappedKajiyaHairMaterial::serialize_typename() { return "MappedKajiyaHairMaterial"; } 
//void MappedKajiyaHairMaterial::serialize(Archive* a) {
//	a->member("KdTxt", KdTxt);
//	a->member("KsTxt", KsTxt);
//	a->member("n", n);
//}
//
