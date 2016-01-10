# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
GatherPointShooter.h
GatherPointShooter.cpp
)

ADD_LIBRARY(gpshoot ${SOURCES})

TARGET_LINK_LIBRARIES(gpshoot scene vmath tbbutils)
