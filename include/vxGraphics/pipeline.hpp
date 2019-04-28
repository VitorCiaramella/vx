#ifndef VX_GRAPHICS_PIPELINE_HEADER
#define VX_GRAPHICS_PIPELINE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vulkan/vulkan.hpp>
#include <string>

#include "device.hpp"

typedef struct VxGraphicsShader
{
    std::string                             filePath;        
    wpt(VxGraphicsDevice)                   wpVxDevice;

    VkResult                                vxLoadShaderResult;
    VkShaderModule                          vkShaderModule;

} VxGraphicsShader;
#define initVxGraphicsShader(object) \
    object->vkCreateShaderModuleResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsPipelineLayout
{
    wpt(VxGraphicsDevice)                   wpVxDevice;

    VkResult                                vkCreatePipelineLayoutResult;
    VkPipelineLayout                        vkPipelineLayout;

} VxGraphicsPipelineLayout;
#define initVxGraphicsPipelineLayout(object) \
    object->vkCreatePipelineLayoutResult = VK_RESULT_MAX_ENUM; \

typedef struct VxGraphicsPipeline
{
    wpt(VxGraphicsDevice)                   wpVxDevice;
    wpt(VxGraphicsPipelineLayout)           wpVxPipelineLayout;
    VkRenderPass                            vkRenderPass;
    VkPipelineCache                         vkPipelineCache;

    VkResult                                vkCreateGraphicsPipelinesResult;
    VkPipeline                              vkPipeline;

} VxGraphicsPipeline;
#define initVxGraphicsPipeline(object) \
    object->vkCreateGraphicsPipelinesResult = VK_RESULT_MAX_ENUM; \


#endif