#pragma once

namespace livre
{
namespace Graphics
{
    class RenderInstance
    {
        // Vulkan handles
        void* instance;
        void* debugMessenger;
        void* logicalDevice;
        void* physicalDevice;
        void* surface;
        void* presentationQueue;
        void* graphicsQueue;

        bool _validationSupport() const;
        void _initDebugMessenger();
        void _pickPhysicalDevice();
        void _initLogicalDevice();
        void _initSurface(void* window);

    public:
        RenderInstance(void* window, const std::string& title = "vulkan");
        ~RenderInstance();
    };
}
}