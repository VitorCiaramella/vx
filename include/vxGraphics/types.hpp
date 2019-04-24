#ifndef VX_GRAPHICS_TYPES
#define VX_GRAPHICS_TYPES

#include <vulkan/vulkan.hpp>
//# GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

#include <string>

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

    std::vector<std::string>                shadersFilePaths;

    uint32_t                                mainWindowWidth;
    uint32_t                                mainWindowHeight;
} VxGraphicsInstanceCreateInfo;

typedef struct VxGraphicsLayer
{
    VkLayerProperties                       vkLayer;

    VkResult                                vkEnumerateInstanceExtensionPropertiesResult;
    std::vector<VkExtensionProperties>      vkAvailableExtensions;
} VxGraphicsLayer;
#define initVxGraphicsLayer(object) \
    object.vkEnumerateInstanceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; 

typedef struct VxGraphicsQueueFamily
{
    VkQueueFamilyProperties                 vkQueueFamily;
    uint32_t                                queueFamilyIndex;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                vkCreateCommandPoolResult;
    VkCommandPool                           vkCommandPool;
} VxGraphicsQueueFamily;
#define initVxGraphicsQueueFamily(object) \
    object.vkCreateCommandPoolResult = VK_RESULT_MAX_ENUM;

typedef struct VxGraphicsPhysicalDevice
{
    VkPhysicalDevice                        vkPhysicalDevice;
    VkPhysicalDeviceProperties              vkPhysicalDeviceProperties;    
    std::vector<VxGraphicsQueueFamily>      vxQueueFamilies;
    VkPhysicalDeviceFeatures                vkPhysicalDeviceFeatures;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                vkEnumerateDeviceExtensionPropertiesResult;
    std::vector<VkExtensionProperties>      vkAvailableExtensions;

    VkResult                                vkEnumerateDeviceLayerPropertiesResult;
    std::vector<VxGraphicsLayer>            vxAvailableLayers;

} VxGraphicsPhysicalDevice;
#define initVxGraphicsPhysicalDevice(object) \
    object.vkEnumerateDeviceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; \
    object.vkEnumerateDeviceLayerPropertiesResult = VK_RESULT_MAX_ENUM;

typedef struct VxGraphicsQueue
{
    VkQueue                                 vkQueue;
    uint32_t                                queueIndex;
    uint32_t                                queueFamilyIndex;
} VxGraphicsQueue;
#define initVxGraphicsQueue(object) 

typedef struct VxGraphicsDevice
{
    VkDevice                                vkDevice;
    std::vector<VxGraphicsQueue>            vxQueues;
    std::vector<VxGraphicsQueueFamily>      vxQueueFamilies;
} VxGraphicsDevice;
#define initVxGraphicsDevice(object) 

typedef struct VxGraphicsSurface
{
    VkResult                                glfwCreateWindowSurfaceResult;
    VkSurfaceKHR                            vkSurface;

    VkResult                                vkGetPhysicalDeviceSurfaceFormatsKHRResult;
    std::vector<VkSurfaceFormatKHR>         vkSurfaceFormats;

    VkResult                                vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult;
    VkSurfaceCapabilitiesKHR                vkSurfaceCapabilities;

    VkResult                                vkGetPhysicalDeviceSurfacePresentModesKHRResult;
    std::vector<VkPresentModeKHR>           vkPresentModes;
} VxGraphicsSurface;
#define initVxGraphicsSurface(object) \
    object->vkGetPhysicalDeviceSurfaceFormatsKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfacePresentModesKHRResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsSwapchain
{
    VkResult                                vkCreateSwapchainKHRResult;
    VkSwapchainKHR                          vkSwapchain;

    rpt(VxGraphicsDevice)                   vxDevice;
    VkSurfaceFormatKHR                      vkFormat;

    VkResult                                vkGetSwapchainImagesKHRResult;
    std::vector<VkImage>                    vkImages;

    VkResult                                vkCreateImageViewResult;
    std::vector<VkImageView>                vkImageViews;

    VkResult                                vkCreateFramebufferResult;
    std::vector<VkFramebuffer>              vkFramebuffers;

} VxGraphicsSwapchain;
#define initVxGraphicsSwapchain(object) \
    object->vkCreateSwapchainKHRResult = VK_RESULT_MAX_ENUM; \


typedef struct VxGraphicsShader
{
    std::string                             filePath;        
    rpt(VxGraphicsDevice)                   vxDevice;

    VkResult                                vxLoadShaderResult;
    VkShaderModule                          vkShaderModule;

} VxGraphicsShader;
#define initVxGraphicsShader(object) \
    object->vkCreateShaderModuleResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsPipelineLayout
{
    rpt(VxGraphicsDevice)                   vxDevice;

    VkResult                                vkCreatePipelineLayoutResult;
    VkPipelineLayout                        vkPipelineLayout;

} VxGraphicsPipelineLayout;
#define initVxGraphicsPipelineLayout(object) \
    object->vkCreatePipelineLayoutResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsPipeline
{
    rpt(VxGraphicsDevice)                   vxDevice;
    rpt(VxGraphicsPipelineLayout)           vxPipelineLayout;
    VkRenderPass                            vkRenderPass;
    VkPipelineCache                         vkPipelineCache;

    VkResult                                vkCreateGraphicsPipelinesResult;
    VkPipeline                              vkPipeline;

} VxGraphicsPipeline;
#define initVxGraphicsPipeline(object) \
    object->vkCreateGraphicsPipelinesResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsInstance 
{
    rpt(VxGraphicsInstanceCreateInfo)       rpCreateInfo;

    VkResult                                vkEnumerateInstanceLayerPropertiesResult;
    std::vector<VxGraphicsLayer>            vxAvailableLayers;

    VkResult                                vkEnumerateInstanceExtensionPropertiesResult;
    std::vector<VkExtensionProperties>      vkAvailableExtensions;

    VkResult                                vkEnumeratePhysicalDevicesResult;
    std::vector<VxGraphicsPhysicalDevice>   vxAvailablePhysicalDevices;

    VkResult                                vkCreateInstanceResult;
    VkInstance                              vkInstance;

    vectorR(VxGraphicsPhysicalDevice)       vxSelectedPhysicalDevices;

    VkResult                                vkCreateDeviceResult;
    std::vector<VxGraphicsDevice>           vxDevices;

    rpt(GLFWwindow)                         mainWindow;

    VxGraphicsSurface                       vxMainSurface;

    VxGraphicsSwapchain                     vxMainSwapchain;

    VkResult                                vkCreateRenderPassResult;
    VkRenderPass                            vkRenderPass;

    std::vector<VxGraphicsShader>           vxShaders;

    VxGraphicsPipelineLayout                vxPipelineLayout;
    VxGraphicsPipeline                      vxPipeline;
} VxGraphicsInstance;

#define initVxGraphicsInstance(object) \
    object->vkEnumerateInstanceLayerPropertiesResult = VK_RESULT_MAX_ENUM; \
    object->vkEnumerateInstanceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; \
    object->vkCreateInstanceResult = VK_RESULT_MAX_ENUM; \
    object->vkEnumeratePhysicalDevicesResult = VK_RESULT_MAX_ENUM; \



#endif
