#pragma once

#include "livre/header.h"

#include "Pipeline.h"
#include "BufferArray.h"

namespace livre
{
    class Renderer;

    /**
     * @brief Interface to rendering with Vulkan.
     * 
     * Contains RenderInstance object.
     */
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

        const BufferArray* _bufferArray;

    protected:
        void _initShaders() override;

        STATUS _createLayouts();

    public:
        GraphicsPipeline(const Renderer& renderer, const BufferArray* bufferArray);
        ~GraphicsPipeline();

        void setMode(const MODE& mode);
        MODE getMode() const;

        Shader& getVertexShader();
        Shader& getFragmentShader();

        STATUS create() override;
    };
}