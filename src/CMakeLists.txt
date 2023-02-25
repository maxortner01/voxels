cmake_minimum_required(VERSION 3.13)

project(voxels)

include_directories(voxels ${CMAKE_SOURCE_DIR}/include)

add_executable(voxels main.cpp)