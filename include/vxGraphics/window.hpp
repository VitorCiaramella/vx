#ifndef VX_GRAPHICS_WINDOW_HEADER
#define VX_GRAPHICS_WINDOW_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vulkan/vulkan.hpp>
#include <string>

#include "instance.hpp"
#include "device.hpp"

enum class VxWindowLoopResult;
struct VxGraphicsWindow;
struct VxGraphicsSurfacePhysicalDevice;
struct VxGraphicsSurface;
struct VxGraphicsSwapchain;
struct VxGraphicsInstance;
struct VxGraphicsQueueFamily;

typedef VxWindowLoopResult (*PFN_vxWindowLoop)(const spt(VxGraphicsWindow) & spVxGraphicsWindow);
typedef bool (*PFN_vxGetWindowSize)(void* rpWindowHandle, uint32_t & width, uint32_t & height);

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
    PFN_vxWindowLoop                        rpVxWindowLoopFunction;
    void*                                   rpExistingWindowHandle;

    ~VxGraphicsWindowCreateInfo();
    void destroy();
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
        rpVxWindowLoopFunction = nullptr;
        rpExistingWindowHandle = nullptr;
    }
} VxGraphicsWindowCreateInfo;

typedef enum class VxWindowLoopResult
{
    VX_WL_CONTINUE,
    VX_WL_STOP,
} VxWindowLoopResult;

typedef struct VxGraphicsWindow
{
    void*                                   rpWindowHandle;
    PFN_vxWindowLoop                        rpVxWindowLoopFunction;
    PFN_vxGetWindowSize                     rpVxGetWindowSize;
    wpt(VxGraphicsInstance)                 wpVxGraphicsInstance;

    VkResult                                createGraphicsSurfaceResult;
    spt(VxGraphicsSurface)                  spVxGraphicsSurface;

    ~VxGraphicsWindow();
    void destroy();
    void init(spt(VxGraphicsInstance) spVxGraphicsInstance, PFN_vxWindowLoop windowLoopFunction)
    {
        rpVxWindowLoopFunction = windowLoopFunction;
        rpWindowHandle = nullptr;
        rpVxGetWindowSize = nullptr;
        wpVxGraphicsInstance = spVxGraphicsInstance;
        createGraphicsSurfaceResult = VK_RESULT_MAX_ENUM;
        spVxGraphicsSurface = nullptr;
    }
} VxGraphicsWindow;


