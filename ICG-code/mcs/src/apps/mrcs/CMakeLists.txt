# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
main.cpp
MrcsCascade.cpp
MrcsCascade.h
MrcsLightgroup.cpp
MrcsLightgroup.h
common.h
)

ADD_EXECUTABLE(mrcs ${SOURCES})

TARGET_LINK_LIBRARIES(mrcs lightgen sampler tbbutils vlutil ${LIBS_gsl} ${LIBS_cblas})
