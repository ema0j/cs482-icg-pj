# AUX_SOURCE_DIRECTORY(. SOURCES)

SET(SOURCES
intersection.h
rayEngine.cpp
rayEngine.h
rayPrimitive.h
rayPrimitive.cpp
rayIntersectionOp.h
rayTesselatedEngine.cpp
rayTesselatedEngine.h
rayTesselatedPrimitive.h
rayTesselatedPrimitive.cpp
rayTesselationCache.h
rayTesselationCache.cpp
rayTesselatedKdTreeEngine.cpp
rayTesselatedKdTreeEngine.h
rayDoubleSidedEngine.h
rayDoubleSidedEngine.cpp
rayListEngine.h
rayListEngine.cpp
rayListEngineData.h
rayListEngineBuilder.h
rayListEngineBuilder.cpp
rayBVHEngine.h
rayBVHEngine.cpp
rayBVHEngineData.h
rayBVHEngineData.cpp
rayBVHEngineBuilder.h
rayBVHEngineBuilder.cpp
)

ADD_LIBRARY(ray ${SOURCES})
