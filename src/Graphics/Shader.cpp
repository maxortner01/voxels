#include "livre/livre.h"

#include <fstream>
#include <cstring>
#include <bitset>
#include <cmath>

#ifndef NO_SHADERC
#   include <shaderc/shaderc.hpp>
#endif

namespace livre
{
    Shader::Shader(const Graphics::RenderInstance& instance, const TYPE& type) :
        _type(type), _instance(instance), _module(nullptr)
    {   }

    Shader::~Shader()
    {
        if (_module)
        {
            vkDestroyShaderModule((const VkDevice)_instance.getLogicalDevice(), (VkShaderModule)_module, nullptr);
            _module = nullptr;
        }
    }

#ifdef NO_SHADERC
    Shader::STATUS Shader::fromFileAsGLSL(const std::string& filename) 
    { 
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif
        WARN_LOG("Shaderc not loaded.");

        return STATUS::SHADERC_NOT_LOADED; 
    }
    Shader::STATUS Shader::fromStringAsGLSL(const std::string& filename) 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif
        WARN_LOG("Shaderc not loaded.");

        return STATUS::SHADERC_NOT_LOADED; 
    }

#else
    Shader::STATUS Shader::fromFileAsGLSL(const std::string& filename)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        std::ifstream file(filename);

#   ifndef LIVRE_LOGGING
        if (!file) return STATUS::FILE_DOESNT_EXIST;
#   else
        if (!file) { logger->error("File '{}' does not exist!", filename); return STATUS::FILE_DOESNT_EXIST;}
#   endif

        std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()    ) );

        return fromStringAsGLSL(content, filename);
    }

    Shader::STATUS Shader::fromStringAsGLSL(const std::string& contents, const std::string& filename)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;

        shaderc_shader_kind kind;
        if      (_type == TYPE::VERTEX)   kind = shaderc_glsl_vertex_shader;
        else if (_type == TYPE::FRAGMENT) kind = shaderc_glsl_fragment_shader;

        options.SetOptimizationLevel(shaderc_optimization_level_size);
        shaderc::PreprocessedSourceCompilationResult pp = compiler.PreprocessGlsl(contents, kind, filename.c_str(), options);
        shaderc::SpvCompilationResult compiling = compiler.CompileGlslToSpv(contents, kind, filename.c_str(), options);
        
        if (compiling.GetNumErrors() > 0)
        {
            ERROR_LOG("Error compiling shader:\n{}", compiling.GetErrorMessage());
            return COMPILATION_ERROR;
        }

        if (filename != "shader.glsl")
            INFO_LOG("Shader '{}' compiled successfully.", filename);
        else
            INFO_LOG("Successfully compiled shader!");

        return fromSPIR(compiling.begin(), std::distance(compiling.begin(), compiling.end()) * sizeof(uint32_t));
    }
#endif

    Shader::STATUS Shader::fromFileAsSPIR(const std::string& filename)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        std::ifstream file(filename, std::ios::ate | std::ios::binary);

#   ifndef LIVRE_LOGGING
        if (!file) return STATUS::FILE_DOESNT_EXIST;
#   else
        if (!file) { logger->error("File '{}' does not exist!", filename); return STATUS::FILE_DOESNT_EXIST;}
#   endif

        size_t fileSize = (size_t) file.tellg();
        uint32_t* contents  = (uint32_t*)std::malloc(fileSize);

        file.seekg(0);
        file.read((char*)contents, fileSize);
        file.close();

        STATUS status = fromSPIR(contents, fileSize);

        std::free(contents);
        return status;
    }

    Shader::STATUS Shader::fromSPIR(const uint32_t* contents, const size_t& size)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif
        
        TRACE_LOG("Creating shader module ({} bytes)...", size);
        VkShaderModule module;
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = contents;

        VkResult result = vkCreateShaderModule((const VkDevice)_instance.getLogicalDevice(), &createInfo, nullptr, &module);
        TRACE_LOG("...done");

        if (result != VK_SUCCESS)
        {
            ERROR_LOG("Error creating shader module!");
            return VULKAN_ERROR;
        }

        _module = module;
        INFO_LOG("Shader module created successfully.");

        return SUCCESS;
    }
}