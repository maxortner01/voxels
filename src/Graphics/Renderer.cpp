#include "livre/livre.h"

namespace livre
{
    Renderer::Renderer(const Renderer::CreateInfo& info, const Graphics::RenderInstance& instance) :
        InstanceObject(instance), framesInFlight(info.framesInFlight), frames(nullptr), currentFrame(0)
    {  
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif
        frames = (Frame*)std::malloc(sizeof(Frame) * framesInFlight);
        std::memset(frames, 0, sizeof(Frame) * framesInFlight);

        for (uint32_t i = 0; i < framesInFlight; i++)
        {
            TRACE_LOG("Making command buffer for renderer (frame {}).", i + 1);
            frames[i].commandBuffer = instance.makeCommandBuffer();

            TRACE_LOG("Making sync objects for renderer (frame {}).", i + 1);
            VkSemaphore _imageAvailableSemaphore;
            VkSemaphore _renderFinishedSemaphore;
            VkFence _inFlightFence;

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            if (vkCreateSemaphore((VkDevice)instance.getLogicalDevice(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
                vkCreateSemaphore((VkDevice)instance.getLogicalDevice(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS ||
                vkCreateFence((VkDevice)instance.getLogicalDevice(), &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS) {
                ERROR_LOG("Failed to create semaphores!"); return;
            }

            frames[i].imageAvailableSemaphore = _imageAvailableSemaphore;
            frames[i].renderFinishedSemaphore = _renderFinishedSemaphore;
            frames[i].inFlightFence = _inFlightFence;

            if (!frames[i].imageAvailableSemaphore || !frames[i].renderFinishedSemaphore || !frames[i].inFlightFence)
                ERROR_LOG("Error creating objects!");
            
            INFO_LOG("Frame {} created successfully.", i + 1);
        }
    }

    Renderer::~Renderer()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        TRACE_LOG("Destroying sync objects...");
        if (frames)
            for (uint32_t i = 0; i < framesInFlight; i++)
            {
                vkDestroySemaphore((VkDevice)_instance.getLogicalDevice(), (VkSemaphore)frames[i].imageAvailableSemaphore, nullptr);
                vkDestroySemaphore((VkDevice)_instance.getLogicalDevice(), (VkSemaphore)frames[i].renderFinishedSemaphore, nullptr);
                vkDestroyFence((VkDevice)_instance.getLogicalDevice(), (VkFence)frames[i].inFlightFence, nullptr);
            }
        TRACE_LOG("...done");

        std::free(frames);
        frames = nullptr;
    }

    uint32_t Renderer::getImageIndex() const
    { return imageIndex; }

    const Renderer::Frame& Renderer::getCurrentFrame() const
    { return frames[currentFrame]; }

    void Renderer::startFrame() 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif 

        VkDevice       logicalDevice  = (VkDevice)_instance.getLogicalDevice();
        VkSwapchainKHR swapChain      = (VkSwapchainKHR)_instance.getSwapChain();
        VkFence        inFlightFence  = (VkFence)frames[currentFrame].inFlightFence;
        VkSemaphore    imageAvaliable = (VkSemaphore)frames[currentFrame].imageAvailableSemaphore;

        vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &inFlightFence);

        vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvaliable, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer((VkCommandBuffer)frames[currentFrame].commandBuffer,  0);

        VkExtent2D _extent = *((VkExtent2D*)_instance.getSwapChainImages().extent);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VkResult result = vkBeginCommandBuffer((VkCommandBuffer)frames[currentFrame].commandBuffer, &beginInfo);

        if (result != VK_SUCCESS)
        {
            ERROR_LOG("Error starting command buffer recording!");
            return;
        }
    }

    void Renderer::draw(const Graphics::Drawable& object, const GraphicsPipeline& _pipeline) const
    {
        object.draw(*(const Renderer*)this, _pipeline);
    }

    void Renderer::endFrame()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        VkSwapchainKHR swapChain      = (VkSwapchainKHR)_instance.getSwapChain();
        VkFence        inFlightFence  = (VkFence)frames[currentFrame].inFlightFence;
        VkSemaphore    renderFinished = (VkSemaphore)frames[currentFrame].renderFinishedSemaphore;
        VkSemaphore    imageAvaliable = (VkSemaphore)frames[currentFrame].imageAvailableSemaphore;

        VkResult result = vkEndCommandBuffer((VkCommandBuffer)frames[currentFrame].commandBuffer);

#   ifdef LIVRE_LOGGING
        if (result != VK_SUCCESS) ERROR_LOG("Error ending command buffer!");
#   endif

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvaliable};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = (VkCommandBuffer*)&frames[currentFrame].commandBuffer;

        VkSemaphore signalSemaphores[] = {renderFinished};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        result = vkQueueSubmit((VkQueue)_instance.getGraphicsQueue(), 1, &submitInfo, inFlightFence);
        if (result != VK_SUCCESS)
        {
            ERROR_LOG("Error drawing command buffer!");
            return;
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = nullptr; // Optional
        vkQueuePresentKHR((VkQueue)_instance.getPresentQueue(), &presentInfo);

        currentFrame = (currentFrame + 1) % framesInFlight;
    }
}