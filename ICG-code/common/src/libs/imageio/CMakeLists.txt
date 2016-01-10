# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
imageio.cpp
imageio.h
)

ADD_LIBRARY(imageio ${SOURCES})

TARGET_LINK_LIBRARIES(imageio image)
TARGET_LINK_LIBRARIES(imageio ${LIBS_freeimage})