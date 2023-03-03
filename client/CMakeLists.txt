cmake_minimum_required(VERSION 3.13)

project(client)

include_directories(client ${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_compile_options(-static-libstdc++)

add_executable(client main.cpp)

target_link_libraries(client libvoxels.dll.a)