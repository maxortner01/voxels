#pragma once

#include "livre/header.h"

#include "Pipeline.h"

namespace livre
{
    class GraphicsPipeline : public Pipeline
    {
    public:
        enum MODE
        {
            POINTS,
            WIREFRAME,
            FILL
        };

    private:
        MODE _mode;
        void* _renderPass;
        void* _swapChainFramebuffers;
        void* _commandPool;

    protected:
        void _initShaders() override;

        STATUS _createPipeline();
        STATUS _createLayouts();
        STATUS _createRenderPass();
        STATUS _createFramebuffers();

    public:
        GraphicsPipeline(const Graphics::RenderInstance& instance);
        ~GraphicsPipeline();

        void setMode(const MODE& mode);
        MODE getMode() const;

        Shader& getVertexShader();
        Shader& getFragmentShader();

        STATUS create() override;
    };
}