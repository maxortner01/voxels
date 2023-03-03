cmake_minimum_required(VERSION 3.13)

project(voxels)

IF (WIN32)
    add_compile_definitions(VOXEL_BUILD)
ENDIF()

include_directories(voxels ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/extern/glew/include ${CMAKE_SOURCE_DIR}/extern/glfw/include)

add_definitions(-DGLEW_STATIC)

file(GLOB src "*.cpp")

add_library(voxels SHARED ${src} "${CMAKE_SOURCE_DIR}/extern/glew/src/glew.c")

target_link_libraries(voxels opengl32 glfw)
