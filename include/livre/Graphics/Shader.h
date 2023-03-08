#pragma once

#include "livre/header.h"

#include "RenderInstance.h"

namespace livre
{
    class DLLOUT Shader
    {
    public:
        enum TYPE
        {
            NONE, 
            VERTEX,
            FRAGMENT
        };

        enum STATUS
        {
            SUCCESS,
            FILE_DOESNT_EXIST,
            COMPILATION_ERROR,
            SHADERC_NOT_LOADED,
            VULKAN_ERROR
        };

    private:
        TYPE _type;
        void* _module;

        const Graphics::RenderInstance& _instance;

    public:
        Shader(const Graphics::RenderInstance& instance, const TYPE& type);
        ~Shader();

        STATUS fromFileAsGLSL(const std::string& filename);
        STATUS fromStringAsGLSL(const std::string& contents, const std::string& filename = "shader.glsl");

        STATUS fromFileAsSPIR(const std::string& filename);
        STATUS fromSPIR(const uint32_t* contents, const size_t& size);
    };
}