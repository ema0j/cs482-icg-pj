# setting up cmake
cmake_minimum_required(VERSION 2.6)

# main project name defined
project(manylight)

# setting up other projects
set(FAR_PATH ../../far/src)
set(COMMON_PATH ../../common/src)

# setting up macros
include(${COMMON_PATH}/build/macros.cmake)

# set default build to release
default_release()

# setting up project
output_directories_default()
compiler_flags_default()
proj_directories_default(${COMMON_PATH} ${FAR_PATH})

# include external libraries
include(${COMMON_PATH}/build/libs.cmake)

add_subdirectory(${COMMON_PATH}/libs/misc libs/misc)
add_subdirectory(${COMMON_PATH}/libs/vmath libs/vmath)
add_subdirectory(${COMMON_PATH}/libs/image libs/image)
add_subdirectory(${COMMON_PATH}/libs/imageio libs/imageio)
add_subdirectory(${COMMON_PATH}/libs/tinyxml libs/tinyxml)
add_subdirectory(${COMMON_PATH}/libs/serialization libs/serialization)
#add_subdirectory(${COMMON_PATH}/libs/glutils libs/glutils)
add_subdirectory(${COMMON_PATH}/libs/tbbutils libs/tbbutils)

add_subdirectory(${FAR_PATH}/libs/scene libs/scene)
add_subdirectory(${FAR_PATH}/libs/ray libs/ray)
add_subdirectory(${FAR_PATH}/libs/lightgen libs/lightgen)
add_subdirectory(${FAR_PATH}/libs/sampler libs/sampler)

add_subdirectory(apps/lightcut)
add_subdirectory(apps/mlightcut)
add_subdirectory(apps/ccmat)
add_subdirectory(apps/mrcs)

add_subdirectory(libs/lightcutter)
add_subdirectory(libs/lighttree)
add_subdirectory(libs/vlutil)
add_subdirectory(libs/nmatrix)
add_subdirectory(libs/gpshoot)

