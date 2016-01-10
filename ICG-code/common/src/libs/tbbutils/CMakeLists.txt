# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
tbbutils.h
tbbtimer.h
tbbutils.cpp
)

ADD_LIBRARY(tbbutils ${SOURCES})

TARGET_LINK_LIBRARIES(tbbutils optimized ${LIBS_tbb} debug ${LIBS_tbb_debug} optimized ${LIBS_tbbmalloc_proxy} debug ${LIBS_tbbmalloc_proxy_debug} )
