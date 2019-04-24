#include <vxGraphics/vxGraphics.hpp>

#include <string>
#include <algorithm>

#include "platform.hpp"


//TODO: make it a function pointer
void vxErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


VxResult vxCreateWindow(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    glfwSetErrorCallback(vxErrorCallback);

    if (!glfwInit())
    {
        return VxResult::VX_ERROR;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(upGraphicsInstance->rpCreateInfo->mainWindowWidth, upGraphicsInstance->rpCreateInfo->mainWindowHeight, "My Title", NULL, NULL);

    if (!window)
    {
        return VxResult::VX_ERROR;
    }

    //glfwMakeContextCurrent(window);
    //glfwSwapInterval(1);

    int width, height;
    //glfwGetFramebufferSize(window, &width, &height);
    //glViewport(0, 0, width, height);

    upGraphicsInstance->mainWindow = window;

    return VxResult::VX_SUCCESS;
}

VkExtent2D vxGetWindowSize(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    VkExtent2D result;
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(upGraphicsInstance->mainWindow, &width, &height);
    result.width = width;
    result.height = height;
    return result;
}

VkSemaphore createSemaphore(VkDevice device)
{
	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkSemaphore semaphore = 0;
	vkCreateSemaphore(device, &createInfo, 0, &semaphore);
	return semaphore;
}

VkResult vxAwaitWindowClose(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    VkCommandBuffer commandBuffer;
    AssertVkResult(vxAllocateCommandBuffer, upGraphicsInstance, commandBuffer);

    auto device = upGraphicsInstance->vxDevices[0].vkDevice;
    auto swapchain = upGraphicsInstance->vxMainSwapchain.vkSwapchain;
    auto swapchainFramebuffers = upGraphicsInstance->vxMainSwapchain.vkFramebuffers;
    auto swapchainImages = upGraphicsInstance->vxMainSwapchain.vkImages;    
    auto commandPool = upGraphicsInstance->vxDevices[0].vxQueueFamilies[0].vkCommandPool; 
    auto queue = upGraphicsInstance->vxDevices[0].vxQueues[0].vkQueue;
    auto renderPass = upGraphicsInstance->vkRenderPass;
    auto windowSize = vxGetWindowSize(upGraphicsInstance);
    auto acquireSemaphore = createSemaphore(upGraphicsInstance->vxDevices[0].vkDevice);
    auto releaseSemaphore = createSemaphore(upGraphicsInstance->vxDevices[0].vkDevice);
    auto trianglePipeline = upGraphicsInstance->vxPipeline.vkPipeline;

    while (!glfwWindowShouldClose(upGraphicsInstance->mainWindow))
    {
        glfwPollEvents();

        uint32_t imageIndex = 0;
		AssertVkResult(vkAcquireNextImageKHR, device, swapchain, ~0ull, acquireSemaphore, VK_NULL_HANDLE, &imageIndex);

		AssertVkResult(vkResetCommandPool, device, commandPool, 0);

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		AssertVkResult(vkBeginCommandBuffer, commandBuffer, &beginInfo);

		VkClearColorValue color = { 90.f / 255.f, 10.f / 255.f, 36.f / 255.f, 1 };
		VkClearValue clearColor = { color };

		VkRenderPassBeginInfo passBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		passBeginInfo.renderPass = renderPass;
		passBeginInfo.framebuffer = swapchainFramebuffers[imageIndex];
		passBeginInfo.renderArea.extent = windowSize;
		passBeginInfo.clearValueCount = 1;
		passBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = { 0, float(windowSize.height), float(windowSize.width), -float(windowSize.height), 0, 1 };
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

		AssertVkResult(vkEndCommandBuffer, commandBuffer);

		VkPipelineStageFlags submitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &acquireSemaphore;
		submitInfo.pWaitDstStageMask = &submitStageMask;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &releaseSemaphore;
		AssertVkResult(vkQueueSubmit, queue, 1, &submitInfo, VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &releaseSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;
		AssertVkResult(vkQueuePresentKHR, queue, &presentInfo);

        AssertVkResult(vkDeviceWaitIdle, device);

    }
    glfwDestroyWindow(upGraphicsInstance->mainWindow);

    return VkResult::VK_SUCCESS;
}


VkResult vxFillAvailableExtensions(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    uint32_t extensionCount;
    StoreAndAssertVkResultP(upGraphicsInstance, vkEnumerateInstanceExtensionProperties, nullptr, &extensionCount, nullptr);
    if (extensionCount > 0) 
    {
        upGraphicsInstance->vkAvailableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        StoreAndAssertVkResultP(upGraphicsInstance, vkEnumerateInstanceExtensionProperties, nullptr, &extensionCount, upGraphicsInstance->vkAvailableExtensions.data());
    }
    return VK_SUCCESS;
}

VkResult vxFillAvailableLayers(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    uint32_t layerCount;
    StoreAndAssertVkResultP(upGraphicsInstance, vkEnumerateInstanceLayerProperties, &layerCount, nullptr);
    if (layerCount > 0) 
    {
        auto availableLayers = nup(VkLayerProperties[],layerCount);
        StoreAndAssertVkResultP(upGraphicsInstance, vkEnumerateInstanceLayerProperties, &layerCount, availableLayers.get());
        CopyResultItems(upGraphicsInstance->vxAvailableLayers, vkLayer, VxGraphicsLayer, availableLayers, layerCount);

        for (auto && vxLayer : upGraphicsInstance->vxAvailableLayers) 
        {
            uint32_t extensionCount;
            StoreAndAssertVkResult(vxLayer, vkEnumerateInstanceExtensionProperties, vxLayer.vkLayer.layerName, &extensionCount, nullptr);
            if (extensionCount > 0) 
            {
                vxLayer.vkAvailableExtensions = std::vector<VkExtensionProperties>(extensionCount);
                StoreAndAssertVkResult(vxLayer, vkEnumerateInstanceExtensionProperties, vxLayer.vkLayer.layerName, &extensionCount, vxLayer.vkAvailableExtensions.data());
            }
        }
    }
    return VK_SUCCESS;
}

static bool vxQueueFamilySupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	return !!vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
#else
	return true;
#endif
}

VkResult vxFillAvailableDevices(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    uint32_t deviceCount;
    StoreAndAssertVkResultP(upGraphicsInstance, vkEnumeratePhysicalDevices, upGraphicsInstance->vkInstance, &deviceCount, nullptr);
    if (deviceCount > 0) 
    {
        auto availableDevices = nup(VkPhysicalDevice[],deviceCount);
        StoreAndAssertVkResultP(upGraphicsInstance, vkEnumeratePhysicalDevices, upGraphicsInstance->vkInstance, &deviceCount, availableDevices.get());
        CopyResultItems(upGraphicsInstance->vxAvailablePhysicalDevices, vkPhysicalDevice, VxGraphicsPhysicalDevice, availableDevices, deviceCount);

        for (auto && vxPhysicalDevice : upGraphicsInstance->vxAvailablePhysicalDevices) 
        {
            vxPhysicalDevice.supportsCompute = false;
            vxPhysicalDevice.supportsGraphics = false;
            vxPhysicalDevice.supportsPresentation = false;
            
            vkGetPhysicalDeviceProperties(vxPhysicalDevice.vkPhysicalDevice, &vxPhysicalDevice.vkPhysicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(vxPhysicalDevice.vkPhysicalDevice, &vxPhysicalDevice.vkPhysicalDeviceFeatures);

            uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(vxPhysicalDevice.vkPhysicalDevice, &queueFamilyPropertyCount, nullptr);
            if (queueFamilyPropertyCount > 0)
            {
                auto queueFamilyProperties = nup(VkQueueFamilyProperties[],queueFamilyPropertyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(vxPhysicalDevice.vkPhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.get());
                CopyResultItems(vxPhysicalDevice.vxQueueFamilies, vkQueueFamily, VxGraphicsQueueFamily, queueFamilyProperties, queueFamilyPropertyCount);

                uint32_t queueFamilyIndex = 0; 
                for (auto && vxQueueFamily : vxPhysicalDevice.vxQueueFamilies) 
                {
                    vxQueueFamily.queueFamilyIndex = queueFamilyIndex++;
                    vxQueueFamily.supportsPresentation = vxQueueFamilySupportsPresentation(vxPhysicalDevice.vkPhysicalDevice, vxQueueFamily.queueFamilyIndex);
                    vxQueueFamily.supportsCompute = vxQueueFamily.vkQueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
                    vxQueueFamily.supportsGraphics = vxQueueFamily.vkQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;

                    vxPhysicalDevice.supportsCompute = vxPhysicalDevice.supportsCompute | vxQueueFamily.supportsPresentation;
                    vxPhysicalDevice.supportsGraphics = vxPhysicalDevice.supportsGraphics | vxQueueFamily.supportsCompute;
                    vxPhysicalDevice.supportsPresentation = vxPhysicalDevice.supportsPresentation | vxQueueFamily.supportsGraphics;
                }
            }

            uint32_t extensionCount;
            StoreAndAssertVkResult(vxPhysicalDevice, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice.vkPhysicalDevice, nullptr, &extensionCount, nullptr);
            if (extensionCount > 0) 
            {
                vxPhysicalDevice.vkAvailableExtensions = std::vector<VkExtensionProperties>(extensionCount);
                StoreAndAssertVkResult(vxPhysicalDevice, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice.vkPhysicalDevice, nullptr, &extensionCount, vxPhysicalDevice.vkAvailableExtensions.data());
            }            
            for (auto && vxLayer : upGraphicsInstance->vxAvailableLayers) 
            {
                StoreAndAssertVkResult(vxPhysicalDevice, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice.vkPhysicalDevice, vxLayer.vkLayer.layerName, &extensionCount, nullptr);
                if (extensionCount > 0) 
                {
                    auto baseIndex = vxPhysicalDevice.vkAvailableExtensions.size();
                    vxPhysicalDevice.vkAvailableExtensions.reserve(vxPhysicalDevice.vkAvailableExtensions.size()+extensionCount);
                    StoreAndAssertVkResult(vxPhysicalDevice, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice.vkPhysicalDevice, vxLayer.vkLayer.layerName, &extensionCount, &vxPhysicalDevice.vkAvailableExtensions[baseIndex]);
                }            
            }
        }
    }
    return VK_SUCCESS;
}

VkResult vxCreateVkInstance(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

    VkApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = strdup(upGraphicsInstance->rpCreateInfo->applicationName.c_str());
    applicationInfo.applicationVersion = upGraphicsInstance->rpCreateInfo->applicationVersion;
    applicationInfo.pEngineName = strdup(upGraphicsInstance->rpCreateInfo->engineName.c_str());
    applicationInfo.engineVersion = upGraphicsInstance->rpCreateInfo->engineVersion;
    applicationInfo.apiVersion = upGraphicsInstance->rpCreateInfo->apiVersion;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    auto layersToEnable = nrp(std::vector<char*>);
    for (auto && desiredLayerToEnable : upGraphicsInstance->rpCreateInfo->desiredLayersToEnable) 
    {
        for (auto && vxLayer : upGraphicsInstance->vxAvailableLayers) 
        {
            if (strcmp(desiredLayerToEnable.c_str(), vxLayer.vkLayer.layerName) == 0) 
            {
                layersToEnable->push_back(strdup(desiredLayerToEnable.c_str()));
            }           
        }
    }
    instanceCreateInfo.enabledLayerCount = layersToEnable->size();
    instanceCreateInfo.ppEnabledLayerNames = layersToEnable->data();

    auto extensionsToEnable = nrp(std::vector<char*>);
    for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
    {
        for (auto && vkExtension : upGraphicsInstance->vkAvailableExtensions) 
        {
            if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
            {
                extensionsToEnable->push_back(strdup(desiredExtensionToEnable.c_str()));
            }           
        }
    }
    instanceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
    instanceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();

    StoreAndAssertVkResultP(upGraphicsInstance, vkCreateInstance, &instanceCreateInfo, NULL, &upGraphicsInstance->vkInstance);

    return VK_SUCCESS;
}

VkResult vxSelectPhysicalDevices(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    for (auto && vxPhysicalDevice : upGraphicsInstance->vxAvailablePhysicalDevices) 
    {
        bool deviceHasAllDesiredExtensions = true;
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            bool containExtension = false;
            for (auto && vkExtension : vxPhysicalDevice.vkAvailableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
                {
                    containExtension = true;
                    break;
                }           
            }
            if (!containExtension)
            {
                deviceHasAllDesiredExtensions = false;
                break;
            }
        }

        if (true
            //TODO: support required and nice to have extensions
            //&& deviceHasAllDesiredExtensions 
            //TODO: no devices with Vulkan 1.1, support minimum parameter
            //&& availableDevice.deviceProperties.apiVersion >= upGraphicsInstance->rpCreateInfo->apiVersion
            && vxPhysicalDevice.supportsGraphics
            && vxPhysicalDevice.supportsPresentation
            )
        {
            //prioritize discrete devices
            //TODO: create a sorting based on device type and memory
            if (vxPhysicalDevice.vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                upGraphicsInstance->vxSelectedPhysicalDevices.insert(upGraphicsInstance->vxSelectedPhysicalDevices.begin(), &vxPhysicalDevice);            
            }
            else
            {
                upGraphicsInstance->vxSelectedPhysicalDevices.push_back(&vxPhysicalDevice);
            }           
        }
    }
    return VK_SUCCESS;
}

VkResult vxCreateDevices(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    uint32_t desiredDeviceCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredDeviceCount,(uint32_t)1,(uint32_t)upGraphicsInstance->vxSelectedPhysicalDevices.size());
    for (auto && vxPhysicalDevice : upGraphicsInstance->vxSelectedPhysicalDevices) 
    {
        VxGraphicsDevice vxDevice = {};
        initVxGraphicsDevice(vxDevice);

        //TODO: instead of global queue count, do per desired device type
        uint32_t desiredQueueCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredQueueCountPerDevice,(uint32_t)1,(uint32_t)100);

        VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

        auto rpQueueCreateInfos = nrp(std::vector<VkDeviceQueueCreateInfo>);
        for (auto && queueFamily : vxPhysicalDevice->vxQueueFamilies) 
        {
            if (queueFamily.supportsPresentation
                && queueFamily.supportsGraphics)
            {
                uint32_t queueCount = std::clamp(desiredQueueCount,(uint32_t)1,queueFamily.vkQueueFamily.queueCount);
                desiredQueueCount = std::clamp(desiredQueueCount-queueCount,(uint32_t)0,(uint32_t)100);
                float queuePriority = 1.f;
                auto queuePriorities = nrp(std::vector<float>,queueCount,queuePriority);

                VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                queueCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;
                queueCreateInfo.queueCount = queueCount;
                queueCreateInfo.pQueuePriorities = queuePriorities->data();
                rpQueueCreateInfos->push_back(queueCreateInfo);

                vxDevice.vxQueueFamilies.push_back(queueFamily);
                for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
                {
                    VxGraphicsQueue vxQueue = {};
                    initVxGraphicsQueue(vxQueue);
                    vxQueue.queueFamilyIndex = queueFamily.queueFamilyIndex;
                    vxQueue.queueIndex = queueIndex;
                    vxDevice.vxQueues.push_back(vxQueue);
                }                
            }
            if (desiredQueueCount == 0)
            {
                break;
            }
        }
        deviceCreateInfo.queueCreateInfoCount = rpQueueCreateInfos->size();
        deviceCreateInfo.pQueueCreateInfos = rpQueueCreateInfos->data();
        deviceCreateInfo.pEnabledFeatures = &vxPhysicalDevice->vkPhysicalDeviceFeatures;

        auto extensionsToEnable = nrp(std::vector<char*>);
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            for (auto && vkExtension : vxPhysicalDevice->vkAvailableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
                {
                    extensionsToEnable->push_back(strdup(desiredExtensionToEnable.c_str()));
                }           
            }
        }
        deviceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
        deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();

        VkDevice vkDevice;
        StoreAndAssertVkResultP(upGraphicsInstance, vkCreateDevice, vxPhysicalDevice->vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice);
        vxDevice.vkDevice = vkDevice;
        for (auto && vxQueue : vxDevice.vxQueues) 
        {
            vkGetDeviceQueue(vxDevice.vkDevice, vxQueue.queueFamilyIndex, vxQueue.queueIndex, &vxQueue.vkQueue);
        }
        for (auto && vxQueueFamily : vxDevice.vxQueueFamilies) 
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = vxQueueFamily.queueFamilyIndex;                    
            StoreAndAssertVkResult(vxQueueFamily, vkCreateCommandPool, vxDevice.vkDevice, &commandPoolCreateInfo, nullptr, &vxQueueFamily.vkCommandPool);
        }

        upGraphicsInstance->vxDevices.push_back(vxDevice);

        if (upGraphicsInstance->vxDevices.size() >= desiredDeviceCount) 
        {
            break;
        }        
    }
    return VK_SUCCESS;
}

