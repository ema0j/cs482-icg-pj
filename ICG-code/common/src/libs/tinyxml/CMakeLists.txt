# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
tinystr.cpp
tinystr.h
tinyxml.cpp
tinyxml.h
tinyxmlerror.cpp
tinyxmlparser.cpp
)

ADD_LIBRARY(tinyxml ${SOURCES})

TARGET_LINK_LIBRARIES(tinyxml)