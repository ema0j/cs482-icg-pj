#include "source_area.h"
#include "surface.h"
#include "shape_mesh.h"
#include "material_smooth.h"
#include "intersectionMethods.h"
#include <vmath/spectrum3.h>

void AreaSource::serialize(Archive * a) {
    a->member("Le", le);
    a->member("shadowSamples", shadowSamples);
}

Vec3f QuadAreaSource::SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf) {
    Vec3f Pl = ComputeSurfacePos(s * 2 + Vec2f(-1,-1));
    *wi = (Pl - P).GetNormalized();
    *shadowRayMaxDist = (Pl - P).GetLength();
    float cosAngle = max(-(*wi).z,0.0f);
    // in pbrt, everything but L is folded in pdf
    *pdf = cosAngle > 0 ? (Pl - P).GetLengthSqr() / (cosAngle * Area()) : 0.0f;
    return le;
}

float QuadAreaSource::SampleShadowPdf(const Vec3f &P, const Vec3f &wi, float time) {
    Vec3f Pl;
    if(!_Intersect(P,wi,Pl)) return 0;
    float cosAngle = max(-wi.z,0.0f);
    return (Pl - P).GetLengthSqr() / (cosAngle * Area());
}

Vec3f QuadAreaSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    *E = ComputeSurfacePos(ss * 2 + Vec2f(-1,-1));
    float anglePdf;
    *D = Sampling::HemisphericalDirection(sa, &anglePdf);
    *pdf = anglePdf / Area();
    // BUG ???
    // no cosine here according to pbrt ??? 
    // pbrt move it outside to make the rendering equation more clear.
    // we put it here for efficiency.
     float cosAngle = max((*D).z,0.0f);
     return le * cosAngle;
    // the assert is for the cosine

    //return le;
}

shared_ptr<VLightSample> QuadAreaSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const 
                                              Vec2f& ss, const Vec2f& sa, float time)
{
    Vec3f P = ComputeSurfacePos(ss * 2 + Vec2f(-1,-1));
    float anglePdf;
    Vec3f D = Sampling::HemisphericalDirection(sa, &anglePdf);
    Vec3f irradiance = le * Area();
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new OrientdVLightSample(P, Vec3f::Z(), irradiance));
    return sample;
}

string QuadAreaSource::serialize_typename() { return "QuadAreaSource"; } 

void QuadAreaSource::serialize(Archive * a) {
    AreaSource::serialize(a);
    a->member("size", size);
    a->optional("surface", insertSurface, false);
}

bool QuadAreaSource::_Intersect(const Vec3f& P, const Vec3f& wi, Vec3f& Pl) {
    carray<Vec3f> meshPos(4);
    meshPos[0] = ComputeSurfacePos(Vec2f(-1,-1));
    meshPos[1] = ComputeSurfacePos(Vec2f(-1,1));
    meshPos[2] = ComputeSurfacePos(Vec2f(1,1));
    meshPos[3] = ComputeSurfacePos(Vec2f(1,-1));

    Ray ray(P,wi,0);

    float t, b1, b2, rayEpsilon;
    if(IntersectTriangle(meshPos[0], meshPos[1], meshPos[2], ray, &t, &b1, &b2, &rayEpsilon)) {
        Pl = meshPos[0] * (1-b1-b2) +  meshPos[1] * b1 + meshPos[2] * b2;
        return true;
    }
    if(IntersectTriangle(meshPos[0], meshPos[2], meshPos[3], ray, &t, &b1, &b2, &rayEpsilon)) {
        Pl = meshPos[0] * (1-b1-b2) +  meshPos[2] * b1 + meshPos[3] * b2;
        return true;
    }

    return false;
}

shared_ptr<Surface> QuadAreaSource::_CreateIntersectionMesh(shared_ptr<Xform> xform) {
    carray<Vec3f> meshPos(4);
    meshPos[0] = ComputeSurfacePos(Vec2f(-1,-1));
    meshPos[1] = ComputeSurfacePos(Vec2f(-1,1));
    meshPos[2] = ComputeSurfacePos(Vec2f(1,1));
    meshPos[3] = ComputeSurfacePos(Vec2f(1,-1));

    carray<Vec3f> meshNormal(4);
    meshNormal[0] = Vec3f::Z();
    meshNormal[1] = Vec3f::Z();
    meshNormal[2] = Vec3f::Z();
    meshNormal[3] = Vec3f::Z();

    carray<Vec3i> meshIdx(2);
    meshIdx[0][0] = 0; meshIdx[0][1] = 1; meshIdx[0][2] = 2;
    meshIdx[1][0] = 0; meshIdx[1][1] = 2; meshIdx[1][2] = 3;

    shared_ptr<Material> meshMaterial = shared_ptr<Material>(new LambertEmissionMaterial(le));
    return shared_ptr<Surface>(new Surface(shared_ptr<Shape>(new MeshShape(meshPos,meshNormal,carray<Vec2f>(),meshIdx)),meshMaterial,xform,true));
}

