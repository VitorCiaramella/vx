#include "commonHeaders.hpp"

VxGraphicsSurfacePhysicalDevice::~VxGraphicsSurfacePhysicalDevice()
{
    vxLogInfo2("Destructor call", "Memory");
    wpVxSupportedQueueFamilies.clear();
    vkSurfaceFormats.clear();
    vkPresentModes.clear();
}

VxGraphicsSurface::~VxGraphicsSurface()
{
    vxLogInfo2("Destructor call", "Memory");
    if (vkSurface != nullptr)
    {
        GetAndAssertSharedPointer(spVxGraphicsInstance, wpVxGraphicsInstance);
        AssertTrue(spVxGraphicsInstance->vkInstance!=nullptr);
        vkDestroySurfaceKHR(spVxGraphicsInstance->vkInstance, vkSurface, nullptr);
        vkSurface = nullptr;
    }
    spVxSupportedSurfaceDevices.clear();
}

VxGraphicsWindowCreateInfo::~VxGraphicsWindowCreateInfo()
{
    vxLogInfo2("Destructor call", "Memory");
}

void vxGraphicsDestroyWindow(VxGraphicsWindow * rpVxGraphicsWindow)
{
    if (rpVxGraphicsWindow->rpGlfwWindow != nullptr)
    {
        vxLogInfo2("Destroying glfwWindow...", "Memory");
        glfwDestroyWindow(rpVxGraphicsWindow->rpGlfwWindow);
        rpVxGraphicsWindow->rpGlfwWindow  = nullptr;
        vxLogInfo2("GlfwWindow destroyed.", "Memory");
    }
}

VxGraphicsWindow::~VxGraphicsWindow()
{
    vxLogInfo2("Destructor call", "Memory");
    vxGraphicsDestroyWindow(this);
}

VkResult vxGraphicsDestroyWindow(spt(VxGraphicsWindow) & spVxGraphicsWindow)
{
    if (spVxGraphicsWindow != nullptr)
    {
        vxGraphicsDestroyWindow(spVxGraphicsWindow.get());
        spVxGraphicsWindow = nullptr;
    }
    return VK_SUCCESS;
}

void vxGlfwErrorCallback(int error, const char* description)
{
    vxLogError3("%i: %s", "GLFW", error, description);
}

VkResult vxCreateGraphicsWindow(spt(VxGraphicsWindowCreateInfo) spVxGraphicsWindowCreateInfo, spt(VxGraphicsWindow) & spVxGraphicsWindow)
{
    glfwSetErrorCallback(vxGlfwErrorCallback);

    if (!glfwInit())
    {
        return VkResult::VK_ERROR_INITIALIZATION_FAILED;
    }

    glfwWindowHint(GLFW_RESIZABLE, spVxGraphicsWindowCreateInfo->resizable);
    glfwWindowHint(GLFW_VISIBLE, spVxGraphicsWindowCreateInfo->visible);
    glfwWindowHint(GLFW_DECORATED, spVxGraphicsWindowCreateInfo->decorated);
    glfwWindowHint(GLFW_FOCUSED, spVxGraphicsWindowCreateInfo->focused);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, spVxGraphicsWindowCreateInfo->focusOnShow);
    glfwWindowHint(GLFW_CENTER_CURSOR, spVxGraphicsWindowCreateInfo->centerCursor);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, spVxGraphicsWindowCreateInfo->scaleToMonitor);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, spVxGraphicsWindowCreateInfo->transparentFramebuffer);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto rpGLFWmonitor = glfwGetPrimaryMonitor();
    AssertTrueVkResult(rpGLFWmonitor != nullptr);

    auto rpGLFWvidmode = glfwGetVideoMode(rpGLFWmonitor);
    AssertTrueVkResult(rpGLFWvidmode != nullptr);

    glfwWindowHint(GLFW_RED_BITS, rpGLFWvidmode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, rpGLFWvidmode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, rpGLFWvidmode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, rpGLFWvidmode->refreshRate);

    GLFWwindow *rpGLFWwindow;
    if (spVxGraphicsWindowCreateInfo->fullScreen)
    {
        rpGLFWwindow = glfwCreateWindow(rpGLFWvidmode->width, rpGLFWvidmode->height, spVxGraphicsWindowCreateInfo->title, rpGLFWmonitor, nullptr);
    }
    else
    {
        rpGLFWwindow = glfwCreateWindow(spVxGraphicsWindowCreateInfo->initialWidth, spVxGraphicsWindowCreateInfo->initialHeight, spVxGraphicsWindowCreateInfo->title, nullptr, nullptr);
    }
    AssertTrueVkResult(rpGLFWwindow != nullptr);

    spVxGraphicsWindow = nsp<VxGraphicsWindow>();
    spVxGraphicsWindow->rpGlfwWindow = rpGLFWwindow;

    return VkResult::VK_SUCCESS;
}


