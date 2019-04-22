#ifndef VX_GRAPHICS_FUNCTIONS
#define VX_GRAPHICS_FUNCTIONS

#include <vulkan/vulkan.hpp>

VxResult vxCreateGraphicsInstance(rpt(VxGraphicsInstanceCreateInfo) rpCreateInfo, upt(VxGraphicsInstance) & puGraphicsInstance);
VxResult vxAllocateCommandBuffer(const upt(VxGraphicsInstance) & puGraphicsInstance, VkCommandBuffer & commandBuffer);

#endif
