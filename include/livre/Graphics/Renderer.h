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

        Frame* frames;

    public:
        Renderer(const CreateInfo& info, const Graphics::RenderInstance& instance);
        virtual ~Renderer();

        uint32_t getImageIndex() const; 
        const Frame& getCurrentFrame() const;

        void startFrame();
        void draw(const Graphics::Drawable& object, const GraphicsPipeline& _pipeline) const;
        void endFrame();

    };
}