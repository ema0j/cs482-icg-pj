# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
Distribution1D.h
pathSampler.cpp
pathSampler.h
LightSampleUtils.h
LightSampleUtils.cpp
)

ADD_LIBRARY(sampler ${SOURCES})

TARGET_LINK_LIBRARIES(sampler)