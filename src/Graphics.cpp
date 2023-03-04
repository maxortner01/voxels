#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef LIVRE_LOGGING
#   include <spdlog/spdlog.h>
#   include "spdlog/sinks/stdout_color_sinks.h"
#endif

#include "./Graphics/Window.cpp"
#include "./Graphics/Shader.cpp"
#include "./Graphics/BufferObject.cpp"
#include "./Graphics/VertexObject.cpp"