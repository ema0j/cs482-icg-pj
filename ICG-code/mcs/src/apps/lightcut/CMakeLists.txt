# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
main.cpp
)

ADD_EXECUTABLE(lightcut ${SOURCES})

TARGET_LINK_LIBRARIES(lightcut lighttree lightgen sampler lightcutter)
