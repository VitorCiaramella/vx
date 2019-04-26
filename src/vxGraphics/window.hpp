#ifndef VX_GRAPHICS_WINDOW
#define VX_GRAPHICS_WINDOW

#include <vxGraphics/vxGraphics.hpp>

VxResult vxCreateWindow(const upt(VxGraphicsInstance) & upGraphicsInstance);
VkExtent2D vxGetWindowSize(const upt(VxGraphicsInstance) & upGraphicsInstance);
VkResult vxAwaitWindowClose(const upt(VxGraphicsInstance) & upGraphicsInstance);

#endif