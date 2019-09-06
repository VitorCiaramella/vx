#if defined(VK_USE_PLATFORM_MACOS_MVK)

#include "../commonHeaders.hpp"

VkResult vxCreateSurface_PlatformSpecific(const spt(VxGraphicsSurface) & spVxGraphicsSurface)
{
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);
    GetAndAssertSharedPointerVk(spVxGraphicsWindow, spVxGraphicsSurface->wpVxGraphicsWindow);
    AssertNotNullVkResult(spVxGraphicsWindow->rpWindowHandle);

    VkMetalSurfaceCreateInfoEXT surfaceCreateInfo = { VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT };
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.pLayer = spVxGraphicsWindow->rpWindowHandle;

    PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCreateMetalSurfaceEXT");
    AssertNotNullVkResult(vkCreateMetalSurfaceEXT);
    StoreAndAssertVkResultP(spVxGraphicsSurface->createSurfaceResult, vkCreateMetalSurfaceEXT, spVxGraphicsInstance->vkInstance, &surfaceCreateInfo, nullptr, &spVxGraphicsSurface->vkSurface);

    return VK_SUCCESS;
}

bool vxQueueFamilySupportsPresentation_PlatformSpecific(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
	return true;
}

#endif
