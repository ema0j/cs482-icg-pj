# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
main.cpp
)

ADD_EXECUTABLE(mlightcut ${SOURCES})

TARGET_LINK_LIBRARIES(mlightcut lighttree lightgen sampler lightcutter)
