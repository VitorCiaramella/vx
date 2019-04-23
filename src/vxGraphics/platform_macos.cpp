#if defined(VK_USE_PLATFORM_MACOS_MVK)

#include <vxGraphics/vxGraphics.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkResult vxCreateSurface(const upt(VxGraphicsInstance) & puGraphicsInstance)
{
    //VkMacOSSurfaceCreateInfoMVK createInfo = {};
    //createInfo.pView = window->view;
    //puGraphicsInstance->vkCreateSurfaceResult = vkCreateMacOSSurfaceMVK(puGraphicsInstance->vkInstance, &createInfo, NULL, &puGraphicsInstance->mainSurface.surface);

    puGraphicsInstance->vkCreateSurfaceResult = glfwCreateWindowSurface(puGraphicsInstance->vkInstance, puGraphicsInstance->mainWindow, NULL, &puGraphicsInstance->mainSurface.surface);

    return puGraphicsInstance->vkCreateSurfaceResult;
}

#endif
