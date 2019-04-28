#include "commonHeaders.hpp"

/*

//TODO: move to platform specific
static bool vxQueueFamilySupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	return !!vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
#else
	return true;
#endif
}


VkResult vxSelectSurfacePhysicalDevice(const upt(VxGraphicsInstance) & upGraphicsInstance, VxGraphicsSurface * vxSurface)
{
    std::vector<VxGraphicsSurfacePhysicalDevice*> sortedDevices;
    for (auto && vxSurfacePhysicalDevice : vxSurface->spVxSupportedSurfaceDevices) 
    {
        bool deviceHasAllDesiredExtensions = true;
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            bool containExtension = false;
            for (auto && vkExtension : vxSurfacePhysicalDevice.wpVxPhysicalDevice->vkAvailableExtensions) 
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
            if (vxSurfacePhysicalDevice.wpVxPhysicalDevice->vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                sortedDevices.insert(sortedDevices.begin(), &vxSurfacePhysicalDevice);       
                break;
            }
            else
            {
                sortedDevices.push_back(&vxSurfacePhysicalDevice);
            }           
        }
    }
    if (sortedDevices.size() > 0)
    {
        vxSurface->wpVxSurfaceDevice = sortedDevices[0];
        return VK_SUCCESS;
    }
    return VK_ERROR_SURFACE_LOST_KHR;
}

VkResult vxCreateSurface(const spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsSurface) & spVxGraphicsSurface)
{
    spVxGraphicsSurface = nsp(VxGraphicsSurface);
    spVxGraphicsSurface->wpVxGraphicsInstance = spVxGraphicsInstance;
    spVxGraphicsSurface->wpVxGraphicsWindow = spVxGraphicsInstance->spMainVxGraphicsWindow;
    AssertVkResult(vxCreateSurface_PlatformSpecific, spVxGraphicsSurface);

    for (auto && vxPhysicalDevice : spVxGraphicsSurface->wpVxGraphicsInstance->spVxAvailablePhysicalDevices)
    {
        if (!vxPhysicalDevice.supportsPresentation
            || !vxPhysicalDevice.supportsGraphics)
        {
            continue;
        }

        VxGraphicsSurfacePhysicalDevice vxSupportedPhysicalDevice = {};
        for (auto && vxQueueFamily : vxPhysicalDevice.vxQueueFamilies)
        {
            VkBool32 surfaceSupported = VK_FALSE;
            AssertVkResult(vkGetPhysicalDeviceSurfaceSupportKHR, vxPhysicalDevice.vkPhysicalDevice, vxQueueFamily.queueFamilyIndex, upGraphicsInstance->vxMainSurface.vkSurface, &surfaceSupported);
            if (surfaceSupported)
            {
                vxSupportedPhysicalDevice.wpVxSupportedQueueFamilies.push_back(vxQueueFamily);
            }
        }
        if (vxSupportedPhysicalDevice.wpVxSupportedQueueFamilies.size() > 0)
        {
            vxSupportedPhysicalDevice.wpVxSurface = vxSurface;
            vxSupportedPhysicalDevice.wpVxPhysicalDevice = &vxPhysicalDevice;

            uint32_t formatCount;
            StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfaceFormatsKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.wpVxSurface->vkSurface, &formatCount, nullptr);
            if (formatCount > 0) 
            {
                vxSupportedPhysicalDevice.vkSurfaceFormats = std::vector<VkSurfaceFormatKHR>(formatCount);
                StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfaceFormatsKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.wpVxSurface->vkSurface, &formatCount, vxSupportedPhysicalDevice.vkSurfaceFormats.data());
            }

            StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.wpVxSurface->vkSurface, &vxSupportedPhysicalDevice.vkSurfaceCapabilities);

            uint32_t presentModeCount;
            StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfacePresentModesKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.wpVxSurface->vkSurface, &presentModeCount, nullptr);
            if (presentModeCount > 0) 
            {
                vxSupportedPhysicalDevice.vkPresentModes = std::vector<VkPresentModeKHR>(presentModeCount);
                StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfacePresentModesKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.wpVxSurface->vkSurface, &presentModeCount, vxSupportedPhysicalDevice.vkPresentModes.data());
            }

            vxSurface->spVxSupportedSurfaceDevices.push_back(vxSupportedPhysicalDevice);
        }
    }

    AssertVkResult(vxSelectSurfacePhysicalDevice, upGraphicsInstance, vxSurface);

    return VK_SUCCESS;
}

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
