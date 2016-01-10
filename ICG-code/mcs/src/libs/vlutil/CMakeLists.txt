# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
LightEval.h
LightEval.cpp
ListVirtualLightCache.h
SimpleVirtualLightCache.h
)

ADD_LIBRARY(vlutil ${SOURCES})

TARGET_LINK_LIBRARIES(vlutil scene vmath)
