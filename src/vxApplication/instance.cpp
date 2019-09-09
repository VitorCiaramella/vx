#include <vxApplication/vxApplication.hpp>
#include <vxGraphics/vxGraphics.hpp>
#include <vxGraphics/macros.hpp>

#include <iostream>
#include <vector>
#include <string>

#ifdef VK_USE_PLATFORM_MACOS_MVK
#include <MoltenVK/vk_mvk_moltenvk.h>
#endif

using namespace std;

spt(VxMemoryBuffer) spVertexBuffer1;
spt(VxMemoryBuffer) spIndexBuffer1;
VxMesh mesh1;

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
    spGraphicsInstanceCreateInfo->desiredLayersToEnable.push_back("VK_LAYER_KHRONOS_validation");
    //spGraphicsInstanceCreateInfo->desiredLayersToEnable.push_back("VK_LAYER_LUNARG_standard_validation");
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
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
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_MVK_MOLTENVK_EXTENSION_NAME);
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);  
    #endif
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    spGraphicsInstanceCreateInfo->desiredExtensionsToEnable.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif

	spGraphicsInstanceCreateInfo->shadersFilePaths.push_back(spCreateInfo->resourcePath + "/shaders/triangle.vert.spv");
	spGraphicsInstanceCreateInfo->shadersFilePaths.push_back(spCreateInfo->resourcePath + "/shaders/triangle.frag.spv");

	spGraphicsInstanceCreateInfo->spVxGraphicsPipelineCreateInfo->shadersFilePaths.push_back(spCreateInfo->resourcePath + "/shaders/triangle.vert.spv");
	spGraphicsInstanceCreateInfo->spVxGraphicsPipelineCreateInfo->shadersFilePaths.push_back(spCreateInfo->resourcePath + "/shaders/triangle.frag.spv");

    //spGraphicsInstanceCreateInfo->spMainWindowCreateInfo->rpVxWindowLoopFunction = &windowLoop2;
    spGraphicsInstanceCreateInfo->rpMainWindowHandle = spCreateInfo->rpMainWindowHandle;
    spGraphicsInstanceCreateInfo->spMainWindowCreateInfo->rpExistingWindowHandle = spCreateInfo->rpMainWindowHandle;
    AssertVkVxResult(vxCreateGraphicsInstance, spGraphicsInstanceCreateInfo, spVxApplicationInstance);
    
    //To Deprecate
    //AssertVkVxResult(vxGraphicsRun, spVxGraphicsInstance);    

    auto mesh = vxLoadMesh(spCreateInfo->resourcePath + "/objects/pirate.obj");

    spt(VxMemoryAllocator) spVxMemoryAllocator = spVxApplicationInstance->spVxGraphicsInstance->spMainVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxMemoryAllocator;
    spt(VxMemoryBuffer) spVertexBuffer;
    AssertVkVxResult(vxCreateVertexBuffer, spVxMemoryAllocator, 128 * 1024 * 1024, spVertexBuffer);
    spt(VxMemoryBuffer) spIndexBuffer;
    AssertVkVxResult(vxCreateIndexBuffer, spVxMemoryAllocator, 128 * 1024 * 1024, spIndexBuffer);

	memcpy(spVertexBuffer->data, mesh.vertices.data(), mesh.vertices.size() * sizeof(VxVertex));
	memcpy(spIndexBuffer->data, mesh.indices.data(), mesh.indices.size() * sizeof(uint32_t));

    spVertexBuffer1 = spVertexBuffer;
    spIndexBuffer1 = spIndexBuffer;
    mesh1 = mesh;

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
    this->mustTerminate = true;
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

VkResult vxAcquireNextImageKHR(spt(VxGraphicsWindow) spVxGraphicsWindow, uint32_t *rpImageIndex)
{
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsWindow->spVxGraphicsSurface->wpVxGraphicsInstance);
    auto device = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->vkDevice;

    VkResult vkResult;
    while(true) 
    {
        auto swapchain = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkSwapchain;
        auto spVxSemaphore = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->imageAvailableSemaphores[spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->currentFrame];
        auto acquireSemaphore = spVxSemaphore->vkSemaphore;
        // Get the index of the next available swapchain image:
        vkResult = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, rpImageIndex);

        if (vkResult == VK_SUCCESS) 
        {
            return VK_SUCCESS;
        } 
        else if (vkResult == VK_ERROR_OUT_OF_DATE_KHR) 
        {
            // demo->swapchain is out of date (e.g. the window was resized) and
            // must be recreated:
            spVxGraphicsInstance->spMainVxGraphicsPipeline->destroy();
            spVxGraphicsInstance->spMainVxGraphicsPipeline = nullptr;
            spVxGraphicsInstance->spMainVxGraphicsWindow->spVxGraphicsSurface->createSwapchainResult = vxCreateSwapchain(spVxGraphicsInstance->spMainVxGraphicsWindow->spVxGraphicsSurface, spVxGraphicsInstance->spMainVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain);
            spVxGraphicsInstance->createMainGraphicsPipelineResult = vxCreatePipeline(spVxGraphicsInstance->spMainVxGraphicsWindow->spVxGraphicsSurface, spVxGraphicsInstance->spCreateInfo->spVxGraphicsPipelineCreateInfo, spVxGraphicsInstance->spMainVxGraphicsPipeline);
            //demo_resize(demo);
            continue;
        } 
        else if (vkResult == VK_SUBOPTIMAL_KHR) 
        {
            // demo->swapchain is not as optimal as it could be, but the platform's
            // presentation engine will still present the image correctly.
            return VK_SUCCESS;
        } 
        else 
        {
            return vkResult;
        }
    } 
    return VK_SUCCESS;
}

