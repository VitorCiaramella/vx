#include <vxApplication/vxApplication.hpp>
#include <vxGraphics/vxGraphics.hpp>

//To be removed
#include "../vxGraphics/macros.hpp"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

VkImageMemoryBarrier imageBarrier2(VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
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

VxResult vxCreateApplicationInstance(spt(VxApplicationInstanceCreateInfo) spCreateInfo, spt(VxApplicationInstance) & spVxApplicationInstance)
{
    spVxApplicationInstance = nsp<VxApplicationInstance>();
    spVxApplicationInstance->spCreateInfo = spCreateInfo;
    //spVxApplicationInstance->spVxGraphicsDebug = nsp<VxGraphicsDebug>(spVxGraphicsInstance);
    
    auto spGraphicsInstanceCreateInfo = nsp<VxGraphicsInstanceCreateInfo>();
    spGraphicsInstanceCreateInfo->applicationName = "vxGraphics";
    spGraphicsInstanceCreateInfo->applicationVersion = VX_MAKE_VERSION(0,1,0);
    spGraphicsInstanceCreateInfo->engineName = "vxGraphics";
    spGraphicsInstanceCreateInfo->engineVersion = VX_MAKE_VERSION(0,1,0);
    spGraphicsInstanceCreateInfo->apiVersion = VK_API_VERSION_1_0;//VK_API_VERSION_1_1

    #ifdef _DEBUG
    //spGraphicsInstanceCreateInfo->desiredLayersToEnable.push_back("VK_LAYER_KHRONOS_validation");
    //spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    //spGraphicsInstanceCreateInfo->desiredLayersToEnable.push_back("VK_LAYER_LUNARG_standard_validation");
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    #endif

    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME);
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_8BIT_STORAGE_EXTENSION_NAME);
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_16BIT_STORAGE_EXTENSION_NAME);
    //spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);   
    //spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME);
    //spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
    //spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
    //spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);

    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    #ifdef VK_USE_PLATFORM_MACOS_MVK
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);  
    #endif
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif

	spGraphicsInstanceCreateInfo->spVxGraphicsPipelineCreateInfo->shadersFilePaths.push_back(spCreateInfo->resourcePath + "/shaders/triangle.vert.spv");
	spGraphicsInstanceCreateInfo->spVxGraphicsPipelineCreateInfo->shadersFilePaths.push_back(spCreateInfo->resourcePath + "/shaders/triangle.frag.spv");

    //spGraphicsInstanceCreateInfo->spMainWindowCreateInfo->rpVxWindowLoopFunction = &windowLoop2;
    spGraphicsInstanceCreateInfo->rpMainWindowHandle = spCreateInfo->rpMainWindowHandle;
    spGraphicsInstanceCreateInfo->spMainWindowCreateInfo->rpExistingWindowHandle = spCreateInfo->rpMainWindowHandle;
    AssertVkVxResult(vxCreateGraphicsInstance, spGraphicsInstanceCreateInfo, spVxApplicationInstance);
    
    //To Deprecate
    //AssertVkVxResult(vxGraphicsRun, spVxGraphicsInstance);    

    return VxResult::VX_SUCCESS;
}

VxApplicationInstance::~VxApplicationInstance()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}
void VxApplicationInstance::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    if (spVxGraphicsInstance != nullptr)
    {
        spVxGraphicsInstance->destroy();
        spVxGraphicsInstance = nullptr;
    }
    if (spCreateInfo != nullptr)
    {
        spCreateInfo->destroy();
        spCreateInfo = nullptr;
    }
}

VxWindowLoopResult VxApplicationInstance::draw()
{
    auto spVxGraphicsWindow = this->spVxGraphicsInstance->spMainVxGraphicsWindow;

    if (spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkAcquireSemaphore == nullptr)
    {
        return VxWindowLoopResult::VX_WL_STOP;
    }
    GetAndAssertSharedPointer2(spVxGraphicsInstance, spVxGraphicsWindow->spVxGraphicsSurface->wpVxGraphicsInstance, VxWindowLoopResult::VX_WL_STOP);
    auto commandBuffer = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxQueueFamilies[0]->vkCommandBuffer;
    auto device = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->vkDevice;
    auto swapchain = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkSwapchain;
    auto swapchainFramebuffers = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkFramebuffers;
    auto swapchainImages = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkImages;    
    auto commandPool = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxQueueFamilies[0]->vkCommandPool; 
    auto queue = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxQueues[0]->vkQueue;
    auto renderPass = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkRenderPass;
    auto windowSize = vxGetWindowSize(spVxGraphicsWindow);
    auto acquireSemaphore = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkAcquireSemaphore;
    auto releaseSemaphore = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkReleaseSemaphore;
    auto trianglePipeline = spVxGraphicsInstance->spMainVxGraphicsPipeline->vkPipeline;

    uint32_t imageIndex = 0;
    AssertVkResultVxWindowLoop(vkAcquireNextImageKHR, device, swapchain, ~0ull, acquireSemaphore, VK_NULL_HANDLE, &imageIndex);

    AssertVkResultVxWindowLoop(vkResetCommandPool, device, commandPool, 0);

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    AssertVkResultVxWindowLoop(vkBeginCommandBuffer, commandBuffer, &beginInfo);

    VkImageMemoryBarrier renderBeginBarrier = imageBarrier2(swapchainImages[imageIndex], 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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

    VkImageMemoryBarrier renderEndBarrier = imageBarrier2(swapchainImages[imageIndex], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

VxApplicationInstanceCreateInfo::~VxApplicationInstanceCreateInfo()
{

}     
void VxApplicationInstanceCreateInfo::destroy()
{

}