#include "cubemap.h"
#include "image.h"
#include "waveletImageUtils.h"
#include "sparseImage.h"
#include "sparseCubeMap.h"
#include "mipmap.h"

Image<float> img(32,32);
MipMap<float> mp(img);
