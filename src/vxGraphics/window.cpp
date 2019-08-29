#include "commonHeaders.hpp"

#include <algorithm>

void VxGraphicsSurfacePhysicalDevice::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    wpVxSupportedQueueFamilies.clear();
    vkSurfaceFormats.clear();
    vkPresentModes.clear();
}

VxGraphicsSurfacePhysicalDevice::~VxGraphicsSurfacePhysicalDevice()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsSurface::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    spVxGraphicsSwapchain->destroy();
    spVxGraphicsSwapchain = nullptr;
    spVxSurfaceDevice->destroy();
    spVxSurfaceDevice = nullptr;
    spVxSupportedPhysicalDevices.clear();
    if (vkSurface != nullptr)
    {
        GetAndAssertSharedPointer(spVxGraphicsInstance, wpVxGraphicsInstance);
        AssertNotNull(spVxGraphicsInstance->vkInstance);
        vxLogInfo2("Destroying vkSurface...", "Memory");
        vkDestroySurfaceKHR(spVxGraphicsInstance->vkInstance, vkSurface, nullptr);
        vkSurface = nullptr;
        vxLogInfo2("vkSurface destroyed.", "Memory");
    }
}

VxGraphicsSurface::~VxGraphicsSurface()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsWindowCreateInfo::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
}

VxGraphicsWindowCreateInfo::~VxGraphicsWindowCreateInfo()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsWindow::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    spVxGraphicsSurface->destroy();
    spVxGraphicsSurface = nullptr;
}

VxGraphicsWindow::~VxGraphicsWindow()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsSwapchain::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);

    GetAndAssertSharedPointer(spVxDevice, wpVxDevice);
    AssertNotNull(spVxDevice->vkDevice);
    if (vkReleaseSemaphore != nullptr)
    {
        vkDestroySemaphore(spVxDevice->vkDevice, vkReleaseSemaphore, nullptr);
        vkReleaseSemaphore = nullptr;
    }
    if (vkAcquireSemaphore != nullptr)
    {
        vkDestroySemaphore(spVxDevice->vkDevice, vkAcquireSemaphore, nullptr);
        vkAcquireSemaphore = nullptr;
    }
	for (auto && vkFramebuffer : vkFramebuffers)
    {
		vkDestroyFramebuffer(spVxDevice->vkDevice, vkFramebuffer, nullptr);
    }
	for (auto && vkImageView : vkImageViews)
    {
		vkDestroyImageView(spVxDevice->vkDevice, vkImageView, nullptr);
    }
    vkFramebuffers.clear();
    vkImageViews.clear();
    vkImages.clear();
    if (vkRenderPass != nullptr)
    {
        vkDestroyRenderPass(spVxDevice->vkDevice, vkRenderPass, nullptr);
        vkRenderPass = nullptr;
    }
    if (vkSwapchain != nullptr)
    {
        vkDestroySwapchainKHR(spVxDevice->vkDevice, vkSwapchain, nullptr);
        vkSwapchain = nullptr;
    }
}

VxGraphicsSwapchain::~VxGraphicsSwapchain()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

VkResult vxCreateGraphicsWindow(spt(VxGraphicsWindowCreateInfo) spVxGraphicsWindowCreateInfo, spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsWindow) & spVxGraphicsWindow)
{
    AssertNotNullVkResult(spVxGraphicsWindowCreateInfo->rpExistingWindowHandle);
    spVxGraphicsWindow = nsp<VxGraphicsWindow>(spVxGraphicsInstance, spVxGraphicsWindowCreateInfo->rpVxWindowLoopFunction);
    spVxGraphicsWindow->rpWindowHandle = spVxGraphicsWindowCreateInfo->rpExistingWindowHandle;
    return VkResult::VK_SUCCESS;
}

