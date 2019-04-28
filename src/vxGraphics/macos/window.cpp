#if defined(VK_USE_PLATFORM_MACOS_MVK)

#include "../commonHeaders.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


VkResult vxCreateSurface_PlatformSpecific(const spt(VxGraphicsSurface) & spVxGraphicsSurface)
{
    //VkMacOSSurfaceCreateInfoMVK createInfo = {};
    //createInfo.pView = window->view;
    //puGraphicsInstance->vkCreateSurfaceResult = vkCreateMacOSSurfaceMVK(puGraphicsInstance->vkInstance, &createInfo, NULL, &puGraphicsInstance->mainSurface.surface);
    //TODO: remove dependency from glfw
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);
    GetAndAssertSharedPointerVk(spVxGraphicsWindow, spVxGraphicsSurface->wpVxGraphicsWindow);
    StoreAndAssertVkResultP(spVxGraphicsSurface->createSurfaceResult, glfwCreateWindowSurface, spVxGraphicsInstance->vkInstance, spVxGraphicsWindow->rpGlfwWindow, nullptr, &spVxGraphicsSurface->vkSurface);
    return VK_SUCCESS;
}

bool vxQueueFamilySupportsPresentation_PlatformSpecific(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
	return true;
}

#endif
