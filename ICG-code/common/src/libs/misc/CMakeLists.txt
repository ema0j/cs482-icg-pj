# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
shortString.h
queue.h
compilerOptions.h
properties.h
properties.cpp
stringUtils.h
threadUtils.h
misc.cpp
timer.h
report.h
mtreport.h
stats.h
arrays.h
stdcommon.h
console.h
console.cpp
)

ADD_LIBRARY(misc ${SOURCES})

TARGET_LINK_LIBRARIES(misc tbbutils)