VkResult vxGetSurfaceCapabilities(const spt(VxGraphicsWindow) & spVxGraphicsWindow, VkSurfaceCapabilitiesKHR & vkSurfaceCapabilities)
{
    auto spVxGraphicsSurface = spVxGraphicsWindow->spVxGraphicsSurface;
    AssertNotNullVkResult(spVxGraphicsSurface);
    AssertNotNullVkResult(spVxGraphicsSurface->vkSurface);

    auto spVxSurfaceDevice = spVxGraphicsSurface->spVxSurfaceDevice;
    AssertNotNullVkResult(spVxSurfaceDevice);
    AssertNotNullVkResult(spVxSurfaceDevice->vkDevice);

    GetAndAssertSharedPointerVk(spVxSurfacePhysicalDevice, spVxGraphicsSurface->wpVxSurfacePhysicalDevice);
    vkSurfaceCapabilities = spVxSurfacePhysicalDevice->vkSurfaceCapabilities;
    return VkResult::VK_SUCCESS;
}

VkExtent2D vxGetWindowSize(const spt(VxGraphicsWindow) & spVxGraphicsWindow)
{
    VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
    if (vxGetSurfaceCapabilities(spVxGraphicsWindow, vkSurfaceCapabilities) == VkResult::VK_SUCCESS)
    {
        return vkSurfaceCapabilities.currentExtent;
    }                    

    VkExtent2D result;
    result.width = 500;
    result.height = 500;
    return result;
}

VxWindowLoopResult windowDummyLoop(const spt(VxGraphicsWindow) & spVxGraphicsWindow)
{
    return VxWindowLoopResult::VX_WL_CONTINUE;
}

VkResult vxSelectSurfacePhysicalDevice(spt(VxGraphicsSurface) spVxGraphicsSurface)
{
    vxLogInfo2("Selecting best physical device for graphics surface...", "Vulkan");
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);
    vectorW(VxGraphicsSurfacePhysicalDevice) sortedDevices;
    for (auto && vxSurfacePhysicalDevice : spVxGraphicsSurface->spVxSupportedPhysicalDevices) 
    {
        GetAndAssertSharedPointerVk(spVxPhysicalDevice, vxSurfacePhysicalDevice->wpVxPhysicalDevice);
        bool deviceHasAllDesiredExtensions = true;
        for (auto && desiredExtensionToEnable : spVxGraphicsInstance->spCreateInfo->desiredExtensionsToEnable) 
        {
            bool containExtension = false;
            for (auto && vkExtension : spVxPhysicalDevice->vkAvailableExtensions) 
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
            )
        {
            //prioritize discrete devices
            //TODO: create a sorting based on device type and memory
            if (spVxPhysicalDevice->vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                sortedDevices.insert(sortedDevices.begin(), vxSurfacePhysicalDevice);       
                break;
            }
            else
            {
                sortedDevices.push_back(vxSurfacePhysicalDevice);
            }           
        }
    }
    if (sortedDevices.size() > 0)
    {
        spVxGraphicsSurface->wpVxSurfacePhysicalDevice = sortedDevices[0];
        vxLogInfo2("Graphics surface physical device selected.", "Vulkan");
        return VK_SUCCESS;
    }
    vxLogError2("Couldn't find a physical device for the graphics surface.", "Vulkan");
    return VK_ERROR_SURFACE_LOST_KHR;
}

