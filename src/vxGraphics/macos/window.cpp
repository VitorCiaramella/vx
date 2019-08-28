#if defined(VK_USE_PLATFORM_MACOS_MVK)

#include "../commonHeaders.hpp"

VkResult vxCreateSurface_PlatformSpecific(const spt(VxGraphicsSurface) & spVxGraphicsSurface)
{
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);
    GetAndAssertSharedPointerVk(spVxGraphicsWindow, spVxGraphicsSurface->wpVxGraphicsWindow);
    AssertNotNullVkResult(spVxGraphicsWindow->rpWindowHandle);

    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = { VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK };
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.pView = spVxGraphicsWindow->rpWindowHandle;
    StoreAndAssertVkResultP(spVxGraphicsSurface->createSurfaceResult, vkCreateMacOSSurfaceMVK, spVxGraphicsInstance->vkInstance, &surfaceCreateInfo, nullptr, &spVxGraphicsSurface->vkSurface);

    return VK_SUCCESS;
}

bool vxQueueFamilySupportsPresentation_PlatformSpecific(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
	return true;
}

#endif
