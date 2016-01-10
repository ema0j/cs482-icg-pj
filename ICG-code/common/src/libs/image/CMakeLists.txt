# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
cubemap.h
image.cpp
image.h
mipmap.h
sparseImage.h
sparseCubeMap.h
waveletImageUtils.h
imageUtils.cpp
imageUtils.h
cubeUtils.cpp
cubeUtils.h
)

ADD_LIBRARY(image ${SOURCES})