VkResult vxSelectSurfacePhysicalDevice(spt(VxGraphicsSurface) spVxGraphicsSurface)
{
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);
    vectorW(VxGraphicsSurfacePhysicalDevice) sortedDevices;
    for (auto && vxSurfacePhysicalDevice : spVxGraphicsSurface->spVxSupportedSurfaceDevices) 
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
        spVxGraphicsSurface->wpVxSurfaceDevice = sortedDevices[0];
        return VK_SUCCESS;
    }
    return VK_ERROR_SURFACE_LOST_KHR;
}

VkResult vxCreateGraphicsSurface(const spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsSurface) & spVxGraphicsSurface)
{
    spVxGraphicsSurface = nsp<VxGraphicsSurface>();
    spVxGraphicsSurface->wpVxGraphicsInstance = spVxGraphicsInstance;
    spVxGraphicsSurface->wpVxGraphicsWindow = spVxGraphicsInstance->spMainVxGraphicsWindow;
    AssertVkResult(vxCreateSurface_PlatformSpecific, spVxGraphicsSurface);
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

            spVxGraphicsSurface->spVxSupportedSurfaceDevices.push_back(spVxSupportedPhysicalDevice);
        }
    }

    AssertVkResult(vxSelectSurfacePhysicalDevice, spVxGraphicsSurface);

    return VK_SUCCESS;
}

/*





VkResult vxCreateSurfaceDevice(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    printf("\nCreating Device\n");
 
    uint32_t desiredDeviceCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredDeviceCount,(uint32_t)1,(uint32_t)upGraphicsInstance->vxSelectedPhysicalDevices.size());
    for (auto && vxPhysicalDevice : upGraphicsInstance->vxSelectedPhysicalDevices) 
    {
        VxGraphicsDevice vxDevice = {};
        initVxGraphicsDevice(vxDevice);
        vxDevice.wpVxPhysicalDevice = vxPhysicalDevice;

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
            auto extensionSupported = false;
            for (auto && vkExtension : vxPhysicalDevice->vkAvailableExtensions) 
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
        
        StoreAndAssertVkResultP(upGraphicsInstance, vkCreateDevice, vxDevice.wpVxPhysicalDevice->vkPhysicalDevice, &deviceCreateInfo, nullptr, &vxDevice.vkDevice);
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
*/


/*



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

VkResult vxAwaitWindowClose(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    if (upGraphicsInstance->vxWindowLoopFunction == nullptr)
    {
        return VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE;
    }
    while (!glfwWindowShouldClose(upGraphicsInstance->mainWindow))
    {
        glfwPollEvents();
        upGraphicsInstance->windowSize = vxGetWindowSize(upGraphicsInstance);
        auto loopResult = upGraphicsInstance->vxWindowLoopFunction(upGraphicsInstance);        
        if (loopResult != VxWindowLoopResult::VX_WL_CONTINUE)
        {
            break;
        }
    }
    return VkResult::VK_SUCCESS;
}
*/