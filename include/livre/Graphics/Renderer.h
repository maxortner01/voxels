#pragma once

#include "livre/header.h"

#include "livre/Graphics/InstanceObject.h"
#include "livre/Graphics/GraphicsPipeline.h"

#include "Drawable.h"

namespace livre
{
    class DLLOUT Renderer : public Graphics::InstanceObject
    {
    public:
        struct Frame
        {
            void* commandBuffer;
            void* imageAvailableSemaphore;
            void* renderFinishedSemaphore;
            void* inFlightFence;
        };

        struct CreateInfo
        {
            uint32_t framesInFlight = 2;
        };

    private:
        const uint32_t framesInFlight;
        uint32_t currentFrame;
        uint32_t imageIndex;

        Color _clearColor;

        void* _renderPass;
        void* _swapChainFramebuffers;
        Frame* frames;

        void _createRenderPass();
        void _createFramebuffers();

    public:
        Renderer(const CreateInfo& info, const Graphics::RenderInstance& instance);
        virtual ~Renderer();

        Color getClearColor() const;
        void  setClearColor(const Color& _color);

        uint32_t getImageIndex() const; 
        const Frame& getCurrentFrame() const;

        const void* getRenderPass() const;

        void startFrame();
        void draw(const Graphics::Drawable& object, const GraphicsPipeline& _pipeline) const;
        void endFrame();

    };
}