VkResult vxCreateSwapchain(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxSwapchain = &upGraphicsInstance->vxMainSwapchain;

    //TODO: how to choose best format?
    vxSwapchain->vkFormat = upGraphicsInstance->vxMainSurface.vkSurfaceFormats[0];
    vxSwapchain->vxDevice = &(upGraphicsInstance->vxDevices[0]);

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = upGraphicsInstance->vxMainSurface.vkSurface;

    uint32_t desiredImageCount = 2;
	createInfo.minImageCount = std::clamp(desiredImageCount, upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.minImageCount, upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.maxImageCount==0 ? desiredImageCount : upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.maxImageCount);;

	createInfo.imageFormat = vxSwapchain->vkFormat.format;
	createInfo.imageColorSpace = vxSwapchain->vkFormat.colorSpace;

    auto windowSize = vxGetWindowSize(upGraphicsInstance);
    if (upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        createInfo.imageExtent.width = std::clamp(windowSize.width, upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.minImageExtent.width, upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.maxImageExtent.width);
        createInfo.imageExtent.height = std::clamp(windowSize.height, upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.minImageExtent.height, upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.maxImageExtent.height);
    }
    else
    {
        createInfo.imageExtent = upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.currentExtent;
    }

	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; //is the number of queue families having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
	createInfo.pQueueFamilyIndices = nullptr; //s an array of queue family indices having access to the images(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT

	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    auto oldSwapchain = vxSwapchain->vkSwapchain;
    createInfo.oldSwapchain = oldSwapchain;

	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    //if not supported, use current transformation
    if (!(upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.supportedTransforms & createInfo.preTransform))
    {
        createInfo.preTransform = upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.currentTransform;
    }

    // Find the first supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < 4; i++) {
        if (upGraphicsInstance->vxMainSurface.vkSurfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            createInfo.compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }    

	createInfo.clipped = VK_TRUE;
    //TODO: parametrize the device
    StoreAndAssertVkResultP(vxSwapchain, vkCreateSwapchainKHR, vxSwapchain->vxDevice->vkDevice, &createInfo, nullptr, &vxSwapchain->vkSwapchain);

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        //TODO: parametrize the device
        vkDestroySwapchainKHR(vxSwapchain->vxDevice->vkDevice, oldSwapchain, nullptr);
    }

    uint32_t imageCount;
    StoreAndAssertVkResultP(upGraphicsInstance, vkEnumerateInstanceExtensionProperties, nullptr, &imageCount, nullptr);
    if (imageCount > 0) 
    {
        vxSwapchain->vkImages = std::vector<VkImage>(imageCount);
        StoreAndAssertVkResultP(vxSwapchain, vkGetSwapchainImagesKHR, vxSwapchain->vxDevice->vkDevice, vxSwapchain->vkSwapchain, &imageCount, vxSwapchain->vkImages.data());

        vxSwapchain->vkImageViews = std::vector<VkImageView>(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            createInfo.image = vxSwapchain->vkImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = vxSwapchain->vkFormat.format;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;
            StoreAndAssertVkResultP(vxSwapchain, vkCreateImageView, vxSwapchain->vxDevice->vkDevice, &createInfo, nullptr, &vxSwapchain->vkImageViews[i]);
        }

        vxSwapchain->vkFramebuffers = std::vector<VkFramebuffer>(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            createInfo.renderPass = upGraphicsInstance->vkRenderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &vxSwapchain->vkImageViews[i];
            createInfo.width = windowSize.width;
            createInfo.height = windowSize.height;
            createInfo.layers = 1;
            StoreAndAssertVkResultP(vxSwapchain, vkCreateFramebuffer, vxSwapchain->vxDevice->vkDevice, &createInfo, nullptr, &vxSwapchain->vkFramebuffers[i]);
        }
    }
    return VK_SUCCESS;
}

