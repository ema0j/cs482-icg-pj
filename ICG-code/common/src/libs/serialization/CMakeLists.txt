# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
archive.h
archive.cpp
archiveRegistry.h
xmloarchive.h
xmliarchive.h
textoarchive.h
textiarchive.h
binoarchive.h
biniarchive.h
archivehelper.h
archivehelper.cpp
)

ADD_LIBRARY(serialization ${SOURCES})

TARGET_LINK_LIBRARIES(serialization tinyxml)