VkResult vxCreateGraphicsSurface(const spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsSurface) & spVxGraphicsSurface)
{
    spVxGraphicsSurface = nsp<VxGraphicsSurface>(spVxGraphicsInstance, spVxGraphicsInstance->spMainVxGraphicsWindow);
    vxLogInfo2("Creating graphics surface...", "Vulkan");
    AssertVkResult(vxCreateSurface_PlatformSpecific, spVxGraphicsSurface);
    vxLogInfo2("Graphics surface created.", "Vulkan");

    vxLogInfo2("Getting supported physical devices for the graphics surface...", "Vulkan");
    for (auto && vxPhysicalDevice : spVxGraphicsInstance->spVxAvailablePhysicalDevices)
    {
        if (!vxPhysicalDevice->supportsGraphics)
        {
            continue;
        }
        auto spVxSupportedPhysicalDevice = nsp<VxGraphicsSurfacePhysicalDevice>();
        for (auto && vxQueueFamily : vxPhysicalDevice->spVxQueueFamilies)
        {
            auto supportsPresentation = vxQueueFamilySupportsPresentation_PlatformSpecific(vxPhysicalDevice->vkPhysicalDevice, vxQueueFamily->queueFamilyIndex);
            if (!supportsPresentation)
            {
                continue;
            }

            VkBool32 surfaceSupported = VK_FALSE;
            AssertVkResult(vkGetPhysicalDeviceSurfaceSupportKHR, vxPhysicalDevice->vkPhysicalDevice, vxQueueFamily->queueFamilyIndex, spVxGraphicsSurface->vkSurface, &surfaceSupported);
            if (surfaceSupported)
            {
                spVxSupportedPhysicalDevice->wpVxSupportedQueueFamilies.push_back(vxQueueFamily);
            }
        }
        if (spVxSupportedPhysicalDevice->wpVxSupportedQueueFamilies.size() > 0)
        {
            spVxSupportedPhysicalDevice->wpVxSurface = spVxGraphicsSurface;
            spVxSupportedPhysicalDevice->wpVxPhysicalDevice = vxPhysicalDevice;

            uint32_t formatCount;
            StoreAndAssertVkResultP(spVxSupportedPhysicalDevice->vkGetPhysicalDeviceSurfaceFormatsKHRResult, vkGetPhysicalDeviceSurfaceFormatsKHR, vxPhysicalDevice->vkPhysicalDevice, spVxGraphicsSurface->vkSurface, &formatCount, nullptr);
            if (formatCount > 0) 
            {
                spVxSupportedPhysicalDevice->vkSurfaceFormats = std::vector<VkSurfaceFormatKHR>(formatCount);
                StoreAndAssertVkResultP(spVxSupportedPhysicalDevice->vkGetPhysicalDeviceSurfaceFormatsKHRResult, vkGetPhysicalDeviceSurfaceFormatsKHR, vxPhysicalDevice->vkPhysicalDevice, spVxGraphicsSurface->vkSurface, &formatCount, spVxSupportedPhysicalDevice->vkSurfaceFormats.data());
            }

            StoreAndAssertVkResultP(spVxSupportedPhysicalDevice->vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vxPhysicalDevice->vkPhysicalDevice, spVxGraphicsSurface->vkSurface, &spVxSupportedPhysicalDevice->vkSurfaceCapabilities);

            uint32_t presentModeCount;
            StoreAndAssertVkResultP(spVxSupportedPhysicalDevice->vkGetPhysicalDeviceSurfacePresentModesKHRResult, vkGetPhysicalDeviceSurfacePresentModesKHR, vxPhysicalDevice->vkPhysicalDevice, spVxGraphicsSurface->vkSurface, &presentModeCount, nullptr);
            if (presentModeCount > 0) 
            {
                spVxSupportedPhysicalDevice->vkPresentModes = std::vector<VkPresentModeKHR>(presentModeCount);
                StoreAndAssertVkResultP(spVxSupportedPhysicalDevice->vkGetPhysicalDeviceSurfacePresentModesKHRResult, vkGetPhysicalDeviceSurfacePresentModesKHR, vxPhysicalDevice->vkPhysicalDevice, spVxGraphicsSurface->vkSurface, &presentModeCount, spVxSupportedPhysicalDevice->vkPresentModes.data());
            }

            spVxGraphicsSurface->spVxSupportedPhysicalDevices.push_back(spVxSupportedPhysicalDevice);
        }
    }
    vxLogInfo3("Supported physical devices for the graphics surface: %i", "Vulkan", spVxGraphicsSurface->spVxSupportedPhysicalDevices.size());

    AssertVkResult(vxSelectSurfacePhysicalDevice, spVxGraphicsSurface);

    return VK_SUCCESS;
}

