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
    Shader::Shader(const Graphics::RenderInstance& instance, const TYPE& type, const std::string& title) :
        InstanceObject(instance), _type(type), _module(nullptr), _title(title), _complete(false)
    {   }

    Shader::~Shader()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        if (_module)
        {
            TRACE_LOG("Destroying shader '{}'...", _title);
            vkDestroyShaderModule((const VkDevice)_instance.getLogicalDevice(), (VkShaderModule)_module, nullptr);
            TRACE_LOG("...done");
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

        _title = filename;

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

        _title = filename;

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

        _title = filename;

        std::ifstream file(filename, std::ios::ate | std::ios::binary);

#   ifndef LIVRE_LOGGING
        if (!file) return STATUS::FILE_DOESNT_EXIST;
#   else
        if (!file) { logger->error("File '{}' does not exist!", filename); return STATUS::FILE_DOESNT_EXIST;}
#   endif

        size_t fileSize = (size_t) file.tellg();
        uint32_t* contents  = (uint32_t*)LIVRE_ALLOC(fileSize);

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
        INFO_LOG("Shader module ({}) created successfully.", _title);

        _complete = true;

        return SUCCESS;
    }

    bool Shader::isComplete() const
    { return _complete; }

    void* Shader::getModule() const 
    { return _module; }

    Shader::TYPE Shader::getType() const
    { return _type; }

    void Shader::getShaderStageInfo(void* info) const
    {
        VkShaderStageFlagBits type;
        if      (_type == VERTEX)   type = VK_SHADER_STAGE_VERTEX_BIT;
        else if (_type == FRAGMENT) type = VK_SHADER_STAGE_FRAGMENT_BIT;
        else if (_type == COMPUTE)  type = VK_SHADER_STAGE_COMPUTE_BIT;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = type;
        
        vertShaderStageInfo.module = (VkShaderModule)_module;
        vertShaderStageInfo.pName  = "main";

        std::memcpy(info, &vertShaderStageInfo, sizeof(VkPipelineShaderStageCreateInfo));
    }
}