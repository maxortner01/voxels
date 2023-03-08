cmake_minimum_required(VERSION 3.13)

project(voxels)

IF (WIN32)
    add_compile_definitions(VOXEL_BUILD)
ENDIF()

include_directories(voxels ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/extern/glew/include ${CMAKE_SOURCE_DIR}/extern/spdlog/include ${CMAKE_SOURCE_DIR}/extern/glfw/include)

if (APPLE)
  set(ENV{VK_ICD_FILENAMES} /Users/maxortner/VulkanSDK/1.3.239.0/macOS/share/vulkan/icd.d/MoltenVK_icd.json)
  set(ENV{VK_LAYER_PATH} /Users/maxortner/VulkanSDK/1.3.239.0/macOS/share/vulkan/explicit_layer.d)
  include_directories(voxels /usr/local/include $ENV{HOME}/VulkanSDK/macOS/include)
  link_directories(/usr/local/lib $ENV{HOME}/VulkanSDK/macOS/lib)
endif()

link_directories(${CMAKE_SOURCE_DIR}/build/extern/glew/lib ${CMAKE_SOURCE_DIR}/lib/)

add_definitions(-DGLEW_STATIC -DLIVRE_LOGGING -DGLFW_INCLUDE_VULKAN)
add_compile_options(-std=c++17 -Wc++11-extensions -Wswitch)

file(GLOB src "*.cpp")

add_library(voxels SHARED ${src})

target_link_libraries(voxels libglew.a libglfw3.a)

if (APPLE)
  target_link_libraries(voxels "-framework Cocoa -framework OpenGL -framework IOKit" libvulkan.1.3.239.dylib libvulkan.1.dylib libshaderc.a libshaderc_combined.a)
endif()
