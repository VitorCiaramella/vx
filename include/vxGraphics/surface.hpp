#ifndef VX_GRAPHICS_SURFACE_HEADER
#define VX_GRAPHICS_SURFACE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <string>

#include "instance.hpp"
#include "device.hpp"

enum class VxWindowLoopResult;
struct VxGraphicsWindow;
struct VxGraphicsSurfacePhysicalDevice;
struct VxGraphicsSurface;
struct VxGraphicsSwapchain;

typedef VxWindowLoopResult (*PFN_vxWindowLoop)(const upt(VxGraphicsInstance) & upGraphicsInstance);

typedef enum class VxWindowLoopResult
{
    VX_WL_CONTINUE,
    VX_WL_STOP,
} VxWindowLoopResult;

typedef struct VxGraphicsWindow
{
    rpt(GLFWwindow)                         rpGlfwWindow;
} VxGraphicsWindow;
#define initVxGraphicsWindow(object) \


typedef struct VxGraphicsSurfacePhysicalDevice
{
    wpt(VxGraphicsSurface)                  wpVxSurface;
    wpt(VxGraphicsPhysicalDevice)           wpVxPhysicalDevice;
    std::vector<wpt(VxGraphicsQueueFamily)> wpVxSupportedQueueFamilies;

    VkResult                                vkGetPhysicalDeviceSurfaceFormatsKHRResult;
    std::vector<VkSurfaceFormatKHR>         vkSurfaceFormats;

    VkResult                                vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult;
    VkSurfaceCapabilitiesKHR                vkSurfaceCapabilities;

    VkResult                                vkGetPhysicalDeviceSurfacePresentModesKHRResult;
    std::vector<VkPresentModeKHR>           vkPresentModes;

} VxGraphicsSurfacePhysicalDevice;
#define initVxGraphicsSurfacePhysicalDevice(object) \
    object->vkEnumerateDeviceExtensionPropertiesResult = VK_RESULT_MAX_ENUM; \
    object->vkEnumerateDeviceLayerPropertiesResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfaceFormatsKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfacePresentModesKHRResult = VK_RESULT_MAX_ENUM; \


typedef struct VxGraphicsSurface
{
    wpt(VxGraphicsInstance)                         wpVxGraphicsInstance;
    wpt(VxGraphicsWindow)                           wpVxGraphicsWindow;

    VkResult                                        glfwCreateWindowSurfaceResult;
    VkSurfaceKHR                                    vkSurface;

    std::vector<spt(VxGraphicsSurfacePhysicalDevice)> spVxSupportedSurfaceDevices;
    wpt(VxGraphicsSurfacePhysicalDevice)            wpVxSurfaceDevice;
} VxGraphicsSurface;
#define initVxGraphicsSurface(object) \
    object->vkGetPhysicalDeviceSurfaceFormatsKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult = VK_RESULT_MAX_ENUM; \
    object->vkGetPhysicalDeviceSurfacePresentModesKHRResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsSwapchain
{
    VkResult                                vkCreateSwapchainKHRResult;
    VkSwapchainKHR                          vkSwapchain;

    wpt(VxGraphicsSurface)                  wpVxSurface;
    wpt(VxGraphicsDevice)                   wpVxDevice;
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


VkResult vxCreateSurface(const spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsSurface) & spVxGraphicsSurface);
VkResult vxCreateSurface_PlatformSpecific(const spt(VxGraphicsSurface) & spVxGraphicsSurface);


#endif