typedef struct VxGraphicsSurfacePhysicalDevice
{
    wpt(VxGraphicsSurface)                  wpVxSurface;
    wpt(VxGraphicsPhysicalDevice)           wpVxPhysicalDevice;
    vectorW(VxGraphicsQueueFamily)          wpVxSupportedQueueFamilies;

    VkResult                                vkGetPhysicalDeviceSurfaceFormatsKHRResult;
    vectorT(VkSurfaceFormatKHR)             vkSurfaceFormats;

    VkResult                                vkGetPhysicalDeviceSurfacePresentModesKHRResult;
    vectorT(VkPresentModeKHR)               vkPresentModes;

    VkResult getSurfaceCapabilities(VkSurfaceCapabilitiesKHR * rpVkSurfaceCapabilities);

    ~VxGraphicsSurfacePhysicalDevice();
    void destroy();
    void init()
    {
        wpVxSupportedQueueFamilies.reserve(32);
        vkGetPhysicalDeviceSurfaceFormatsKHRResult = VK_RESULT_MAX_ENUM;
        vkSurfaceFormats.reserve(32);
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

    vectorS(VxGraphicsSurfacePhysicalDevice)        spVxSupportedPhysicalDevices;
    wpt(VxGraphicsSurfacePhysicalDevice)            wpVxSurfacePhysicalDevice;

    spt(VxGraphicsDevice)                           spVxSurfaceDevice;

    VkResult                                        createSwapchainResult;
    spt(VxGraphicsSwapchain)                        spVxGraphicsSwapchain;

    ~VxGraphicsSurface();
    void destroy();
    void init(spt(VxGraphicsInstance) spVxGraphicsInstance, spt(VxGraphicsWindow) spVxGraphicsWindow)
    {
        wpVxGraphicsInstance = spVxGraphicsInstance;
        wpVxGraphicsWindow = spVxGraphicsWindow;
        createSurfaceResult = VK_RESULT_MAX_ENUM;
        vkSurface = nullptr;
        spVxSupportedPhysicalDevices.reserve(32);
        spVxSurfaceDevice = nullptr;
        createSwapchainResult = VK_RESULT_MAX_ENUM;
        spVxGraphicsSwapchain = nullptr;
    }
} VxGraphicsSurface;

typedef struct VxGraphicsSwapchain
{
    const int MAX_FRAMES_IN_FLIGHT = 2;
    size_t currentFrame = 0;

    wpt(VxGraphicsSurface)                  wpVxSurface;
    wpt(VxGraphicsDevice)                   wpVxDevice;
    VkSurfaceFormatKHR                      vkFormat;

    VkExtent2D                              swapchainSize;

    VkResult                                createSwapchainResult;
    VkSwapchainKHR                          vkSwapchain;

    VkResult                                createRenderPassResult;
    VkRenderPass                            vkRenderPass;

    VkResult                                getImagesResult;
    vectorT(VkImage)                        vkImages;

    VkResult                                createImageViewsResult;
    vectorT(VkImageView)                    vkImageViews;

    VkResult                                createFramebuffersResult;
    vectorT(VkFramebuffer)                  vkFramebuffers;

    vectorS(VxSemaphore)                    imageAvailableSemaphores;
    vectorS(VxSemaphore)                    renderFinishedSemaphores;

    VkSemaphore getImageAvailableSemaphore();
    VkSemaphore getRenderFinishedSemaphore();
    void advanceFrame();
    
    ~VxGraphicsSwapchain();
    void destroy();
    void init(spt(VxGraphicsSurface) spVxSurface, spt(VxGraphicsDevice) spVxDevice)
    {
        wpVxSurface = spVxSurface;
        wpVxDevice = spVxDevice;
        vkFormat = {};
        createSwapchainResult = VK_RESULT_MAX_ENUM;
        vkSwapchain = nullptr;
        createRenderPassResult = VK_RESULT_MAX_ENUM;
        vkRenderPass = nullptr;
        getImagesResult = VK_RESULT_MAX_ENUM;
        vkImages.reserve(32);
        createImageViewsResult = VK_RESULT_MAX_ENUM;
        vkImageViews.reserve(32);
        createFramebuffersResult = VK_RESULT_MAX_ENUM;
        vkFramebuffers.reserve(32);
        imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    }
} VxGraphicsSwapchain;

VkResult vxCreateGraphicsWindow(spt(VxGraphicsWindowCreateInfo) spVxGraphicsWindowCreateInfo, spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsWindow) & spVxGraphicsWindow);
VkExtent2D vxGetWindowSize(const spt(VxGraphicsWindow) & spVxGraphicsWindow);

VkResult vxCreateGraphicsSurface(const spt(VxGraphicsInstance) & spVxGraphicsInstance, spt(VxGraphicsSurface) & spVxGraphicsSurface);
VkResult vxCreateSurfaceDevice(spt(VxGraphicsSurface) spVxGraphicsSurface, spt(VxGraphicsDevice) & spVxGraphicsDevice);
VkResult vxCreateSwapchain(spt(VxGraphicsSurface) spVxGraphicsSurface, spt(VxGraphicsSwapchain) & spVxGraphicsSwapchain);

VkResult vxCreateSurface_PlatformSpecific(const spt(VxGraphicsSurface) & spVxGraphicsSurface);
bool vxQueueFamilySupportsPresentation_PlatformSpecific(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);

#endif