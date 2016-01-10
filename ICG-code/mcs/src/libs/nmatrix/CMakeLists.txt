# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
kdtree.h
MatrixData.h
KnnMatrixImpl.h
KnnMatrixImpl.cpp
KnnMatrix.h
KnnMatrix.cpp
GatherGroupTbbImpl.h
GatherGroupTbbImpl.cpp
)

ADD_LIBRARY(cmatrix ${SOURCES})

TARGET_LINK_LIBRARIES(cmatrix scene lightgen vmath vlutil tbbutils ${LIBS_gsl} ${LIBS_cblas})
