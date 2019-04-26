#include <vxGraphics/vxGraphics.hpp>
#include "internals.hpp"

VkResult vxSelectSurfacePhysicalDevice(const upt(VxGraphicsInstance) & upGraphicsInstance, VxGraphicsSurface * vxSurface)
{
    std::vector<VxGraphicsSurfacePhysicalDevice*> sortedDevices;
    for (auto && vxSurfacePhysicalDevice : vxSurface->vxSupportedSurfaceDevices) 
    {
        bool deviceHasAllDesiredExtensions = true;
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            bool containExtension = false;
            for (auto && vkExtension : vxSurfacePhysicalDevice.vxPhysicalDevice->vkAvailableExtensions) 
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
            if (vxSurfacePhysicalDevice.vxPhysicalDevice->vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
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
        vxSurface->vxSurfaceDevice = sortedDevices[0];
        return VK_SUCCESS;
    }
    return VK_ERROR_SURFACE_LOST_KHR;
}

VkResult vxCreateSurface(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxSurface = &(upGraphicsInstance->vxMainSurface);
    AssertVkResult(vxCreateSurface_PlatformSpecific, upGraphicsInstance);

    for (auto && vxPhysicalDevice : upGraphicsInstance->vxAvailablePhysicalDevices)
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
                vxSupportedPhysicalDevice.vxSupportedQueueFamilies.push_back(vxQueueFamily);
            }
        }
        if (vxSupportedPhysicalDevice.vxSupportedQueueFamilies.size() > 0)
        {
            vxSupportedPhysicalDevice.vxSurface = vxSurface;
            vxSupportedPhysicalDevice.vxPhysicalDevice = &vxPhysicalDevice;

            uint32_t formatCount;
            StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfaceFormatsKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.vxSurface->vkSurface, &formatCount, nullptr);
            if (formatCount > 0) 
            {
                vxSupportedPhysicalDevice.vkSurfaceFormats = std::vector<VkSurfaceFormatKHR>(formatCount);
                StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfaceFormatsKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.vxSurface->vkSurface, &formatCount, vxSupportedPhysicalDevice.vkSurfaceFormats.data());
            }

            StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.vxSurface->vkSurface, &vxSupportedPhysicalDevice.vkSurfaceCapabilities);

            uint32_t presentModeCount;
            StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfacePresentModesKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.vxSurface->vkSurface, &presentModeCount, nullptr);
            if (presentModeCount > 0) 
            {
                vxSupportedPhysicalDevice.vkPresentModes = std::vector<VkPresentModeKHR>(presentModeCount);
                StoreAndAssertVkResult(vxSupportedPhysicalDevice, vkGetPhysicalDeviceSurfacePresentModesKHR, vxSupportedPhysicalDevice.vxPhysicalDevice->vkPhysicalDevice, vxSupportedPhysicalDevice.vxSurface->vkSurface, &presentModeCount, vxSupportedPhysicalDevice.vkPresentModes.data());
            }

            vxSurface->vxSupportedSurfaceDevices.push_back(vxSupportedPhysicalDevice);
        }
    }

    AssertVkResult(vxSelectSurfacePhysicalDevice, upGraphicsInstance, vxSurface);

    return VK_SUCCESS;
}