Vec3f ShapeAreaSource::SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf) {
    Vec3f Pl, Ns;
    _shape->SamplePoint(s, &Pl, &Ns);
    *wi = (Pl - P).GetNormalized();
    *shadowRayMaxDist = (Pl - P).GetLength();
    float cosAngle = max((-(*wi) % Ns),0.0f);
    // in pbrt, everything but L is folded in pdf
    *pdf = cosAngle > 0 ? (Pl - P).GetLengthSqr() / (cosAngle * Area()) : 0.0f;
    return le;
}

float ShapeAreaSource::SampleShadowPdf(const Vec3f &P, const Vec3f &wi, float time) {
    Vec3f Pl;
    if(!_Intersect(P, wi, Pl)) return 0;
    float cosAngle = max(-wi.z,0.0f);
    return (Pl - P).GetLengthSqr() / (cosAngle * Area());
}

bool ShapeAreaSource::_Intersect(const Vec3f& P, const Vec3f& wi, Vec3f& Pl) {
    Ray ray(P,wi,0);
    float t;
    DifferentialGeometry dg;
    if(_shape->Intersect(ray, t, dg))
    {
        Pl = dg.P;
        return true;
    }
    else
        return false;
}


string ShapeAreaSource::serialize_typename() { return "ShapeAreaSource"; } 

void ShapeAreaSource::serialize(Archive * a) {
    AreaSource::serialize(a);
    a->member("shape", _shape);
}

Vec3f EnvSource::SampleShadow(const Vec3f& P, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf) {
    if(!useImportanceSampling){ 
        *wi = Sampling::SphericalDirection(s, pdf);
    }else{
        // Find floating-point (u,v) sample coordinates
        Vec2f pdfs, uv;
        uv[0] = uDist.Sample(s[0], &pdfs[0]) / uDist.GetFuncSize();
        uint64_t u = uDist.Map(uv[0]);
        uv[1] = vDist[u].Sample(s[1], &pdfs[1]) / vDist[u].GetFuncSize();
        uv[1] = 1-uv[1]; // flip y since texture are flipped
        *wi = CubeMap<Vec3f>::LatLongDirFromUV(uv);
        // Compute PDF for sampled direction
        *pdf = (pdfs[0] * pdfs[1]) / (2.0f * PIf * PIf * sinf(PIf*(1-uv[1])));
    }
    *shadowRayMaxDist = RAY_INFINITY;
    return le * _Sample(*wi);
}

float EnvSource::SampleShadowPdf(const Vec3f& P, const Vec3f& wi, float time) {
    if(!useImportanceSampling){ 
        return Sampling::SphericalDirectionPdf();
    } else {
        Vec2f uv = CubeMap<Vec3f>::LatLongUVFromDir(wi);
        uv[1] = 1-uv[1]; // flip y since texture are flipped
        uint64_t u = uDist.Map(uv[0]);
        Vec2f pdfs(uDist.Pdf(uv[0]), vDist[u].Pdf(uv[1]));
        return (pdfs[0] * pdfs[1]) / (2.0f * PIf * PIf * sinf(PIf*(1-uv[1])));
    }
}

/*
Vec3f EnvSource::SampleShadow(const Vec3f& P, const Vec3f& N, const Vec2f& s, float time, Vec3f* wi, float* shadowRayMaxDist, float* pdf) {
    *shadowRayMaxDist = RAY_INFINITY;
    *wi = Sampling::SphericalDirectionCos(N, s, pdf);
    return le * _Sample(*wi);
}
*/

