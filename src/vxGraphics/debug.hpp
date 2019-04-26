#ifndef VX_GRAPHICS_DEBUG_HEADER
#define VX_GRAPHICS_DEBUG_HEADER

#include <vxGraphics/vxGraphics.hpp>

VkResult vxCreateDebugReportCallback(const upt(VxGraphicsInstance) & upGraphicsInstance);
void vxDebugLogVkResult(char* vkFunctionName, VkResult vkResult);

#endif

