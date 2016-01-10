# setting up cmake
cmake_minimum_required(VERSION 2.6)

# main project name defined
project(far)

# setting up other projects
set(COMMON_PATH ../../common/src)

# setting up macros
include(${COMMON_PATH}/build/macros.cmake)

# set default build to release
default_release()

# setting up project
output_directories_default()
compiler_flags_default()
proj_directories_default(${COMMON_PATH})

# include external libraries
include(${COMMON_PATH}/build/libs.cmake)

add_subdirectory(${COMMON_PATH}/libs/misc libs/misc)
add_subdirectory(${COMMON_PATH}/libs/vmath libs/vmath)
add_subdirectory(${COMMON_PATH}/libs/image libs/image)
add_subdirectory(${COMMON_PATH}/libs/imageio libs/imageio)
add_subdirectory(${COMMON_PATH}/libs/tinyxml libs/tinyxml)
add_subdirectory(${COMMON_PATH}/libs/serialization libs/serialization)
add_subdirectory(${COMMON_PATH}/libs/glutils libs/glutils)
#add_subdirectory(${COMMON_PATH}/libs/gl3utils libs/gl3utils)

add_subdirectory(libs/scene)
add_subdirectory(libs/ray)
add_subdirectory(libs/tracer)
add_subdirectory(libs/wavelet)
add_subdirectory(libs/sampler)

add_subdirectory(apps/ntrace)
#add_subdirectory(apps/glviewer)
#add_subdirectory(apps/exporter)
#add_subdirectory(apps/importer)

