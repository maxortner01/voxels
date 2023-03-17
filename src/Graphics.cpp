#include <GL/glew.h>
#include <GLFW/glfw3.h>

// used in render instance... for now
#include <optional>
#include <iterator>

#define LIVRE_ALLOC(Args...) std::malloc(Args)

#ifdef LIVRE_LOGGING
#   include <vulkan/vk_enum_string_helper.h>
#   include <spdlog/spdlog.h>
#   include "spdlog/sinks/stdout_color_sinks.h"
#   define TRACE_LOG(Args...) logger->trace(Args)
#   define INFO_LOG(Args...) logger->info(Args)
#   define WARN_LOG(Args...) logger->warn(Args)
#   define ERROR_LOG(Args...) logger->error(Args)
#else
#   define TRACE_LOG(Args...)
#   define INFO_LOG(Args...) 
#   define WARN_LOG(Args...)
#   define ERROR_LOG(Args...)
#endif

#include "./Graphics/Window.cpp"
#include "./Graphics/Shader.cpp"
#include "./Graphics/RenderInstance.cpp"
#include "./Graphics/Pipeline.cpp"
#include "./Graphics/GraphicsPipeline.cpp"
#include "./Graphics/ModelObject.cpp"
#include "./Graphics/Renderer.cpp"