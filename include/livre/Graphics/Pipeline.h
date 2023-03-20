#pragma once

#include "livre/header.h"

#include "Shader.h"

namespace livre
{
    class Renderer;

    class DLLOUT Pipeline 
    {
    public:
        enum STATUS
        {
            SUCCESS,
            SHADER_NOT_COMPLETE,
            PIPELINE_LAYOUT_CREATE_FAILED,
            RENDER_PASS_CREATE_FAILED,
            PIPELINE_CREATE_FAILED,
            FRAMEBUFFER_CREATE_FAILED,
            COMMAND_BUFFER_CREATE_FAILED
        };

    protected:
        // Programmable stages
        Shader** shaders;
        size_t shader_count;

        void* _pipelineLayout;
        void* _pipeline;

        virtual void _initShaders() = 0;

        const Renderer& _renderer;

    public:
        Pipeline(const Renderer& renderer);
        virtual ~Pipeline();

        const void* getPipeline() const;

        bool isComplete() const;

        Shader* getShader(const Shader::TYPE& type);

        virtual STATUS create() = 0;
    };
}