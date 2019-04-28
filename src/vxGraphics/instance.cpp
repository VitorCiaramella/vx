#include "commonHeaders.hpp"

VxGraphicsInstanceCreateInfo::~VxGraphicsInstanceCreateInfo()
{    
    vxLogInfo2("Destructor call", "Memory");
}

VxGraphicsLayer::~VxGraphicsLayer()
{
    vxLogInfo2("Destructor call", "Memory");
    vkAvailableExtensions.clear();
}

VxGraphicsQueueFamily::~VxGraphicsQueueFamily()
{
    vxLogInfo2("Destructor call", "Memory");
}

VxGraphicsPhysicalDevice::~VxGraphicsPhysicalDevice()
{
    vxLogInfo2("Destructor call", "Memory");
    spVxQueueFamilies.clear();
    vkAvailableExtensions.clear();
}

void vxGraphicsDestroyInstance(VxGraphicsInstance * spVxGraphicsInstance)
{
    spVxGraphicsInstance->spVxGraphicsDebug = nullptr;
    spVxGraphicsInstance->spVxAvailablePhysicalDevices.clear();
    if (spVxGraphicsInstance->vkInstance != nullptr)
    {
        vxLogInfo2("Destroying vkInstance...", "Memory");
        vkDestroyInstance(spVxGraphicsInstance->vkInstance, nullptr);
        spVxGraphicsInstance->vkInstance = nullptr;
        vxLogInfo2("vkInstance destroyed.", "Memory");
    }
    spVxGraphicsInstance->vkAvailableExtensions.clear();
    spVxGraphicsInstance->spVxAvailableLayers.clear();
    spVxGraphicsInstance->spCreateInfo = nullptr;
}

VxGraphicsInstance::~VxGraphicsInstance()
{    
    vxLogInfo2("Destructor call", "Memory");
    vxGraphicsDestroyInstance(this);
}

VkResult vxGetAvailableExtensions(char* pLayerName, vector(VkExtensionProperties) & vkAvailableExtensions)
{
    vxLogInfo3("Getting available extensions for layer '%s'...", "Vulkan", pLayerName);
    uint32_t extensionCount;
    AssertVkResult(vkEnumerateInstanceExtensionProperties, pLayerName, &extensionCount, nullptr);
    if (extensionCount > 0) 
    {
        vkAvailableExtensions.resize(extensionCount);
        AssertVkResult(vkEnumerateInstanceExtensionProperties, pLayerName, &extensionCount, vkAvailableExtensions.data());
    }
    vxLogInfo3("Extensions found: %i for layer '%s'.", "Vulkan", extensionCount, pLayerName);
    return VK_SUCCESS;
}

VkResult vxGetAvailableExtensions(vector(VkExtensionProperties) & vkAvailableExtensions)
{
    return vxGetAvailableExtensions(nullptr, vkAvailableExtensions);
}

VkResult vxGetAvailableLayers(vectorS(VxGraphicsLayer) & spVxAvailableLayers)
{
    vxLogInfo2("Getting available layers...", "Vulkan");
    uint32_t layerCount;
    AssertVkResult(vkEnumerateInstanceLayerProperties, &layerCount, nullptr);
    if (layerCount > 0) 
    {
        auto availableLayers = nup(VkLayerProperties[],layerCount);
        AssertVkResult(vkEnumerateInstanceLayerProperties, &layerCount, availableLayers.get());
        CopyResultItemsSP(spVxAvailableLayers, vkLayer, VxGraphicsLayer, availableLayers, layerCount);

        for (auto && vxLayer : spVxAvailableLayers) 
        {
            StoreAndAssertVkResultP(vxLayer->getAvailableExtensionsResult, vxGetAvailableExtensions, vxLayer->vkLayer.layerName, vxLayer->vkAvailableExtensions);
        }
    }
    vxLogInfo3("Layers found: %i.", "Vulkan", layerCount);
    return VK_SUCCESS;
}

