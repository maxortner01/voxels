cmake_minimum_required(VERSION 3.13)

project(voxels)

include_directories(voxels ${CMAKE_SOURCE_DIR}/include)

file(GLOB src "*.cpp")

message(${src})

add_library(voxels SHARED ${src})