#include <vxGraphics/vxGraphics.hpp>

#include "internals.hpp"

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
    printf("\nCreating vkInstance\n");
    VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

    VkApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = strToNewCharArray(upGraphicsInstance->rpCreateInfo->applicationName);
    applicationInfo.applicationVersion = upGraphicsInstance->rpCreateInfo->applicationVersion;
    applicationInfo.pEngineName = strToNewCharArray(upGraphicsInstance->rpCreateInfo->engineName);
    applicationInfo.engineVersion = upGraphicsInstance->rpCreateInfo->engineVersion;
    applicationInfo.apiVersion = upGraphicsInstance->rpCreateInfo->apiVersion;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    auto layersToEnable = nrp(std::vector<char*>);
    for (auto && desiredLayerToEnable : upGraphicsInstance->rpCreateInfo->desiredLayersToEnable) 
    {
        auto layerSupported = false;
        for (auto && vxLayer : upGraphicsInstance->vxAvailableLayers) 
        {
            if (strcmp(desiredLayerToEnable.c_str(), vxLayer.vkLayer.layerName) == 0) 
            {
                layerSupported = true;
                break;
            }           
        }
        if (layerSupported)
        {
            layersToEnable->push_back(strToNewCharArray(desiredLayerToEnable));
            printf("Desired Layer supported: %s\n", desiredLayerToEnable.c_str());
        }
        else
        {
            //TODO: Handle this
            printf("Desired Layer not supported: %s\n", desiredLayerToEnable.c_str());
        }
    }
    instanceCreateInfo.enabledLayerCount = layersToEnable->size();
    instanceCreateInfo.ppEnabledLayerNames = layersToEnable->data();

    auto extensionsToEnable = nrp(std::vector<char*>);
    for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
    {
        auto extensionSupported = false;
        for (auto && vkExtension : upGraphicsInstance->vkAvailableExtensions) 
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
    instanceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
    instanceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();

    StoreAndAssertVkResultP(upGraphicsInstance, vkCreateInstance, &instanceCreateInfo, NULL, &upGraphicsInstance->vkInstance);

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
    AssertVkVxResult(vxCreateDebugReportCallback, upGraphicsInstance);
    AssertVkVxResult(vxFillAvailableDevices, upGraphicsInstance);

    AssertVxResult(vxCreateWindow, upGraphicsInstance);
    AssertVkVxResult(vxCreateSurface, upGraphicsInstance);

    //AssertVkVxResult(vxCreateDevices, upGraphicsInstance);

    //AssertVkVxResult(vxCreateSwapchain, upGraphicsInstance);

    //AssertVkVxResult(vxLoadShaders, upGraphicsInstance);
    //AssertVkVxResult(vxCreatePipelineLayout, upGraphicsInstance);
    //AssertVkVxResult(vxCreatePipeline, upGraphicsInstance);

    //TODO: parametrize
    //AssertVkVxResult(vxAllocateCommandBuffer, upGraphicsInstance, upGraphicsInstance->vkCommandBuffer);
    //upGraphicsInstance->acquireSemaphore = createSemaphore(upGraphicsInstance->vxDevices[0].vkDevice);
    //upGraphicsInstance->releaseSemaphore = createSemaphore(upGraphicsInstance->vxDevices[0].vkDevice);

    return VxResult::VX_SUCCESS;
}