VkResult vxCreateVkInstance(const spt(VxGraphicsInstance) & spVxGraphicsInstance, VkInstance * vkInstance)
{
    vxLogInfo2("Creating Vulkan instance...", "Vulkan");
    VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

    VkApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = strToNewCharArray(spVxGraphicsInstance->spCreateInfo->applicationName);
    applicationInfo.applicationVersion = spVxGraphicsInstance->spCreateInfo->applicationVersion;
    applicationInfo.pEngineName = strToNewCharArray(spVxGraphicsInstance->spCreateInfo->engineName);
    applicationInfo.engineVersion = spVxGraphicsInstance->spCreateInfo->engineVersion;
    applicationInfo.apiVersion = spVxGraphicsInstance->spCreateInfo->apiVersion;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    vxLogInfo2("Checking desired layers to enable...", "Vulkan");
    auto layersToEnable = nrp(std::vector<char*>);
    for (auto && desiredLayerToEnable : spVxGraphicsInstance->spCreateInfo->desiredLayersToEnable) 
    {
        auto layerSupported = false;
        for (auto && vxLayer : spVxGraphicsInstance->spVxAvailableLayers) 
        {
            if (strcmp(desiredLayerToEnable.c_str(), vxLayer->vkLayer.layerName) == 0) 
            {
                layerSupported = true;
                break;
            }           
        }
        if (layerSupported)
        {
            layersToEnable->push_back(strToNewCharArray(desiredLayerToEnable));
            vxLogInfo3("Desired layer supported: %s.", "Vulkan", desiredLayerToEnable.c_str());
        }
        else
        {
            //TODO: Handle this
            vxLogInfo3("Desired layer NOT supported: %s.", "Vulkan", desiredLayerToEnable.c_str());
        }
    }
    instanceCreateInfo.enabledLayerCount = layersToEnable->size();
    instanceCreateInfo.ppEnabledLayerNames = layersToEnable->data();

    vxLogInfo2("Checking desired extensions to enable...", "Vulkan");
    auto extensionsToEnable = nrp(std::vector<char*>);
    for (auto && desiredExtensionToEnable : spVxGraphicsInstance->spCreateInfo->desiredExtensionsToEnable) 
    {
        auto extensionSupported = false;
        for (auto && vkExtension : spVxGraphicsInstance->vkAvailableExtensions) 
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
            vxLogInfo3("Desired extension supported: %s.", "Vulkan", desiredExtensionToEnable.c_str());
        }
        else
        {
            //TODO: Handle this
            vxLogInfo3("Desired extension NOT supported: %s.", "Vulkan", desiredExtensionToEnable.c_str());
        }
        
    }
    instanceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
    instanceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();

    AssertVkResult(vkCreateInstance, &instanceCreateInfo, NULL, vkInstance);
    vxLogInfo2("Vulkan Instance created.", "Vulkan");
    return VK_SUCCESS;
}