VkResult vxFillSurfaceProperties(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    //TODO: Iterate through wanted surfaces
    auto surface = &upGraphicsInstance->vxMainSurface;
    //TODO: Iterate through wanted devices
    auto physicalDevice = upGraphicsInstance->vxSelectedPhysicalDevices[0]->vkPhysicalDevice;

    uint32_t formatCount;
	StoreAndAssertVkResultP(surface, vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, surface->vkSurface, &formatCount, nullptr);
    if (formatCount > 0) 
    {
        surface->vkSurfaceFormats = std::vector<VkSurfaceFormatKHR>(formatCount);
    	StoreAndAssertVkResultP(surface, vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, surface->vkSurface, &formatCount, surface->vkSurfaceFormats.data());
    }

    StoreAndAssertVkResultP(surface, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, physicalDevice, surface->vkSurface, &surface->vkSurfaceCapabilities);

    uint32_t presentModeCount;
	StoreAndAssertVkResultP(surface, vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice, surface->vkSurface, &presentModeCount, nullptr);
    if (presentModeCount > 0) 
    {
        surface->vkPresentModes = std::vector<VkPresentModeKHR>(presentModeCount);
    	StoreAndAssertVkResultP(surface, vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice, surface->vkSurface, &presentModeCount, surface->vkPresentModes.data());
    }

    return VK_SUCCESS;
}

