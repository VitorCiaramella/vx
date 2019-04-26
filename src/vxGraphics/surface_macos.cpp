#if defined(VK_USE_PLATFORM_MACOS_MVK)

#include <vxGraphics/vxGraphics.hpp>
#include "internals.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkResult vxCreateSurface_PlatformSpecific(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxSurface = &(upGraphicsInstance->vxMainSurface);
    //VkMacOSSurfaceCreateInfoMVK createInfo = {};
    //createInfo.pView = window->view;
    //puGraphicsInstance->vkCreateSurfaceResult = vkCreateMacOSSurfaceMVK(puGraphicsInstance->vkInstance, &createInfo, NULL, &puGraphicsInstance->mainSurface.surface);
    //TODO: remove dependency from glfw
    StoreAndAssertVkResultP(vxSurface, glfwCreateWindowSurface, upGraphicsInstance->vkInstance, upGraphicsInstance->mainWindow, NULL, &vxSurface->vkSurface);

    return VK_SUCCESS;
}

#endif
