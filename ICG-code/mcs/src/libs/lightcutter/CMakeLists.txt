# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
Lightcutter.h
Lightcutter.cpp
MTLightcutter.h
MTLightcutter.cpp
MdLightcutter.h
MdLightcutter.cpp
MTMdLightcutter.h
MTMdLightcutter.cpp
#Leafcutter.h
#Leafcutter.cpp
)

ADD_LIBRARY(lightcutter ${SOURCES})

TARGET_LINK_LIBRARIES(lightcutter lighttree scene vmath tbbutils)