VkResult vxCreateRenderPass(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
	VkAttachmentDescription attachments[1] = {};
	attachments[0].format = upGraphicsInstance->vxMainSwapchain.vkFormat.format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachments = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachments;

	VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	createInfo.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
	createInfo.pAttachments = attachments;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;

	StoreAndAssertVkResultP(upGraphicsInstance, vkCreateRenderPass, upGraphicsInstance->vxMainSwapchain.vxDevice->vkDevice, &createInfo, 0, &upGraphicsInstance->vkRenderPass);

	return VK_SUCCESS;
}

VkResult vxLoadShader(VkDevice device, std::string filePath, VkShaderModule * shaderModule)
{
	FILE* file = fopen(filePath.c_str(), "rb");
	assert(file);

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	assert(length >= 0);
	fseek(file, 0, SEEK_SET);

	char* buffer = new char[length];
	assert(buffer);

	size_t rc = fread(buffer, 1, length, file);
	assert(rc == size_t(length));
	fclose(file);

	VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	createInfo.codeSize = length;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer);

	auto vkResult = vkCreateShaderModule(device, &createInfo, 0, shaderModule);

	delete[] buffer;

	return vkResult;
}

VkResult vxLoadShaders(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxDevice = &(upGraphicsInstance->vxDevices[0]);
    
    upGraphicsInstance->vxShaders = std::vector<VxGraphicsShader>(upGraphicsInstance->rpCreateInfo->shadersFilePaths.size());
    uint32_t index = 0;
    for (auto && shadersFilePath : upGraphicsInstance->rpCreateInfo->shadersFilePaths) 
    {
        auto vxShader = &(upGraphicsInstance->vxShaders[index++]);
        vxShader->vxDevice = vxDevice;
        vxShader->filePath = shadersFilePath;
        StoreAndAssertVkResultP(vxShader, vxLoadShader, vxShader->vxDevice->vkDevice, vxShader->filePath, &vxShader->vkShaderModule);
    }
    return VK_SUCCESS;
}

