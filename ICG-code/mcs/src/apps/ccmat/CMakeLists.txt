# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
main.cpp
)

ADD_EXECUTABLE(ccmat ${SOURCES})

TARGET_LINK_LIBRARIES(ccmat lightgen sampler cmatrix gpshoot)