VkResult vxGetAvailablePhysicalDevices(const spt(VxGraphicsInstance) & spVxGraphicsInstance, vectorS(VxGraphicsPhysicalDevice) & spVxAvailablePhysicalDevices)
{
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsDebug->wpVxGraphicsInstance);
    vxLogInfo2("Getting available physical devices...", "Vulkan");
    uint32_t deviceCount;
    AssertVkResult(vkEnumeratePhysicalDevices, spVxGraphicsInstance->vkInstance, &deviceCount, nullptr);
    if (deviceCount > 0) 
    {
        auto availableDevices = nup(VkPhysicalDevice[],deviceCount);
        AssertVkResult(vkEnumeratePhysicalDevices, spVxGraphicsInstance->vkInstance, &deviceCount, availableDevices.get());
        CopyResultItemsSP(spVxAvailablePhysicalDevices, vkPhysicalDevice, VxGraphicsPhysicalDevice, availableDevices, deviceCount);
        vxLogInfo3("Physical devices found: %i.", "Vulkan", deviceCount);

        vxLogInfo2("Getting physical devices properties...", "Vulkan");
        for (auto && vxPhysicalDevice : spVxAvailablePhysicalDevices) 
        {
            vxPhysicalDevice->supportsCompute = false;
            vxPhysicalDevice->supportsGraphics = false;
            
            vkGetPhysicalDeviceProperties(vxPhysicalDevice->vkPhysicalDevice, &vxPhysicalDevice->vkPhysicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(vxPhysicalDevice->vkPhysicalDevice, &vxPhysicalDevice->vkPhysicalDeviceFeatures);

            vxLogInfo2("Getting physical device queue families...", "Vulkan");
            uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(vxPhysicalDevice->vkPhysicalDevice, &queueFamilyPropertyCount, nullptr);
            if (queueFamilyPropertyCount > 0)
            {
                auto queueFamilyProperties = nup(VkQueueFamilyProperties[],queueFamilyPropertyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(vxPhysicalDevice->vkPhysicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.get());
                CopyResultItemsSP(vxPhysicalDevice->spVxQueueFamilies, vkQueueFamily, VxGraphicsQueueFamily, queueFamilyProperties, queueFamilyPropertyCount);

                uint32_t queueFamilyIndex = 0; 
                for (auto && vxQueueFamily : vxPhysicalDevice->spVxQueueFamilies) 
                {
                    vxQueueFamily->queueFamilyIndex = queueFamilyIndex++;
                    vxQueueFamily->supportsCompute = vxQueueFamily->vkQueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
                    vxQueueFamily->supportsGraphics = vxQueueFamily->vkQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;

                    vxPhysicalDevice->supportsCompute = vxPhysicalDevice->supportsCompute | vxQueueFamily->supportsPresentation;
                    vxPhysicalDevice->supportsGraphics = vxPhysicalDevice->supportsGraphics | vxQueueFamily->supportsCompute;
                }
            }
            vxLogInfo3("Physical device queue families: %i", "Vulkan", queueFamilyPropertyCount);

            vxLogInfo2("Getting physical device extensions...", "Vulkan");
            uint32_t extensionCount;
            StoreAndAssertVkResultP(vxPhysicalDevice->getAvailableExtensionResult, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice->vkPhysicalDevice, nullptr, &extensionCount, nullptr);
            if (extensionCount > 0) 
            {
                vxPhysicalDevice->vkAvailableExtensions = std::vector<VkExtensionProperties>(extensionCount);
                StoreAndAssertVkResultP(vxPhysicalDevice->getAvailableExtensionResult, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice->vkPhysicalDevice, nullptr, &extensionCount, vxPhysicalDevice->vkAvailableExtensions.data());
            }            
            for (auto && vxLayer : spVxGraphicsInstance->spVxAvailableLayers) 
            {
                vxPhysicalDevice->getAvailableExtensionResult = vkEnumerateDeviceExtensionProperties(vxPhysicalDevice->vkPhysicalDevice, vxLayer->vkLayer.layerName, &extensionCount, nullptr);
                if (extensionCount > 0) 
                {
                    auto baseIndex = vxPhysicalDevice->vkAvailableExtensions.size();
                    vxPhysicalDevice->vkAvailableExtensions.reserve(vxPhysicalDevice->vkAvailableExtensions.size()+extensionCount);
                    StoreAndAssertVkResultP(vxPhysicalDevice->getAvailableExtensionResult, vkEnumerateDeviceExtensionProperties, vxPhysicalDevice->vkPhysicalDevice, vxLayer->vkLayer.layerName, &extensionCount, &vxPhysicalDevice->vkAvailableExtensions[baseIndex]);
                }            
            }
            vxLogInfo3("Physical device extensions: %i.", "Vulkan", vxPhysicalDevice->vkAvailableExtensions.size());
        }
    }
    return VK_SUCCESS;
}

VkResult vxCreateGraphicsInstance(spt(VxGraphicsInstanceCreateInfo) spCreateInfo, spt(VxGraphicsInstance) & spVxGraphicsInstance)
{
    spVxGraphicsInstance = nsp<VxGraphicsInstance>();
    spVxGraphicsInstance->spCreateInfo = spCreateInfo;
    spVxGraphicsInstance->spVxGraphicsDebug = nsp<VxGraphicsDebug>();
    spVxGraphicsInstance->spVxGraphicsDebug->wpVxGraphicsInstance = spVxGraphicsInstance;

    StoreAndAssertVkResultP(spVxGraphicsInstance->getAvailableLayersResult, vxGetAvailableLayers, spVxGraphicsInstance->spVxAvailableLayers);
    StoreAndAssertVkResultP(spVxGraphicsInstance->getAvailableExtensionsResult, vxGetAvailableExtensions, spVxGraphicsInstance->vkAvailableExtensions);
    StoreAndAssertVkResultP(spVxGraphicsInstance->createInstanceResult, vxCreateVkInstance, spVxGraphicsInstance, &spVxGraphicsInstance->vkInstance);

    AssertVkResult(vxCreateDebugReportCallback, spVxGraphicsInstance->spVxGraphicsDebug);

    StoreAndAssertVkResultP(spVxGraphicsInstance->getAvailablePhysicalDevicesResult, vxGetAvailablePhysicalDevices, spVxGraphicsInstance, spVxGraphicsInstance->spVxAvailablePhysicalDevices);

    //AssertVxResult(vxCreateWindow, spVxGraphicsInstance);
    //AssertVkVxResult(vxCreateSurface, spVxGraphicsInstance, &spVxGraphicsInstance->spVxMainSurface);

    //AssertVkVxResult(vxCreateDevices, spVxGraphicsInstance);

    //AssertVkVxResult(vxCreateSwapchain, upGraphicsInstance);

    //AssertVkVxResult(vxLoadShaders, upGraphicsInstance);
    //AssertVkVxResult(vxCreatePipelineLayout, upGraphicsInstance);
    //AssertVkVxResult(vxCreatePipeline, upGraphicsInstance);

    //TODO: parametrize
    //AssertVkVxResult(vxAllocateCommandBuffer, upGraphicsInstance, upGraphicsInstance->vkCommandBuffer);
    //upGraphicsInstance->acquireSemaphore = createSemaphore(upGraphicsInstance->vxDevices[0].vkDevice);
    //upGraphicsInstance->releaseSemaphore = createSemaphore(upGraphicsInstance->vxDevices[0].vkDevice);

    return VK_SUCCESS;
}

VkResult vxGraphicsDestroyInstance(spt(VxGraphicsInstance) & spVxGraphicsInstance)
{
    if (spVxGraphicsInstance != nullptr)
    {
        vxGraphicsDestroyInstance(spVxGraphicsInstance.get());
        spVxGraphicsInstance = nullptr;
    }
    return VK_SUCCESS;
}

VkResult vxGraphicsTerminate(spt(VxGraphicsInstance) & spVxGraphicsInstance)
{
    vxGraphicsDestroyInstance(spVxGraphicsInstance);
    glfwTerminate();    
    return VK_SUCCESS;
}

VkResult vxGetVkSuccess()
{
    return VK_SUCCESS;
}
