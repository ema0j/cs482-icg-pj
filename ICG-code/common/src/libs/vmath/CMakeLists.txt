# AUX_SOURCE_DIRECTORY(. SOURCES)
# AUX_SOURCE_DIRECTORY(./mt SOURCES_mt)

SET(SOURCES
consts.h
cone.h
fastcone.h
frame.h
frustum.h
functions.h
interval.h
matrix2.h
matrix3.h
matrix4.h
plane.h
projection.h
range1.h
range2.h
range3.h
range6.h
range.cpp
spline.h
streamMethods.h
uspline.h
vec2.h
vec3.h
vec4.h
vec6.h
vfunctions.h
random.h
tuple2.h
tuple3.h
spectrum3.h
geom3.h
geomrange3.h
linearmap3.h
vmath.cpp
)

ADD_LIBRARY(vmath ${SOURCES} ${SOURCES_mt})