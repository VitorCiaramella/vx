#ifndef VX_GRAPHICS_INSTANCE_HEADER
#define VX_GRAPHICS_INSTANCE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vulkan/vulkan.hpp>
#include <string>

#include "debug.hpp"
#include "window.hpp"
#include "pipeline.hpp"

using std::string;

struct VxGraphicsWindowCreateInfo;
struct VxGraphicsWindow;
struct VxGraphicsPipelineCreateInfo;
struct VxGraphicsPipeline;

typedef struct VxGraphicsInstanceCreateInfo
{
    std::string                             applicationName;
    uint32_t                                applicationVersion;
    std::string                             engineName;
    uint32_t                                engineVersion;
    uint32_t                                apiVersion;
    vectorT(string)                          desiredLayersToEnable;
    vectorT(string)                          desiredExtensionsToEnable;

    spt(VxGraphicsWindowCreateInfo)         spMainWindowCreateInfo;
    void*                                   rpMainWindowHandle;
    spt(VxGraphicsPipelineCreateInfo)       spVxGraphicsPipelineCreateInfo;
    
    ~VxGraphicsInstanceCreateInfo();
    void destroy();
    void init()
    {
        applicationName = "";
        applicationVersion = 0;
        engineName = "";
        engineVersion = 0;
        apiVersion = VK_API_VERSION_1_0;
        desiredLayersToEnable.reserve(32);
        desiredExtensionsToEnable.reserve(32);
        spMainWindowCreateInfo = nsp<VxGraphicsWindowCreateInfo>();
        rpMainWindowHandle = nullptr;
        spVxGraphicsPipelineCreateInfo = nsp<VxGraphicsPipelineCreateInfo>();
    }
} VxGraphicsInstanceCreateInfo;

typedef struct VxGraphicsLayer
{
    VkLayerProperties                       vkLayer;

    VkResult                                getAvailableExtensionsResult;
    vectorT(VkExtensionProperties)           vkAvailableExtensions;

    ~VxGraphicsLayer();
    void destroy();
    void init()
    {
        vkLayer = {};
        getAvailableExtensionsResult = VK_RESULT_MAX_ENUM; 
    }
} VxGraphicsLayer;

typedef struct VxGraphicsQueueFamily
{
    VkQueueFamilyProperties                 vkQueueFamily;
    uint32_t                                queueFamilyIndex;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    ~VxGraphicsQueueFamily();
    void destroy();
    void init()
    {
        vkQueueFamily = {};
        queueFamilyIndex = 0;
        supportsPresentation = false;
        supportsCompute = false;
        supportsGraphics = false;
    }
} VxGraphicsQueueFamily;

typedef struct VxGraphicsPhysicalDevice
{
    VkPhysicalDevice                        vkPhysicalDevice;
    VkPhysicalDeviceProperties              vkPhysicalDeviceProperties;    
    vectorS(VxGraphicsQueueFamily)          spVxQueueFamilies;
    VkPhysicalDeviceFeatures                vkPhysicalDeviceFeatures;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                getAvailableExtensionResult;
    vectorT(VkExtensionProperties)          vkAvailableExtensions;

    ~VxGraphicsPhysicalDevice();
    void destroy();
    void init()
    {
        vkPhysicalDevice = {};
        vkPhysicalDeviceProperties = {}; 
        spVxQueueFamilies.reserve(32);
        vkPhysicalDeviceFeatures = {};
        supportsCompute = false;
        supportsGraphics = false;
        getAvailableExtensionResult = VK_RESULT_MAX_ENUM;
        vkAvailableExtensions.reserve(32);
    }
} VxGraphicsPhysicalDevice;

typedef struct VxGraphicsInstance 
{
    spt(VxGraphicsInstanceCreateInfo)       spCreateInfo;

    VkResult                                getAvailableLayersResult;
    vectorS(VxGraphicsLayer)                spVxAvailableLayers;

    VkResult                                getAvailableExtensionsResult;
    vectorT(VkExtensionProperties)          vkAvailableExtensions;

    VkResult                                createInstanceResult;
    VkInstance                              vkInstance;

    spt(VxGraphicsDebug)                    spVxGraphicsDebug;

    VkResult                                getAvailablePhysicalDevicesResult;
    vectorS(VxGraphicsPhysicalDevice)       spVxAvailablePhysicalDevices;

    VkResult                                createMainGraphicsWindowResult;
    spt(VxGraphicsWindow)                   spMainVxGraphicsWindow;

    VkResult                                createMainGraphicsPipelineResult;
    spt(VxGraphicsPipeline)                 spMainVxGraphicsPipeline;
    /* 
    PFN_vxWindowLoop                        vxWindowLoopFunction;

    VkResult                                vkAllocateCommandBuffersResult;
    VkCommandBuffer                         vkCommandBuffer;

    VkSemaphore                             acquireSemaphore;
    VkSemaphore                             releaseSemaphore;
    */

    ~VxGraphicsInstance();
    void destroy();
    void init()
    {
        spCreateInfo = nullptr;

        getAvailableLayersResult = VK_RESULT_MAX_ENUM;
        spVxAvailableLayers.reserve(32);

        getAvailableExtensionsResult = VK_RESULT_MAX_ENUM;
        vkAvailableExtensions.reserve(32);

        createInstanceResult = VK_RESULT_MAX_ENUM;
        vkInstance = nullptr;

        spVxGraphicsDebug = nullptr;

        getAvailablePhysicalDevicesResult = VK_RESULT_MAX_ENUM;
        spVxAvailablePhysicalDevices.reserve(32);

        createMainGraphicsWindowResult = VK_RESULT_MAX_ENUM;
        spMainVxGraphicsWindow = nullptr;

        createMainGraphicsPipelineResult = VK_RESULT_MAX_ENUM;
        spMainVxGraphicsPipeline = nullptr;
    }
} VxGraphicsInstance;

VkResult vxCreateGraphicsInstance(spt(VxGraphicsInstanceCreateInfo) spCreateInfo, spt(VxGraphicsInstance) & spVxGraphicsInstance);

#endif

