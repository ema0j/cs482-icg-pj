# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
LightData.h
LightData.cpp
LightGenerator.cpp
LightGenerator.h
LightDiffuseGenerator.cpp
LightDiffuseGenerator.h
LightSerializeGenerator.h
LightSerializeGenerator.cpp
SerializableLightCache.h
SerializableLightCache.cpp
)

ADD_LIBRARY(lightgen ${SOURCES})

TARGET_LINK_LIBRARIES(lightgen sampler ray scene)
