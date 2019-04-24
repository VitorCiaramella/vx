#ifndef VX_GRAPHICS_FUNCTIONS
#define VX_GRAPHICS_FUNCTIONS

#include <vulkan/vulkan.hpp>

VxResult vxCreateGraphicsInstance(rpt(VxGraphicsInstanceCreateInfo) rpCreateInfo, upt(VxGraphicsInstance) & upGraphicsInstance);
VxResult vxGraphicsRun(upt(VxGraphicsInstance) & upGraphicsInstance);
VxResult vxGraphicsDestroyInstance(upt(VxGraphicsInstance) & upGraphicsInstance);
VxResult vxGraphicsTerminate(upt(VxGraphicsInstance) & upGraphicsInstance);

VkResult vxAllocateCommandBuffer(const upt(VxGraphicsInstance) & upGraphicsInstance, VkCommandBuffer & commandBuffer);

#endif
