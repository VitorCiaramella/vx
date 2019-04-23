#ifndef VX_GRAPHICS_FUNCTIONS
#define VX_GRAPHICS_FUNCTIONS

#include <vulkan/vulkan.hpp>

VxResult vxCreateGraphicsInstance(rpt(VxGraphicsInstanceCreateInfo) rpCreateInfo, upt(VxGraphicsInstance) & puGraphicsInstance);
VxResult vxAllocateCommandBuffer(const upt(VxGraphicsInstance) & puGraphicsInstance, VkCommandBuffer & commandBuffer);
VxResult vxGraphicsRun(upt(VxGraphicsInstance) & puGraphicsInstance);
VxResult vxGraphicsTerminate(upt(VxGraphicsInstance) & puGraphicsInstance);

#endif