VkResult vxCreateSurfaceDevice(spt(VxGraphicsSurface) spVxGraphicsSurface, spt(VxGraphicsDevice) & spVxGraphicsDevice)
{
    GetAndAssertSharedPointerVk(spVxSurfacePhysicalDevice, spVxGraphicsSurface->wpVxSurfacePhysicalDevice);
    GetAndAssertSharedPointerVk(spVxPhysicalDevice, spVxSurfacePhysicalDevice->wpVxPhysicalDevice);
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);

    spVxGraphicsDevice = nsp<VxGraphicsDevice>();
    spVxGraphicsDevice->wpVxPhysicalDevice = spVxSurfacePhysicalDevice->wpVxPhysicalDevice;

    //TODO: instead of global queue count, do per desired device type
    uint32_t desiredQueueCountPerDevice = 1;
    uint32_t desiredQueueCount = std::clamp(desiredQueueCountPerDevice,(uint32_t)1,(uint32_t)100);

    VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

    auto rpQueueCreateInfos = nrp(std::vector<VkDeviceQueueCreateInfo>);
    for (auto && wpVxSupportedQueueFamily : spVxSurfacePhysicalDevice->wpVxSupportedQueueFamilies) 
    {
        GetAndAssertSharedPointerVk(spVxSupportedQueueFamily, wpVxSupportedQueueFamily);
        uint32_t queueCount = std::clamp(desiredQueueCount,(uint32_t)1,spVxSupportedQueueFamily->vkQueueFamily.queueCount);
        desiredQueueCount = std::clamp(desiredQueueCount-queueCount,(uint32_t)0,(uint32_t)100);
        float queuePriority = 1.f;
        auto queuePriorities = nrp(std::vector<float>,queueCount,queuePriority);

        VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        queueCreateInfo.queueFamilyIndex = spVxSupportedQueueFamily->queueFamilyIndex;
        queueCreateInfo.queueCount = queueCount;
        queueCreateInfo.pQueuePriorities = queuePriorities->data();
        rpQueueCreateInfos->push_back(queueCreateInfo);

        auto spVxGraphicsDeviceQueueFamily = nsp<VxGraphicsDeviceQueueFamily>(spVxGraphicsDevice);
        spVxGraphicsDevice->spVxQueueFamilies.push_back(spVxGraphicsDeviceQueueFamily);

        for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
        {
            auto vxQueue = nsp<VxGraphicsQueue>(spVxGraphicsDeviceQueueFamily);
            vxQueue->queueFamilyIndex = spVxSupportedQueueFamily->queueFamilyIndex;
            vxQueue->queueIndex = queueIndex;
            spVxGraphicsDevice->spVxQueues.push_back(vxQueue);
        }                
        if (desiredQueueCount == 0)
        {
            break;
        }
    }
    deviceCreateInfo.queueCreateInfoCount = rpQueueCreateInfos->size();
    deviceCreateInfo.pQueueCreateInfos = rpQueueCreateInfos->data();
    deviceCreateInfo.pEnabledFeatures = &spVxPhysicalDevice->vkPhysicalDeviceFeatures;

    auto extensionsToEnable = nrp(std::vector<char*>);
    for (auto && desiredExtensionToEnable : spVxGraphicsInstance->spCreateInfo->desiredExtensionsToEnable) 
    {
        auto extensionSupported = false;
        for (auto && vkExtension : spVxPhysicalDevice->vkAvailableExtensions) 
        {
            if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
            {
                extensionSupported = true;
                break;
            }           
        }
        if (extensionSupported)
        {
            extensionsToEnable->push_back(strToNewCharArray(desiredExtensionToEnable));
            printf("Desired Extension supported: %s\n", desiredExtensionToEnable.c_str());
        }
        else
        {
            //TODO: Handle this
            printf("Desired Extension not supported: %s\n", desiredExtensionToEnable.c_str());
        }
    }
    deviceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
    deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();
    
    StoreAndAssertVkResultP(spVxGraphicsDevice->createDeviceResult, vkCreateDevice, spVxPhysicalDevice->vkPhysicalDevice, &deviceCreateInfo, nullptr, &spVxGraphicsDevice->vkDevice);
    for (auto && vxQueue : spVxGraphicsDevice->spVxQueues) 
    {
        vkGetDeviceQueue(spVxGraphicsDevice->vkDevice, vxQueue->queueFamilyIndex, vxQueue->queueIndex, &vxQueue->vkQueue);
    }
    for (auto && vxQueueFamily : spVxGraphicsDevice->spVxQueueFamilies) 
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = vxQueueFamily->queueFamilyIndex;                    
        StoreAndAssertVkResultP(vxQueueFamily->vkCreateCommandPoolResult, vkCreateCommandPool, spVxGraphicsDevice->vkDevice, &commandPoolCreateInfo, nullptr, &vxQueueFamily->vkCommandPool);

        VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocateInfo.commandPool = spVxGraphicsDevice->spVxQueueFamilies[0]->vkCommandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;
        auto commandBuffers = nup(VkCommandBuffer[],allocateInfo.commandBufferCount);
        StoreAndAssertVkResultP(vxQueueFamily->vkAllocateCommandBufferResult, vkAllocateCommandBuffers, spVxGraphicsDevice->vkDevice, &allocateInfo, commandBuffers.get());
        vxQueueFamily->vkCommandBuffer = commandBuffers[0];
    }

    return VK_SUCCESS;
}

