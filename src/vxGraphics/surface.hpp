#ifndef VX_GRAPHICS_SURFACE_HEADER
#define VX_GRAPHICS_SURFACE_HEADER

#include <vxGraphics/vxGraphics.hpp>

VkResult vxCreateSurface(const upt(VxGraphicsInstance) & puGraphicsInstance);
VkResult vxCreateSurface_PlatformSpecific(const upt(VxGraphicsInstance) & puGraphicsInstance);

#endif