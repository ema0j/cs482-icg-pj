template<typename T>
ImageTexture<T>::ImageTexture() 
{
    linear = true;
    mipmap = false;
    tile = true;
    flipY = true;
    filename = "";
}

template<typename T>
ImageTexture<T>::ImageTexture(const string& imfile, bool load, bool linear, bool mipmap, bool tile, bool flipY) {
    this->linear = linear;
    this->mipmap = mipmap;
    this->tile = tile;
    this->flipY = flipY;
    filename = imfile;

    if(load) {
        Image<T> *im = ImageIO::Load<T>(filename);
        if(im->Width() == 0 || im->Height() == 0) cerr << "cannot load texture " << filename << endl << flush;
        image.Copy(*im);
        delete im;
        _Init();
    }
}

template<typename T>
ImageTexture<T>::ImageTexture(const Image<Vec3f>& im, const string& imfile, bool linear, bool mipmap, bool tile, bool flipY) {
    this->linear = linear;
    this->mipmap = mipmap;
    this->tile = tile;
    this->flipY = flipY;

    filename = imfile;
    image.Copy(im);
    _Init();
}

template<> float ImageTexture<float>::Sample(const Vec2f& st);
template<> Vec3f ImageTexture<Vec3f>::Sample(const Vec2f& st);

template<typename T>
void ImageTexture<T>::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Textures");
    stat->Increment();
}

template<typename T>
void ImageTexture<T>::serialize(Archive* a) {
    a->member("filename", filename);
    a->optional("linear", linear, true);
    a->optional("tile", tile, true);
    a->optional("mipmap", mipmap, false);
    a->optional("flipy", flipY, true);

    if(a->isreading()) {
        Image<T> *im = ImageIO::Load<T>(filename);
        if(im->Width() == 0 || im->Height() == 0) cerr << "cannot load texture " << filename << endl << flush;
        image.Copy(*im);
        delete im;
        _Init();
    }
}

template<typename T>
void ImageTexture<T>::_Init() {
    if(flipY) image.FlipY();
    average = image.Average();
}

template<> inline string ImageTexture<Vec3f>::serialize_typename() { return "Texture"; }
template<> inline string ImageTexture<float>::serialize_typename() { return "TextureF"; }

template<typename T>
inline void ConstTexture<T>::serialize(Archive* a) { a->member("v", _value); }

template<> inline string ConstTexture<float>::serialize_typename() { return "ConstTextureF"; }
template<> inline string ConstTexture<Vec3f>::serialize_typename() { return "ConstTexture"; }

template<> inline string ScaleTexture<float, float>::serialize_typename() { return "ScaleTextureF"; }
template<> inline string ScaleTexture<Vec3f, float>::serialize_typename() { return "ScaleTextureVF"; }
template<> inline string ScaleTexture<Vec3f, Vec3f>::serialize_typename() { return "ScaleTexture"; }

template<typename T, typename T2>
void ScaleTexture<T, T2>::serialize( Archive* a )
{
    a->member("tex1", _tex1);
    a->member("tex2", _tex2);
}
