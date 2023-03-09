#pragma once

#include "livre/header.h"

#include "Pipeline.h"

namespace livre
{
    class GraphicsPipeline : public Pipeline
    {
    protected:
        void _initShaders() override;

    public:
        Shader& getVertexShader();
        Shader& getFragmentShader();

        STATUS bind() override;
    };
}