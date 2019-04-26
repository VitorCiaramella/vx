#include <iostream>
#include <vector>
#include <string>

#include <vxGraphics/vxGraphics.hpp>
#include "internals.hpp"

using namespace std;

VkImageMemoryBarrier imageBarrier(VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier result = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };

	result.srcAccessMask = srcAccessMask;
	result.dstAccessMask = dstAccessMask;
	result.oldLayout = oldLayout;
	result.newLayout = newLayout;
	result.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	result.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	result.image = image;
	result.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	result.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	result.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	return result;
}

VxWindowLoopResult windowLoop(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    if (upGraphicsInstance->acquireSemaphore == nullptr)
    {
        return VxWindowLoopResult::VX_WL_STOP;
    }
    auto commandBuffer = upGraphicsInstance->vkCommandBuffer;
    auto device = upGraphicsInstance->vxDevices[0].vkDevice;
    auto swapchain = upGraphicsInstance->vxMainSwapchain.vkSwapchain;
    auto swapchainFramebuffers = upGraphicsInstance->vxMainSwapchain.vkFramebuffers;
    auto swapchainImages = upGraphicsInstance->vxMainSwapchain.vkImages;    
    auto commandPool = upGraphicsInstance->vxDevices[0].vxQueueFamilies[0].vkCommandPool; 
    auto queue = upGraphicsInstance->vxDevices[0].vxQueues[0].vkQueue;
    auto renderPass = upGraphicsInstance->vkRenderPass;
    auto windowSize = upGraphicsInstance->windowSize;
    auto acquireSemaphore = upGraphicsInstance->acquireSemaphore;
    auto releaseSemaphore = upGraphicsInstance->releaseSemaphore;
    auto trianglePipeline = upGraphicsInstance->vxPipeline.vkPipeline;

    uint32_t imageIndex = 0;
    AssertVkResultVxWindowLoop(vkAcquireNextImageKHR, device, swapchain, ~0ull, acquireSemaphore, VK_NULL_HANDLE, &imageIndex);

    AssertVkResultVxWindowLoop(vkResetCommandPool, device, commandPool, 0);

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    AssertVkResultVxWindowLoop(vkBeginCommandBuffer, commandBuffer, &beginInfo);

    VkImageMemoryBarrier renderBeginBarrier = imageBarrier(swapchainImages[imageIndex], 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &renderBeginBarrier);

    VkClearColorValue color = { 90.f / 255.f, 10.f / 255.f, 36.f / 255.f, 1 };
    VkClearValue clearColor = { color };

    VkRenderPassBeginInfo passBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    passBeginInfo.renderPass = renderPass;
    passBeginInfo.framebuffer = swapchainFramebuffers[imageIndex];
    passBeginInfo.renderArea.extent = windowSize;
    passBeginInfo.clearValueCount = 1;
    passBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = { 0, 0, float(windowSize.width), float(windowSize.height), 0, 1 };
    VkRect2D scissor = { {0, 0}, {windowSize.width, windowSize.height} };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkImageSubresourceRange range = {};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.layerCount = 1;
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    VkImageMemoryBarrier renderEndBarrier = imageBarrier(swapchainImages[imageIndex], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &renderEndBarrier);

    AssertVkResultVxWindowLoop(vkEndCommandBuffer, commandBuffer);

    VkPipelineStageFlags submitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &acquireSemaphore;
    submitInfo.pWaitDstStageMask = &submitStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &releaseSemaphore;
    AssertVkResultVxWindowLoop(vkQueueSubmit, queue, 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &releaseSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;
    AssertVkResultVxWindowLoop(vkQueuePresentKHR, queue, &presentInfo);

    AssertVkResultVxWindowLoop(vkDeviceWaitIdle, device);

    return VxWindowLoopResult::VX_WL_CONTINUE;
}

int main()
{
    VxResult vxresult;

    VxGraphicsInstanceCreateInfo graphicsInstanceCreateInfo = {};
    graphicsInstanceCreateInfo.applicationName = "vxGraphics";
    graphicsInstanceCreateInfo.applicationVersion = VX_MAKE_VERSION(0,1,0);
    graphicsInstanceCreateInfo.engineName = "vxGraphics";
    graphicsInstanceCreateInfo.engineVersion = VX_MAKE_VERSION(0,1,0);
    graphicsInstanceCreateInfo.apiVersion = VK_API_VERSION_1_1;

    graphicsInstanceCreateInfo.mainWindowWidth = 1024;
    graphicsInstanceCreateInfo.mainWindowHeight = 768;

    #ifdef _DEBUG
    //graphicsInstanceCreateInfo.desiredLayersToEnable.push_back("VK_LAYER_KHRONOS_validation");
    graphicsInstanceCreateInfo.desiredLayersToEnable.push_back("VK_LAYER_LUNARG_standard_validation");
    #endif

    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_8BIT_STORAGE_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_16BIT_STORAGE_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);   
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);

    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    #ifdef VK_USE_PLATFORM_MACOS_MVK
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);  
    #endif
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif
    #ifdef _DEBUG
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    #endif

    graphicsInstanceCreateInfo.desiredDeviceCount = 1;
    graphicsInstanceCreateInfo.desiredQueueCountPerDevice = 1;

	graphicsInstanceCreateInfo.shadersFilePaths.push_back("triangle.vert.spv");
	graphicsInstanceCreateInfo.shadersFilePaths.push_back("triangle.frag.spv");

    upt(VxGraphicsInstance) upGraphicsInstance;
    vxresult = vxCreateGraphicsInstance(&graphicsInstanceCreateInfo, upGraphicsInstance);
    if (vxresult != VxResult::VX_SUCCESS)
    {
        return -1;
    }

    upGraphicsInstance->vxWindowLoopFunction = &windowLoop;
    vxGraphicsRun(upGraphicsInstance);
    
    vxGraphicsDestroyInstance(upGraphicsInstance);
    vxGraphicsTerminate(upGraphicsInstance);

}

