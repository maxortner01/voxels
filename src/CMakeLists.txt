cmake_minimum_required(VERSION 3.13)

project(voxels)

IF (WIN32)
    add_compile_definitions(VOXEL_BUILD)
ENDIF()

include_directories(voxels ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/extern/glew/include ${CMAKE_SOURCE_DIR}/extern/spdlog/include ${CMAKE_SOURCE_DIR}/extern/glfw/include)

link_directories(${CMAKE_SOURCE_DIR}/build/extern/glew/lib ${CMAKE_SOURCE_DIR}/lib/)

add_definitions(-DGLEW_STATIC -DLIVRE_LOGGING)
add_compile_options(-std=c++0x -Wc++11-extensions -Wswitch)

file(GLOB src "*.cpp")

add_library(voxels SHARED ${src})

target_link_libraries(voxels libglew.a libglfw3.a)

if (APPLE)
  target_link_libraries(voxels "-framework Cocoa -framework OpenGL -framework IOKit")
endif()