VkResult vxCreatePipelineLayout(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    upGraphicsInstance->vxPipelineLayout.vxDevice = &(upGraphicsInstance->vxDevices[0]);
    auto vxPipelineLayout = &upGraphicsInstance->vxPipelineLayout;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	StoreAndAssertVkResultP(vxPipelineLayout, vkCreatePipelineLayout, vxPipelineLayout->vxDevice->vkDevice, &pipelineLayoutCreateInfo, 0, &vxPipelineLayout->vkPipelineLayout);
    return VK_SUCCESS;
}

VkResult vxCreatePipeline(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxPipeline = &(upGraphicsInstance->vxPipeline);
    vxPipeline->vxDevice = &(upGraphicsInstance->vxDevices[0]);
    vxPipeline->vxPipelineLayout = &(upGraphicsInstance->vxPipelineLayout);
    vxPipeline->vkRenderPass = upGraphicsInstance->vkRenderPass;
    vxPipeline->vkPipelineCache = nullptr;

	VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

	auto stages = std::vector<VkPipelineShaderStageCreateInfo>(2);
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = upGraphicsInstance->vxShaders[0].vkShaderModule;
	stages[0].pName = "main";
	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = upGraphicsInstance->vxShaders[1].vkShaderModule;
	stages[1].pName = "main";

	createInfo.stageCount = stages.size();
	createInfo.pStages = stages.data();

	VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	createInfo.pVertexInputState = &vertexInput;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	createInfo.pInputAssemblyState = &inputAssembly;

	VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	createInfo.pViewportState = &viewportState;

	VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterizationState.lineWidth = 1.f;
	createInfo.pRasterizationState = &rasterizationState;

	VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.pMultisampleState = &multisampleState;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	createInfo.pDepthStencilState = &depthStencilState;

	VkPipelineColorBlendAttachmentState colorAttachmentState = {};
	colorAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorAttachmentState;
	createInfo.pColorBlendState = &colorBlendState;

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
	dynamicState.pDynamicStates = dynamicStates;
	createInfo.pDynamicState = &dynamicState;

	createInfo.layout = vxPipeline->vxPipelineLayout->vkPipelineLayout;
	createInfo.renderPass = vxPipeline->vkRenderPass;

	StoreAndAssertVkResultP(vxPipeline, vkCreateGraphicsPipelines, vxPipeline->vxDevice->vkDevice, vxPipeline->vkPipelineCache, 1, &createInfo, nullptr, &vxPipeline->vkPipeline);

    return VK_SUCCESS;
}

