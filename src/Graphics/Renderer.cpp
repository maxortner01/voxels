#include "livre/livre.h"

namespace livre
{

    void Renderer::_createRenderPass()
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        TRACE_LOG("Creating render pass.");

        const VkFormat _format = *(const VkFormat*)_instance.getSwapChainImages().format;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = _format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkRenderPass renderPass;

        VkResult result = vkCreateRenderPass((VkDevice)_instance.getLogicalDevice(), &renderPassInfo, nullptr, &renderPass);

        if (result != VK_SUCCESS)
#   ifdef LIVRE_LOGGING
        { ERROR_LOG("Error creating render pass."); return; }
#   else
            return;
#   endif

        _renderPass = renderPass;

        INFO_LOG("Render pass created successfully.");

        return;
    }

    void Renderer::_createFramebuffers() 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif

        VkExtent2D extent = *((VkExtent2D*)_instance.getSwapChainImages().extent);

        _swapChainFramebuffers = LIVRE_ALLOC(sizeof(VkFramebuffer) * _instance.getSwapChainImages().imageCount);

        for (uint32_t i = 0; i < _instance.getSwapChainImages().imageCount; i++) {
            TRACE_LOG("Creating swapchain framebuffer {}.", i);

            VkImageView attachments[] = {
                *((VkImageView*)(_instance.getSwapChainImages().imageViews) + i)
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = (VkRenderPass)_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            VkResult result = vkCreateFramebuffer((VkDevice)_instance.getLogicalDevice(), &framebufferInfo, nullptr, ((VkFramebuffer*)_swapChainFramebuffers) + i);
            if (result != VK_SUCCESS)
#       ifdef LIVRE_LOGGING
            { ERROR_LOG("Framebuffer {} create failed!", i); return; }
#       else
                return;
#       endif
        }

        INFO_LOG("Created swapchain framebuffers successfully!");
        
        return;
    }

    Renderer::Renderer(const Renderer::CreateInfo& info, const Graphics::RenderInstance& instance) :
        InstanceObject(instance), framesInFlight(info.framesInFlight), _renderPass(nullptr), frames(nullptr), currentFrame(0), _clearColor(0, 0, 0)
    {  
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif

        _createRenderPass();
        _createFramebuffers();

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

        if (_swapChainFramebuffers)
        {
            for (uint32_t i = 0; i < getInstance().getSwapChainImages().imageCount; i++)
            {
                TRACE_LOG("Destroying swapchain framebuffer {}...", i);
                vkDestroyFramebuffer((VkDevice)getInstance().getLogicalDevice(), *(((VkFramebuffer*)_swapChainFramebuffers) + i), nullptr);
                TRACE_LOG("...done");
            }

            std::free(_swapChainFramebuffers);
            _swapChainFramebuffers = nullptr;
        }

        if (_renderPass)
        {
            TRACE_LOG("Destroying render pass...");
            vkDestroyRenderPass((VkDevice)getInstance().getLogicalDevice(), (VkRenderPass)_renderPass, nullptr);
            TRACE_LOG("...done");
            _renderPass = nullptr;
        }

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

    Color Renderer::getClearColor() const
    { return _clearColor; }

    void Renderer::setClearColor(const Color& color)
    { _clearColor = color; }

    uint32_t Renderer::getImageIndex() const
    { return imageIndex; }

    const Renderer::Frame& Renderer::getCurrentFrame() const
    { return frames[currentFrame]; }

    const void* Renderer::getRenderPass() const
    {
        return _renderPass;
    }

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
        VkFramebuffer framebuffer = *(((VkFramebuffer*)_swapChainFramebuffers) + imageIndex);

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

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_extent.width);
        viewport.height = static_cast<float>(_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport((VkCommandBuffer)frames[currentFrame].commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _extent;
        vkCmdSetScissor((VkCommandBuffer)frames[currentFrame].commandBuffer, 0, 1, &scissor);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = (VkRenderPass)_renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _extent;

        VkClearValue clearColor = {{{_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass((VkCommandBuffer)frames[currentFrame].commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
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

        vkCmdEndRenderPass((VkCommandBuffer)frames[currentFrame].commandBuffer);

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