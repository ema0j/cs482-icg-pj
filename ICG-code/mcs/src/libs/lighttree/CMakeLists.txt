# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
LightTree.h
LightTree.cpp
BuilderLightCache.h
#BuilderKdTree.h
#BuilderKdTreeImpl.h
LightTreeBuilderUtil.h
LightTreeBuilder.h
LightTreeBuilder.cpp
DivisiveLightTreeBuilder.h
DivisiveLightTreeBuilder.cpp
#MdLightTreeBuilder.h
#MdLightTreeBuilder.cpp
#MdDebugLightTreeBuilder.h
#MdDebugLightTreeBuilder.cpp
#MdDivisiveLightTreeBuilder.h
#MdDivisiveLightTreeBuilder.cpp
GatherTree.h
GatherTree.cpp
GatherTreeBuilder.h
GatherTreeBuilder.cpp
#AgglomerativeLightTreeBuilder.h
#AgglomerativeLightTreeBuilder.cpp
)

ADD_LIBRARY(lighttree ${SOURCES})

TARGET_LINK_LIBRARIES(lighttree scene vmath)