VxResult vxCreateGraphicsInstance(rpt(VxGraphicsInstanceCreateInfo) rpCreateInfo, upt(VxGraphicsInstance) & upGraphicsInstance)
{
    upGraphicsInstance = nup(VxGraphicsInstance);
    initVxGraphicsInstance(upGraphicsInstance);
    upGraphicsInstance->rpCreateInfo = rpCreateInfo;

    AssertVkVxResult(vxFillAvailableLayers, upGraphicsInstance);
    AssertVkVxResult(vxFillAvailableExtensions, upGraphicsInstance);
    AssertVkVxResult(vxCreateVkInstance, upGraphicsInstance);
    AssertVkVxResult(vxFillAvailableDevices, upGraphicsInstance);

    AssertVkVxResult(vxSelectPhysicalDevices, upGraphicsInstance);
    AssertVkVxResult(vxCreateDevices, upGraphicsInstance);

    AssertVxResult(vxCreateWindow, upGraphicsInstance);

    AssertVkVxResult(vxCreateSurface, upGraphicsInstance);
    AssertVkVxResult(vxFillSurfaceProperties, upGraphicsInstance);
    AssertVkVxResult(vxCreateSwapchain, upGraphicsInstance);

    AssertVkVxResult(vxLoadShaders, upGraphicsInstance);
    AssertVkVxResult(vxCreatePipelineLayout, upGraphicsInstance);
    AssertVkVxResult(vxCreateRenderPass, upGraphicsInstance);
    AssertVkVxResult(vxCreatePipeline, upGraphicsInstance);

    return VxResult::VX_SUCCESS;
}

