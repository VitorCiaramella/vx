#include <vxGraphics/vxGraphics.hpp>

#include <string>
#include <algorithm>

VkResult vxFillAvailableExtensions(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    uint32_t extensionCount;
    StoreAndAssertVkResultP(puGraphicsInstance, vkEnumerateInstanceExtensionProperties, nullptr, &extensionCount, nullptr);
    if (extensionCount > 0) 
    {
        auto availableExtensions = nup(VkExtensionProperties[],extensionCount);
        StoreAndAssertVkResultP(puGraphicsInstance, vkEnumerateInstanceExtensionProperties, nullptr, &extensionCount, availableExtensions.get());
        CopyResultItems(puGraphicsInstance->availableExtensions, extension, VxGraphicsExtension, availableExtensions, extensionCount);
    }
    return VK_SUCCESS;
}

VkResult vxFillAvailableLayers(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    uint32_t layerCount;
    StoreAndAssertVkResultP(puGraphicsInstance, vkEnumerateInstanceLayerProperties, &layerCount, nullptr);
    if (layerCount > 0) 
    {
        auto availableLayers = nup(VkLayerProperties[],layerCount);
        StoreAndAssertVkResultP(puGraphicsInstance, vkEnumerateInstanceLayerProperties, &layerCount, availableLayers.get());
        CopyResultItems(puGraphicsInstance->availableLayers, layer, VxGraphicsLayer, availableLayers, layerCount);

        for (auto && availableLayer : puGraphicsInstance->availableLayers) 
        {
            uint32_t extensionCount;
            StoreAndAssertVkResult(availableLayer, vkEnumerateInstanceExtensionProperties, availableLayer.layer.layerName, &extensionCount, nullptr);
            if (extensionCount > 0) 
            {
                auto availableExtensions = nup(VkExtensionProperties[],extensionCount);
                StoreAndAssertVkResult(availableLayer, vkEnumerateInstanceExtensionProperties, availableLayer.layer.layerName, &extensionCount, availableExtensions.get());
                CopyResultItems(availableLayer.availableExtensions, extension, VxGraphicsExtension, availableExtensions, extensionCount);
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

VkResult vxFillAvailableDevices(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    uint32_t deviceCount;
    StoreAndAssertVkResultP(puGraphicsInstance, vkEnumeratePhysicalDevices, puGraphicsInstance->vkInstance, &deviceCount, nullptr);
    if (deviceCount > 0) 
    {
        auto availableDevices = nup(VkPhysicalDevice[],deviceCount);
        StoreAndAssertVkResultP(puGraphicsInstance, vkEnumeratePhysicalDevices, puGraphicsInstance->vkInstance, &deviceCount, availableDevices.get());
        CopyResultItems(puGraphicsInstance->availableDevices, device, VxGraphicsPhysicalDevice, availableDevices, deviceCount);

        for (auto && device : puGraphicsInstance->availableDevices) 
        {
            device.supportsCompute = false;
            device.supportsGraphics = false;
            device.supportsPresentation = false;
            
            vkGetPhysicalDeviceProperties(device.device, &device.deviceProperties);
            vkGetPhysicalDeviceFeatures(device.device, &device.deviceFeatures);

            uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(device.device, &queueFamilyPropertyCount, nullptr);
            if (queueFamilyPropertyCount > 0)
            {
                auto queueFamilyProperties = nup(VkQueueFamilyProperties[],queueFamilyPropertyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(device.device, &queueFamilyPropertyCount, queueFamilyProperties.get());
                CopyResultItems(device.queueFamilies, queueFamily, VxGraphicsQueueFamily, queueFamilyProperties, queueFamilyPropertyCount);

                uint32_t queueFamilyIndex = 0; 
                for (auto && queueFamily : device.queueFamilies) 
                {
                    queueFamily.queueFamilyIndex = queueFamilyIndex++;
                    queueFamily.supportsPresentation = vxQueueFamilySupportsPresentation(device.device, queueFamily.queueFamilyIndex);
                    queueFamily.supportsCompute = queueFamily.queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
                    queueFamily.supportsGraphics = queueFamily.queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;

                    device.supportsCompute = device.supportsCompute | queueFamily.supportsPresentation;
                    device.supportsGraphics = device.supportsGraphics | queueFamily.supportsCompute;
                    device.supportsPresentation = device.supportsPresentation | queueFamily.supportsGraphics;
                }
            }

            uint32_t extensionCount;
            StoreAndAssertVkResult(device, vkEnumerateDeviceExtensionProperties, device.device, nullptr, &extensionCount, nullptr);
            if (extensionCount > 0) 
            {
                auto availableExtensions = nup(VkExtensionProperties[],extensionCount);
                StoreAndAssertVkResult(device, vkEnumerateDeviceExtensionProperties, device.device, nullptr, &extensionCount, availableExtensions.get());
                CopyResultItems(device.availableExtensions, extension, VxGraphicsExtension, availableExtensions, extensionCount);
            }            
            for (auto && availableLayer : puGraphicsInstance->availableLayers) 
            {
                StoreAndAssertVkResult(device, vkEnumerateDeviceExtensionProperties, device.device, availableLayer.layer.layerName, &extensionCount, nullptr);
                if (extensionCount > 0) 
                {
                    auto availableExtensions = nup(VkExtensionProperties[],extensionCount);
                    StoreAndAssertVkResult(device, vkEnumerateDeviceExtensionProperties, device.device, availableLayer.layer.layerName, &extensionCount, availableExtensions.get());
                    AppendResultItems(device.availableExtensions, extension, VxGraphicsExtension, availableExtensions, extensionCount);
                }            
            }
        }
    }
    return VK_SUCCESS;
}

VkResult vxCreateVkInstance(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    VkInstanceCreateInfo instanceCreateInfo = {};

    VkApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = strdup(puGraphicsInstance->rpCreateInfo->applicationName.c_str());
    applicationInfo.applicationVersion = puGraphicsInstance->rpCreateInfo->applicationVersion;
    applicationInfo.pEngineName = strdup(puGraphicsInstance->rpCreateInfo->engineName.c_str());
    applicationInfo.engineVersion = puGraphicsInstance->rpCreateInfo->engineVersion;
    applicationInfo.apiVersion = puGraphicsInstance->rpCreateInfo->apiVersion;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    auto layersToEnable = nrp(std::vector<char*>);
    for (auto && desiredLayerToEnable : puGraphicsInstance->rpCreateInfo->desiredLayersToEnable) 
    {
        for (auto && availableLayer : puGraphicsInstance->availableLayers) 
        {
            if (strcmp(desiredLayerToEnable.c_str(), availableLayer.layer.layerName) == 0) 
            {
                layersToEnable->push_back(strdup(desiredLayerToEnable.c_str()));
            }           
        }
    }
    instanceCreateInfo.enabledLayerCount = layersToEnable->size();
    instanceCreateInfo.ppEnabledLayerNames = layersToEnable->data();

    auto extensionsToEnable = nrp(std::vector<char*>);
    for (auto && desiredExtensionToEnable : puGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
    {
        for (auto && availableExtension : puGraphicsInstance->availableExtensions) 
        {
            if (strcmp(desiredExtensionToEnable.c_str(), availableExtension.extension.extensionName) == 0) 
            {
                extensionsToEnable->push_back(strdup(desiredExtensionToEnable.c_str()));
            }           
        }
    }
    instanceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
    instanceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();

    StoreAndAssertVkResultP(puGraphicsInstance, vkCreateInstance, &instanceCreateInfo, NULL, &puGraphicsInstance->vkInstance);

    return VK_SUCCESS;
}

VkResult vxSelectPhysicalDevices(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    for (auto && availableDevice : puGraphicsInstance->availableDevices) 
    {
        bool deviceHasAllDesiredExtensions = true;
        for (auto && desiredExtensionToEnable : puGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            bool containExtension = false;
            for (auto && availableExtension : availableDevice.availableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), availableExtension.extension.extensionName) == 0) 
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
            //&& availableDevice.deviceProperties.apiVersion >= puGraphicsInstance->rpCreateInfo->apiVersion
            && availableDevice.supportsGraphics
            && availableDevice.supportsPresentation
            )
        {
            //prioritize discrete devices
            //TODO: create a sorting based on device type and memory
            if (availableDevice.deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                puGraphicsInstance->selectedAvailableDevices.insert(puGraphicsInstance->selectedAvailableDevices.begin(), &availableDevice);            
            }
            else
            {
                puGraphicsInstance->selectedAvailableDevices.push_back(&availableDevice);
            }           
        }
    }
    return VK_SUCCESS;
}

VkResult vxCreateDevices(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    uint32_t desiredDeviceCount = std::clamp(puGraphicsInstance->rpCreateInfo->desiredDeviceCount,(uint32_t)1,(uint32_t)puGraphicsInstance->selectedAvailableDevices.size());
    for (auto && availableDevice : puGraphicsInstance->selectedAvailableDevices) 
    {
        VxGraphicsDevice vxDevice = {};
        initVxGraphicsDevice(vxDevice);

        //TODO: instead of global queue count, do per desired device type
        uint32_t desiredQueueCount = std::clamp(puGraphicsInstance->rpCreateInfo->desiredQueueCountPerDevice,(uint32_t)1,(uint32_t)100);

        VkDeviceCreateInfo deviceCreateInfo = {};

        auto rpQueueCreateInfos = nrp(std::vector<VkDeviceQueueCreateInfo>);
        for (auto && queueFamily : availableDevice->queueFamilies) 
        {
            if (queueFamily.supportsPresentation
                && queueFamily.supportsGraphics)
            {
                uint32_t queueCount = std::clamp(desiredQueueCount,(uint32_t)1,queueFamily.queueFamily.queueCount);
                desiredQueueCount = std::clamp(desiredQueueCount-queueCount,(uint32_t)0,(uint32_t)100);
                float queuePriority = 1.f;
                auto queuePriorities = nrp(std::vector<float>,queueCount,queuePriority);

                VkDeviceQueueCreateInfo queueCreateInfo = {};
                queueCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;
                queueCreateInfo.queueCount = queueCount;
                queueCreateInfo.pQueuePriorities = queuePriorities->data();
                rpQueueCreateInfos->push_back(queueCreateInfo);

                vxDevice.queueFamilies.push_back(queueFamily);
                for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
                {
                    VxGraphicsQueue vxQueue = {};
                    initVxGraphicsQueue(vxQueue);
                    vxQueue.queueFamilyIndex = queueFamily.queueFamilyIndex;
                    vxQueue.queueIndex = queueIndex;
                    vxDevice.queues.push_back(vxQueue);
                }                
            }
            if (desiredQueueCount == 0)
            {
                break;
            }
        }
        deviceCreateInfo.queueCreateInfoCount = rpQueueCreateInfos->size();
        deviceCreateInfo.pQueueCreateInfos = rpQueueCreateInfos->data();
        deviceCreateInfo.pEnabledFeatures = &availableDevice->deviceFeatures;

        auto extensionsToEnable = nrp(std::vector<char*>);
        for (auto && desiredExtensionToEnable : puGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            for (auto && availableExtension : availableDevice->availableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), availableExtension.extension.extensionName) == 0) 
                {
                    extensionsToEnable->push_back(strdup(desiredExtensionToEnable.c_str()));
                }           
            }
        }
        deviceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
        deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();

        VkDevice vkDevice;
        StoreAndAssertVkResultP(puGraphicsInstance, vkCreateDevice, availableDevice->device, &deviceCreateInfo, nullptr, &vkDevice);
        vxDevice.device = vkDevice;
        for (auto && queue : vxDevice.queues) 
        {
            vkGetDeviceQueue(vxDevice.device, queue.queueFamilyIndex, queue.queueIndex, &queue.queue);
        }
        for (auto && queueFamily : vxDevice.queueFamilies) 
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo = {};
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;                    
            StoreAndAssertVkResult(queueFamily, vkCreateCommandPool, vxDevice.device, &commandPoolCreateInfo, nullptr, &queueFamily.commandPool);
        }

        puGraphicsInstance->devices.push_back(vxDevice);

        if (puGraphicsInstance->devices.size() >= desiredDeviceCount) 
        {
            break;
        }        
    }
    return VK_SUCCESS;
}