VxWindowLoopResult VxApplicationInstance::draw()
{
    if (this->mustTerminate) return VxWindowLoopResult::VX_WL_STOP;

    auto spVxGraphicsWindow = this->spVxGraphicsInstance->spMainVxGraphicsWindow;

    GetAndAssertSharedPointer2(spVxGraphicsInstance, spVxGraphicsWindow->spVxGraphicsSurface->wpVxGraphicsInstance, VxWindowLoopResult::VX_WL_STOP);

    uint32_t imageIndex = 0;
    AssertVkResultVxWindowLoop(vxAcquireNextImageKHR, spVxGraphicsWindow, &imageIndex);
    
    auto device = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->vkDevice;
    auto swapchain = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkSwapchain;
    //TODO create one command buffer for each swapchain image, the record the command buffer only once
    auto commandBuffer = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxQueueFamilies[0]->vkCommandBuffer;
    auto swapchainFramebuffers = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkFramebuffers;
    auto swapchainImages = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkImages;    
    auto commandPool = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxQueueFamilies[0]->vkCommandPool; 
    auto queue = spVxGraphicsWindow->spVxGraphicsSurface->spVxSurfaceDevice->spVxQueues[0]->vkQueue;
    auto renderPass = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->vkRenderPass;
    auto trianglePipeline = spVxGraphicsInstance->spMainVxGraphicsPipeline->vkPipeline;
    auto swapchainSize = spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->swapchainSize;


    AssertVkResultVxWindowLoop(vkResetCommandPool, device, commandPool, 0);

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    AssertVkResultVxWindowLoop(vkBeginCommandBuffer, commandBuffer, &beginInfo);

    VkImageMemoryBarrier renderBeginBarrier = imageBarrier2(swapchainImages[imageIndex], 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &renderBeginBarrier);

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    VkRenderPassBeginInfo passBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    passBeginInfo.renderPass = renderPass;
    passBeginInfo.framebuffer = swapchainFramebuffers[imageIndex];
    passBeginInfo.renderArea.offset = {0, 0};
    passBeginInfo.renderArea.extent = swapchainSize;
    passBeginInfo.clearValueCount = 1;
    passBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = { 0, 0, float(swapchainSize.width), float(swapchainSize.height), 0, 1 };
    VkRect2D scissor = { {0, 0}, {swapchainSize.width, swapchainSize.height} };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkImageSubresourceRange range = {};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.levelCount = 1;
    range.layerCount = 1;
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline);
    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, spVxGraphicsInstance->spMainVxGraphicsPipeline->vkPipelineLayout, 0, 1,
    //                    &demo->swapchain_image_resources[demo->current_buffer].descriptor_set, 0, NULL);

    /*
    Start
    */
    VkDeviceSize dummyOffset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &spVertexBuffer1->vkBuffer, &dummyOffset);
    vkCmdBindIndexBuffer(commandBuffer, spIndexBuffer1->vkBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, mesh1.indices.size(), 1, 0, 0, 0);
    /*
    End
    */
    vkCmdEndRenderPass(commandBuffer);

    VkImageMemoryBarrier renderEndBarrier = imageBarrier2(swapchainImages[imageIndex], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, 0, 0, 0, 1, &renderEndBarrier);

    AssertVkResultVxWindowLoop(vkEndCommandBuffer, commandBuffer);

    VkSemaphore waitSemaphores[] = {spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->getImageAvailableSemaphore()};    
    VkSemaphore releaseSemaphores[] = {spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->getRenderFinishedSemaphore()};    

    VkPipelineStageFlags submitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = arraySize(waitSemaphores);
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = &submitStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = arraySize(releaseSemaphores);
    submitInfo.pSignalSemaphores = releaseSemaphores;
    AssertVkResultVxWindowLoop(vkQueueSubmit, queue, 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = arraySize(releaseSemaphores);
    presentInfo.pWaitSemaphores = releaseSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;
    AssertVkResultVxWindowLoop(vkQueuePresentKHR, queue, &presentInfo);

    spVxGraphicsWindow->spVxGraphicsSurface->spVxGraphicsSwapchain->advanceFrame();
    
    return VxWindowLoopResult::VX_WL_CONTINUE;
}

VxApplicationInstanceCreateInfo::~VxApplicationInstanceCreateInfo()
{

}     
void VxApplicationInstanceCreateInfo::destroy()
{

}