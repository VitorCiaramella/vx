#ifndef VX_GRAPHICS_INSTANCE_HEADER
#define VX_GRAPHICS_INSTANCE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <string>

#include "debug.hpp"
#include "window.hpp"

struct VxGraphicsWindowCreateInfo;
struct VxGraphicsWindow;

typedef struct VxGraphicsInstanceCreateInfo
{
    std::string                             applicationName;
    uint32_t                                applicationVersion;
    std::string                             engineName;
    uint32_t                                engineVersion;
    uint32_t                                apiVersion;
    std::vector<std::string>                desiredLayersToEnable;
    std::vector<std::string>                desiredExtensionsToEnable;
    uint32_t                                desiredDeviceCount;
    uint32_t                                desiredQueueCountPerDevice;

    spt(VxGraphicsWindowCreateInfo)         spMainWindowCreateInfo;

    std::vector<std::string>                shadersFilePaths;

    ~VxGraphicsInstanceCreateInfo();
    void init()
    {
        applicationName = "";
        applicationVersion = 0;
        engineName = "";
        engineVersion = 0;
        apiVersion = VK_API_VERSION_1_0;
        desiredLayersToEnable.reserve(32);
        desiredExtensionsToEnable.reserve(32);
        desiredDeviceCount = 1;
        desiredQueueCountPerDevice = 1;
        shadersFilePaths.reserve(32);
        spMainWindowCreateInfo = nsp<VxGraphicsWindowCreateInfo>();
    }
} VxGraphicsInstanceCreateInfo;

typedef struct VxGraphicsLayer
{
    VkLayerProperties                       vkLayer;

    VkResult                                getAvailableExtensionsResult;
    vector(VkExtensionProperties)           vkAvailableExtensions;

    ~VxGraphicsLayer();
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
    vector(VkExtensionProperties)           vkAvailableExtensions;

    ~VxGraphicsPhysicalDevice();
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
    vector(VkExtensionProperties)           vkAvailableExtensions;

    VkResult                                createInstanceResult;
    VkInstance                              vkInstance;

    spt(VxGraphicsDebug)                    spVxGraphicsDebug;

    VkResult                                getAvailablePhysicalDevicesResult;
    vectorS(VxGraphicsPhysicalDevice)       spVxAvailablePhysicalDevices;

    VkResult                                createMainGraphicsWindowResult;
    spt(VxGraphicsWindow)                   spMainVxGraphicsWindow;

    /*

    VkResult                                vkCreateDeviceResult;
    std::vector<VxGraphicsDevice>           vxDevices;


    VkResult                                vkGetSwapchainImagesKHRResult;
    VxGraphicsSwapchain                     vxMainSwapchain;

    VkResult                                vkCreateRenderPassResult;
    VkRenderPass                            vkRenderPass;

    std::vector<VxGraphicsShader>           vxShaders;

    VxGraphicsPipelineLayout                vxPipelineLayout;
    VxGraphicsPipeline                      vxPipeline;

 
    PFN_vxWindowLoop                        vxWindowLoopFunction;

    VkExtent2D                              windowSize;
    VkResult                                vkAllocateCommandBuffersResult;
    VkCommandBuffer                         vkCommandBuffer;

    VkSemaphore                             acquireSemaphore;
    VkSemaphore                             releaseSemaphore;
    */

    ~VxGraphicsInstance();
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
    }
} VxGraphicsInstance;

VkResult vxCreateGraphicsInstance(spt(VxGraphicsInstanceCreateInfo) spCreateInfo, spt(VxGraphicsInstance) & spVxGraphicsInstance);
VkResult vxGraphicsRun(const spt(VxGraphicsInstance) & spVxGraphicsInstance);
VkResult vxGraphicsDestroyInstance(spt(VxGraphicsInstance) & spVxGraphicsInstance);
VkResult vxGraphicsTerminate(spt(VxGraphicsInstance) & spVxGraphicsInstance);
VkResult vxGetVkSuccess();

#endif