VxResult vxCreateGraphicsInstance(rpt(VxGraphicsInstanceCreateInfo) rpCreateInfo, upt(VxGraphicsInstance) & puGraphicsInstance)
{
    puGraphicsInstance = nup(VxGraphicsInstance);
    initVxGraphicsInstance(puGraphicsInstance);
    puGraphicsInstance->rpCreateInfo = rpCreateInfo;

    AssertVkVxResult(vxFillAvailableLayers, puGraphicsInstance);
    AssertVkVxResult(vxFillAvailableExtensions, puGraphicsInstance);
    AssertVkVxResult(vxCreateVkInstance, puGraphicsInstance);
    AssertVkVxResult(vxFillAvailableDevices, puGraphicsInstance);

    AssertVkVxResult(vxSelectPhysicalDevices, puGraphicsInstance);
    AssertVkVxResult(vxCreateDevices, puGraphicsInstance);

    return VxResult::VX_SUCCESS;
}

VxResult vxAllocateCommandBuffer(const upt(VxGraphicsInstance) & puGraphicsInstance, VkCommandBuffer & commandBuffer)
{
    auto device = puGraphicsInstance->devices[0];

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.commandPool = device.queueFamilies[0].commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    auto commandBuffers = nup(VkCommandBuffer[],allocateInfo.commandBufferCount);
    AssertVkVxResult(vkAllocateCommandBuffers, device.device, &allocateInfo, commandBuffers.get());

    commandBuffer = commandBuffers[0];

    return VxResult::VX_SUCCESS;
}


