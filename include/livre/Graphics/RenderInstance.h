#pragma once

namespace livre
{
namespace Graphics
{
    class RenderInstance
    {
    public:
        struct SwapChainImages
        {
            void* images;
            void* imageViews;

            uint32_t imageCount;
            void* format;
            void* extent;
        };

    private:
        // Vulkan handles
        void* instance;
        void* debugMessenger;
        void* logicalDevice;
        void* physicalDevice;
        void* surface;
        void* presentationQueue;
        void* graphicsQueue;
        void* swapChain;

        SwapChainImages* _swapChainImages;

        bool _validationSupport() const;
        void _initDebugMessenger();
        void _pickPhysicalDevice();
        void _initLogicalDevice();
        void _initSurface(void* window);
        void _createSwapChain(void* window);
        void _createImageViews();

    public:
        RenderInstance(void* window, const std::string& title = "vulkan");
        ~RenderInstance();
    };
}
}