#define _EPSILONSCALE 1.00001f
Vec3f EnvSource::SamplePhoton(const Vec3f& sceneCenter, float sceneRadius, const Vec2f& ss, const Vec2f& sa, float time, Vec3f* E, Vec3f* D, float* pdf) {
    // from pbrt
    float aux;
    Vec3f D1 = Sampling::SphericalDirection(ss,&aux); // also a point on the sphere
    Vec3f D2 = Sampling::SphericalDirection(sa,&aux); // also a point on the sphere
    *E = D1 * (sceneRadius * _EPSILONSCALE) + sceneCenter;
    *D = (D2 - D1).GetNormalized();
    float cosThetaCenter = max((*D)%(-D1), 0.0f); // -D1 is direction to center
    *pdf = cosThetaCenter / (4.0f * PIf * sceneRadius * sceneRadius);
    return le * _Sample(-(*D));
}

shared_ptr<VLightSample> EnvSource::SampleVLight(const Vec3f& sceneCenter, float sceneRadius, const 
                                              Vec2f& ss, const Vec2f& sa, float time)
{
    float pdfs0, pdfs1;
    Vec3f D1 = Sampling::SphericalDirection(ss, &pdfs0); // also a point on the sphere
    Vec3f D2 = Sampling::SphericalDirection(sa, &pdfs1); // also a point on the sphere
    Vec3f P = D1 * (sceneRadius * _EPSILONSCALE) + sceneCenter;
    Vec3f D = (D2 - D1).GetNormalized();
    float cosThetaCenter = max((D)%(-D1), 0.0f); // -D1 is direction to center

    Vec3f s;
    if (D.IsZero())
    {
        D = Vec3f::Z();
        s = Vec3f();
    }
    else
    s = _Sample(-D);
    float sinTheta = (-D).y;
    Vec3f radiance = le * s * (4.0f * PIf);
    shared_ptr<VLightSample> sample = shared_ptr<VLightSample>(
        new DirVLightSample(D, radiance));
    return sample;
}

void EnvSource::serialize(Archive* a) {
    AreaSource::serialize(a);
    a->member("importanceSampling", useImportanceSampling);
}

void EnvSource::_BuildImportancePDFs(Image<float> &luminance) {
    // build v pdf
    int nu = luminance.Width(), nv = luminance.Height();
    carray<float> func(nv);
    carray<float> sins(nv);
    for(int v = 0; v < nv; v ++) {
        sins[v] = sinf(PIf * (v+0.5f)/float(nv));
    }
    vDist.resize(nu);
    for(int u = 0; u < nu; u ++) {
        for(int v = 0; v < nv; v ++) {
            func[v] = luminance.ElementAt(u,v) * sins[v];
        }
        vDist[u].Init(func);
    }
    
    // build u pdf
    func.resize(nu);
    for(int u = 0; u < nu; u ++) {
        func[u] = vDist[u].GetFuncIntegral();
    }
    uDist.Init(func);
}

string EnvCubeSource::serialize_typename() { return "EnvCubeSource"; } 

void EnvCubeSource::serialize(Archive* a) {
    EnvSource::serialize(a);
    a->member("map", map);
    if(a->isreading() && useImportanceSampling) _InitImportanceSampling();
}

void EnvCubeSource::_InitImportanceSampling() {
    if(vDist.size() != 0) return;

    // get a luminance map
    Image<Vec3f> image(map->CubeMapRef().Resolution()*4,map->CubeMapRef().Resolution()*2);
    map->CubeMapRef().ToLatLong(image,1); // let us be cheap here and sample little
    image.FlipY(); //this is to match implementation of imp. samp. pdfs, which may be up-side-down wrt latlong maps (adjusted to be accurate, but messing up cube maps)
    Image<float> luminance(image.Width(), image.Height());
    for(unsigned int i = 0; i < luminance.Size(); i ++) {
        Vec3f rgb = image.ElementAt(i);
        luminance[i] = Spectrum3f::ToLuminance(rgb.x,rgb.y,rgb.z);
    }

    _BuildImportancePDFs(luminance);
}

string EnvLatLongSource::serialize_typename() { return "EnvLatLongSource"; } 

void EnvLatLongSource::serialize(Archive* a) {
    EnvSource::serialize(a);
    a->member("map", map);
    if(a->isreading() && useImportanceSampling) _InitImportanceSampling();
}

void EnvLatLongSource::_InitImportanceSampling() {
    if(vDist.size() != 0) return;

    // get a luminance map
    Image<Vec3f>& image = map->ImageRef();
    Image<float> luminance = Image<float>(image.Width(), image.Height());
    for(unsigned int i = 0; i < luminance.Size(); i ++) {
        Vec3f rgb = image[i];
        luminance[i] = Spectrum3f::ToLuminance(rgb.x,rgb.y,rgb.z);
    }

    _BuildImportancePDFs(luminance);
}

