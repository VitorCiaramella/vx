#ifndef VX_GRAPHICS_WINDOW_HEADER
#define VX_GRAPHICS_WINDOW_HEADER

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

typedef struct VxGraphicsWindowCreateInfo
{
    uint32_t                                initialWidth;
    uint32_t                                initialHeight;
    bool                                    resizable;
    bool                                    visible;
    bool                                    decorated;
    bool                                    focused;
    bool                                    focusOnShow;
    bool                                    floating;
    bool                                    centerCursor;
    bool                                    transparentFramebuffer;
    bool                                    scaleToMonitor;
    bool                                    fullScreen;
    const char*                             title;

    ~VxGraphicsWindowCreateInfo();
    void init()
    {
        initialWidth = 1024;
        initialHeight = 768;        
        resizable = false;
        visible = true;
        decorated = true;
        focused = true;
        focusOnShow = true;
        floating = false;
        centerCursor = true;
        transparentFramebuffer = false;
        scaleToMonitor = false;
        fullScreen = false;
        title = "VxGraphicsWindow";
    }
} VxGraphicsWindowCreateInfo;

typedef enum class VxWindowLoopResult
{
    VX_WL_CONTINUE,
    VX_WL_STOP,
} VxWindowLoopResult;

typedef struct VxGraphicsWindow
{
    rpt(GLFWwindow)                         rpGlfwWindow;

    VkResult                                createGraphicsSurfaceResult;
    spt(VxGraphicsSurface)                  spVxGraphicsSurface;

    ~VxGraphicsWindow();
    void init()
    {
        rpGlfwWindow = nullptr;
        createGraphicsSurfaceResult = VK_RESULT_MAX_ENUM;
    }
} VxGraphicsWindow;


typedef struct VxGraphicsSurfacePhysicalDevice
{
    wpt(VxGraphicsSurface)                  wpVxSurface;
    wpt(VxGraphicsPhysicalDevice)           wpVxPhysicalDevice;
    vectorW(VxGraphicsQueueFamily)          wpVxSupportedQueueFamilies;

    VkResult                                vkGetPhysicalDeviceSurfaceFormatsKHRResult;
    vector(VkSurfaceFormatKHR)              vkSurfaceFormats;

    VkResult                                vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult;
    VkSurfaceCapabilitiesKHR                vkSurfaceCapabilities;

    VkResult                                vkGetPhysicalDeviceSurfacePresentModesKHRResult;
    vector(VkPresentModeKHR)                vkPresentModes;

    ~VxGraphicsSurfacePhysicalDevice();
    void init()
    {
        wpVxSupportedQueueFamilies.reserve(32);
        vkGetPhysicalDeviceSurfaceFormatsKHRResult = VK_RESULT_MAX_ENUM;
        vkSurfaceFormats.reserve(32);
        vkGetPhysicalDeviceSurfaceCapabilitiesKHRResult = VK_RESULT_MAX_ENUM;
        vkGetPhysicalDeviceSurfacePresentModesKHRResult = VK_RESULT_MAX_ENUM;
        vkPresentModes.reserve(32);
    }
} VxGraphicsSurfacePhysicalDevice;


typedef struct VxGraphicsSurface
{
    wpt(VxGraphicsInstance)                         wpVxGraphicsInstance;
    wpt(VxGraphicsWindow)                           wpVxGraphicsWindow;

    VkResult                                        createSurfaceResult;
    VkSurfaceKHR                                    vkSurface;

    vectorS(VxGraphicsSurfacePhysicalDevice)        spVxSupportedSurfaceDevices;
    wpt(VxGraphicsSurfacePhysicalDevice)            wpVxSurfaceDevice;

    ~VxGraphicsSurface();
    void init()
    {
        createSurfaceResult = VK_RESULT_MAX_ENUM;
        vkSurface = nullptr;
        spVxSupportedSurfaceDevices.reserve(32);
    }
} VxGraphicsSurface;

typedef struct VxGraphicsSwapchain
{
    VkResult                                vkCreateSwapchainKHRResult;
    VkSwapchainKHR                          vkSwapchain;

    wpt(VxGraphicsSurface)                  wpVxSurface;
    wpt(VxGraphicsDevice)                   wpVxDevice;
    VkSurfaceFormatKHR                      vkFormat;

    VkResult                                vkGetSwapchainImagesKHRResult;
    vector(VkImage)                         vkImages;

    VkResult                                vkCreateImageViewResult;
    vector(VkImageView)                     vkImageViews;

    VkResult                                vkCreateFramebufferResult;
    vector(VkFramebuffer)                   vkFramebuffers;

    ~VxGraphicsSwapchain();
    void init()
    {
        vkCreateSwapchainKHRResult = VK_RESULT_MAX_ENUM;
    }
} VxGraphicsSwapchain;

VkResult vxCreateGraphicsWindow(spt(VxGraphicsWindowCreateInfo) spVxGraphicsWindowCreateInfo, spt(VxGraphicsWindow) & spVxGraphicsWindow);
VkResult vxCreateGraphicsSurface(const spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsSurface) & spVxGraphicsSurface);

VkResult vxCreateSurface_PlatformSpecific(const spt(VxGraphicsSurface) & spVxGraphicsSurface);
bool vxQueueFamilySupportsPresentation_PlatformSpecific(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);

/*
VkExtent2D vxGetWindowSize(const upt(VxGraphicsInstance) & upGraphicsInstance);
VkResult vxAwaitWindowClose(const upt(VxGraphicsInstance) & upGraphicsInstance);
*/
#endif