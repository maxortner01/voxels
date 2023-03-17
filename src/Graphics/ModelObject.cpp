#include "livre/livre.h"

namespace livre
{
    void ModelObject::draw(const Renderer& _renderer, const GraphicsPipeline& _pipeline) const 
    {
        VkExtent2D _extent = *((VkExtent2D*)_renderer.getInstance().getSwapChainImages().extent);
        VkFramebuffer framebuffer = *(((VkFramebuffer*)_pipeline.getFramebuffers()) + _renderer.getImageIndex());

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = (VkRenderPass)_pipeline.getRenderPass();
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _extent;
        
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (const VkPipeline)_pipeline.getPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_extent.width);
        viewport.height = static_cast<float>(_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _extent;
        vkCmdSetScissor((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 0, 1, &scissor);

        vkCmdDraw((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer);
    }

/*
    void ModelObject::_recordCommandBuffer(const GraphicsPipeline& _pipeline, const uint32_t& imageIndex) const
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif 

        VkExtent2D _extent = *((VkExtent2D*)_pipeline.getInstance().getSwapChainImages().extent);
        VkFramebuffer framebuffer = *(((VkFramebuffer*)_pipeline.getFramebuffers()) + imageIndex);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VkResult result = vkBeginCommandBuffer((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, &beginInfo);

        if (result != VK_SUCCESS)
        {
            ERROR_LOG("Error starting command buffer recording!");
            return;
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = (VkRenderPass)_pipeline.getRenderPass();
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = _extent;
        
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (const VkPipeline)_pipeline.getPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_extent.width);
        viewport.height = static_cast<float>(_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _extent;
        vkCmdSetScissor((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 0, 1, &scissor);

        vkCmdDraw((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer);

        result = vkEndCommandBuffer((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer);

#   ifdef LIVRE_LOGGING
        if (result != VK_SUCCESS) ERROR_LOG("Error ending command buffer!");
#   endif
    }*/

/*
    void ModelObject::draw(const GraphicsPipeline& _pipeline) const 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("vulkan");
#   endif 

        VkDevice       logicalDevice  = (VkDevice)_pipeline.getInstance().getLogicalDevice();
        VkFence        inFlightFence  = (VkFence)_pipeline.getInstance().getInFlightFence();
        VkSwapchainKHR swapChain      = (VkSwapchainKHR)_pipeline.getInstance().getSwapChain();
        VkSemaphore    imageAvaliable = (VkSemaphore)_pipeline.getInstance().getImageAvaliableSemaphore();
        VkSemaphore    renderFinished = (VkSemaphore)_pipeline.getInstance().getRenderFinishedSemaphire();

        vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &inFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvaliable, VK_NULL_HANDLE, &imageIndex);

        //vkResetCommandBuffer((VkCommandBuffer)_renderer.getCurrentFrame().commandBuffer, 0);
        //_recordCommandBuffer(_pipeline, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvaliable};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        //submitInfo.pCommandBuffers = (VkCommandBuffer*)(&_renderer.getCurrentFrame().commandBuffer);

        VkSemaphore signalSemaphores[] = {renderFinished};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult result = vkQueueSubmit((VkQueue)_pipeline.getInstance().getGraphicsQueue(), 1, &submitInfo, inFlightFence);
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
        vkQueuePresentKHR((VkQueue)_pipeline.getInstance().getPresentQueue(), &presentInfo);
    }*/
}