VxResult vxGraphicsRun(upt(VxGraphicsInstance) & upGraphicsInstance)
{
    AssertVkVxResult(vxAwaitWindowClose, upGraphicsInstance);

    return VxResult::VX_SUCCESS;
}

VxResult vxGraphicsTerminate(upt(VxGraphicsInstance) & upGraphicsInstance)
{
    glfwTerminate();    

    return VxResult::VX_SUCCESS;
}

VxResult vxGraphicsDestroyInstance(upt(VxGraphicsInstance) & upGraphicsInstance)
{
    vkDestroyInstance(upGraphicsInstance->vkInstance, nullptr);
    upGraphicsInstance.release();
    return VxResult::VX_SUCCESS;
}

VkResult vxAllocateCommandBuffer(const upt(VxGraphicsInstance) & upGraphicsInstance, VkCommandBuffer & commandBuffer)
{
    auto device = upGraphicsInstance->vxDevices[0];

    VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocateInfo.commandPool = device.vxQueueFamilies[0].vkCommandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    auto commandBuffers = nup(VkCommandBuffer[],allocateInfo.commandBufferCount);
    AssertVkResult(vkAllocateCommandBuffers, device.vkDevice, &allocateInfo, commandBuffers.get());

    commandBuffer = commandBuffers[0];

    return VK_SUCCESS;
}


