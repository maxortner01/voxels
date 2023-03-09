#pragma once

#include "livre/header.h"

#include "Shader.h"

namespace livre
{
    class DLLOUT Pipeline : public Graphics::InstanceObject
    {
    public:
        enum STATUS
        {
            SUCCESS,
            SHADER_NOT_COMPLETE
        };

    protected:
        // Programmable stages
        Shader** shaders;
        size_t shader_count;

        virtual void _initShaders() = 0;

    public:
        Pipeline(const Graphics::RenderInstance& instance);
        virtual ~Pipeline();

        Shader* getShader(const Shader::TYPE& type);

        virtual STATUS bind() = 0;
    };
}