#if defined(VK_USE_PLATFORM_MACOS_MVK)

#include <vxGraphics/vxGraphics.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkResult vxCreateSurface(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    auto vxSurface = &(puGraphicsInstance->vxMainSurface);
    //VkMacOSSurfaceCreateInfoMVK createInfo = {};
    //createInfo.pView = window->view;
    //puGraphicsInstance->vkCreateSurfaceResult = vkCreateMacOSSurfaceMVK(puGraphicsInstance->vkInstance, &createInfo, NULL, &puGraphicsInstance->mainSurface.surface);
    //TODO: remove dependency from glfw
    StoreAndAssertVkResultP(vxSurface, glfwCreateWindowSurface, puGraphicsInstance->vkInstance, puGraphicsInstance->mainWindow, NULL, &vxSurface->vkSurface);

    return VK_SUCCESS;
}

#endif
