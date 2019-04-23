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

    uint32_t                                mainWindowWidth;
    uint32_t                                mainWindowHeight;
} VxGraphicsInstanceCreateInfo;

typedef struct VxGraphicsExtension
{
    VkExtensionProperties                   extension;

} VxGraphicsExtension;
#define initVxGraphicsExtension(object)


typedef struct VxGraphicsLayer
{
    VkLayerProperties                       layer;
    std::vector<VxGraphicsExtension>        availableExtensions;

    VkResult                                vkEnumerateInstanceExtensionPropertiesResult;
} VxGraphicsLayer;
#define initVxGraphicsLayer(object) \
    object.vkEnumerateInstanceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; 

typedef struct VxGraphicsQueueFamily
{
    VkQueueFamilyProperties                 queueFamily;
    uint32_t                                queueFamilyIndex;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                vkCreateCommandPoolResult;
    VkCommandPool                           commandPool;
} VxGraphicsQueueFamily;
#define initVxGraphicsQueueFamily(object) \
    object.vkCreateCommandPoolResult = VK_RESULT_MAX_ENUM;

typedef struct VxGraphicsPhysicalDevice
{
    VkPhysicalDevice                        device;
    VkPhysicalDeviceProperties              deviceProperties;    
    std::vector<VxGraphicsQueueFamily>      queueFamilies;
    VkPhysicalDeviceFeatures                deviceFeatures;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                vkEnumerateDeviceExtensionPropertiesResult;
    std::vector<VxGraphicsExtension>        availableExtensions;

    VkResult                                vkEnumerateDeviceLayerPropertiesResult;
    std::vector<VxGraphicsLayer>            availableLayers;
} VxGraphicsPhysicalDevice;
#define initVxGraphicsPhysicalDevice(object) \
    object.vkEnumerateDeviceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; \
    object.vkEnumerateDeviceLayerPropertiesResult = VK_RESULT_MAX_ENUM;

typedef struct VxGraphicsQueue
{
    VkQueue                                 queue;
    uint32_t                                queueIndex;
    uint32_t                                queueFamilyIndex;
} VxGraphicsQueue;
#define initVxGraphicsQueue(object) 

typedef struct VxGraphicsDevice
{
    VkDevice                                device;
    std::vector<VxGraphicsQueue>            queues;
    std::vector<VxGraphicsQueueFamily>      queueFamilies;
} VxGraphicsDevice;
#define initVxGraphicsDevice(object) 

typedef struct VxGraphicsSurface
{
    VkSurfaceKHR                            surface;

    VkResult                                vkGetPhysicalDeviceSurfaceFormatsKHRResult;
    std::vector<VkSurfaceFormatKHR>         formats;

    VkResult                                vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult;
    VkSurfaceCapabilitiesKHR                capabilities;

    VkResult                                vkGetPhysicalDeviceSurfacePresentModesKHRResult;
    std::vector<VkPresentModeKHR>           presentModes;
} VxGraphicsSurface;
#define initVxGraphicsSurface(object) \
    object->vkGetPhysicalDeviceSurfaceFormatsKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfacePresentModesKHRResult = VK_RESULT_MAX_ENUM; \


typedef struct VxGraphicsInstance 
{
    rpt(VxGraphicsInstanceCreateInfo)       rpCreateInfo;

    VkResult                                vkEnumerateInstanceLayerPropertiesResult;
    std::vector<VxGraphicsLayer>            availableLayers;

    VkResult                                vkEnumerateInstanceExtensionPropertiesResult;
    std::vector<VxGraphicsExtension>        availableExtensions;

    VkResult                                vkEnumeratePhysicalDevicesResult;
    std::vector<VxGraphicsPhysicalDevice>   availableDevices;

    VkResult                                vkCreateInstanceResult;
    VkInstance                              vkInstance;

    vectorR(VxGraphicsPhysicalDevice)       selectedAvailableDevices;

    VkResult                                vkCreateDeviceResult;
    std::vector<VxGraphicsDevice>           devices;

    rpt(GLFWwindow)                         mainWindow;

    VkResult                                vkCreateSurfaceResult;    
    VxGraphicsSurface                       mainSurface;

    VkResult                                vkCreateSwapchainKHRResult;
    VkSwapchainKHR                          mainSwapchain;
} VxGraphicsInstance;

#define initVxGraphicsInstance(object) \
    object->vkEnumerateInstanceLayerPropertiesResult = VK_RESULT_MAX_ENUM; \
    object->vkEnumerateInstanceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; \
    object->vkCreateInstanceResult = VK_RESULT_MAX_ENUM; \
    object->vkEnumeratePhysicalDevicesResult = VK_RESULT_MAX_ENUM; \
    object->vkCreateDeviceResult = VK_RESULT_MAX_ENUM; \
    object->vkCreateSurfaceResult = VK_RESULT_MAX_ENUM; \
    object->vkCreateSwapchainKHRResult = VK_RESULT_MAX_ENUM; \
    object->mainSwapchain =  VK_NULL_HANDLE; \



#endif
