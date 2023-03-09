#pragma once

#include "livre/header.h"

#include "InstanceObject.h"

namespace livre
{
    /**
     * @brief A singular shader object.
     * 
     * This object represents one shader to be compiled. Contains methods to 
     * read files and compile text (assuming NO_SHADERC isn't defined) glsl
     * into a Vulkan shader module.
     */
    class DLLOUT Shader : public Graphics::InstanceObject
    {
    public:
        enum TYPE
        {
            NONE, 
            VERTEX,
            FRAGMENT,
            COMPUTE
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
        std::string _title;

        TYPE _type;
        void* _module;

        bool _complete;

    public:
        Shader(const Graphics::RenderInstance& instance, const TYPE& type, const std::string& title = "shader.glsl");
        ~Shader();
        
        STATUS fromFileAsGLSL(const std::string& filename);
        STATUS fromStringAsGLSL(const std::string& contents, const std::string& filename = "shader.glsl");

        STATUS fromFileAsSPIR(const std::string& filename);
        STATUS fromSPIR(const uint32_t* contents, const size_t& size);

        bool isComplete() const;
        void* getModule() const;
        TYPE  getType() const;

        void getShaderStageInfo(void* info) const;
    };
}