VkResult vxCreateSwapchain(spt(VxGraphicsSurface) spVxGraphicsSurface, spt(VxGraphicsSwapchain) & spVxGraphicsSwapchain)
{    
    AssertNotNullVkResult(spVxGraphicsSurface);
    AssertNotNullVkResult(spVxGraphicsSurface->vkSurface);

    auto spVxSurfaceDevice = spVxGraphicsSurface->spVxSurfaceDevice;
    AssertNotNullVkResult(spVxSurfaceDevice);
    AssertNotNullVkResult(spVxSurfaceDevice->vkDevice);

    GetAndAssertSharedPointerVk(spVxSurfacePhysicalDevice, spVxGraphicsSurface->wpVxSurfacePhysicalDevice);
    auto vkSurfaceCapabilities = spVxSurfacePhysicalDevice->vkSurfaceCapabilities;
    GetAndAssertSharedPointerVk(spVxGraphicsWindow, spVxGraphicsSurface->wpVxGraphicsWindow);

    spVxGraphicsSwapchain = nsp<VxGraphicsSwapchain>(spVxGraphicsSurface, spVxSurfaceDevice);

    //TODO: how to choose best format?
    spVxGraphicsSwapchain->vkFormat = spVxSurfacePhysicalDevice->vkSurfaceFormats[0];

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = spVxGraphicsSurface->vkSurface;

    uint32_t desiredImageCount = 2;
	createInfo.minImageCount = std::clamp(desiredImageCount, vkSurfaceCapabilities.minImageCount, vkSurfaceCapabilities.maxImageCount==0 ? desiredImageCount : vkSurfaceCapabilities.maxImageCount);;

	createInfo.imageFormat = spVxGraphicsSwapchain->vkFormat.format;
	createInfo.imageColorSpace = spVxGraphicsSwapchain->vkFormat.colorSpace;

    auto windowSize = vxGetWindowSize(spVxGraphicsWindow);
    if (vkSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        createInfo.imageExtent.width = std::clamp(windowSize.width, vkSurfaceCapabilities.minImageExtent.width, vkSurfaceCapabilities.maxImageExtent.width);
        createInfo.imageExtent.height = std::clamp(windowSize.height, vkSurfaceCapabilities.minImageExtent.height, vkSurfaceCapabilities.maxImageExtent.height);
    }
    else
    {
        createInfo.imageExtent = vkSurfaceCapabilities.currentExtent;
    }

	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; //is the number of queue families having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
	createInfo.pQueueFamilyIndices = nullptr; //s an array of queue family indices having access to the images(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT

	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    auto oldSwapchain = spVxGraphicsSwapchain->vkSwapchain;
    createInfo.oldSwapchain = oldSwapchain;

	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    //if not supported, use current transformation
    if (!(vkSurfaceCapabilities.supportedTransforms & createInfo.preTransform))
    {
        createInfo.preTransform = vkSurfaceCapabilities.currentTransform;
    }

    // Find the first supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < 4; i++) {
        if (vkSurfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            createInfo.compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }    

	createInfo.clipped = VK_TRUE;
    //TODO: parametrize the device
    StoreAndAssertVkResultP(spVxGraphicsSwapchain->createSwapchainResult, vkCreateSwapchainKHR, spVxSurfaceDevice->vkDevice, &createInfo, nullptr, &spVxGraphicsSwapchain->vkSwapchain);

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        //TODO: parametrize the device
        vkDestroySwapchainKHR(spVxSurfaceDevice->vkDevice, oldSwapchain, nullptr);
    }

	VkAttachmentDescription attachments[1] = {};
	attachments[0].format = spVxGraphicsSwapchain->vkFormat.format;
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
	VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	renderPassCreateInfo.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
	renderPassCreateInfo.pAttachments = attachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	StoreAndAssertVkResultP(spVxGraphicsSwapchain->createRenderPassResult, vkCreateRenderPass, spVxSurfaceDevice->vkDevice, &renderPassCreateInfo, 0, &spVxGraphicsSwapchain->vkRenderPass);

    uint32_t imageCount;
    StoreAndAssertVkResultP(spVxGraphicsSwapchain->getImagesResult, vkGetSwapchainImagesKHR, spVxSurfaceDevice->vkDevice, spVxGraphicsSwapchain->vkSwapchain, &imageCount, nullptr);
    if (imageCount > 0) 
    {
        spVxGraphicsSwapchain->vkImages = std::vector<VkImage>(imageCount);
        StoreAndAssertVkResultP(spVxGraphicsSwapchain->getImagesResult, vkGetSwapchainImagesKHR, spVxSurfaceDevice->vkDevice, spVxGraphicsSwapchain->vkSwapchain, &imageCount, spVxGraphicsSwapchain->vkImages.data());

        spVxGraphicsSwapchain->vkImageViews = std::vector<VkImageView>(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            createInfo.image = spVxGraphicsSwapchain->vkImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = spVxGraphicsSwapchain->vkFormat.format;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;
            StoreAndAssertVkResultP(spVxGraphicsSwapchain->createImageViewsResult, vkCreateImageView, spVxSurfaceDevice->vkDevice, &createInfo, nullptr, &spVxGraphicsSwapchain->vkImageViews[i]);
        }

        spVxGraphicsSwapchain->vkFramebuffers = std::vector<VkFramebuffer>(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            createInfo.renderPass = spVxGraphicsSwapchain->vkRenderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &spVxGraphicsSwapchain->vkImageViews[i];
            createInfo.width = windowSize.width;
            createInfo.height = windowSize.height;
            createInfo.layers = 1;
            StoreAndAssertVkResultP(spVxGraphicsSwapchain->createFramebuffersResult, vkCreateFramebuffer, spVxSurfaceDevice->vkDevice, &createInfo, nullptr, &spVxGraphicsSwapchain->vkFramebuffers[i]);
        }
    }

    StoreAndAssertVkResultP(spVxGraphicsSwapchain->createAcquireSemaphoreResult, vxCreateSemaphore, spVxSurfaceDevice->vkDevice, &spVxGraphicsSwapchain->vkAcquireSemaphore);
    StoreAndAssertVkResultP(spVxGraphicsSwapchain->createReleaseSemaphoreResult, vxCreateSemaphore, spVxSurfaceDevice->vkDevice, &spVxGraphicsSwapchain->vkReleaseSemaphore);
    
    return VK_SUCCESS;
}
