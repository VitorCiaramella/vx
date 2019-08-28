#ifndef VX_GRAPHICS_PIPELINE_HEADER
#define VX_GRAPHICS_PIPELINE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vulkan/vulkan.hpp>
#include <string>

#include "device.hpp"
#include "window.hpp"

struct VxGraphicsSurface;

typedef struct VxGraphicsPipelineCreateInfo
{
    vectorT(std::string)                     shadersFilePaths;

    ~VxGraphicsPipelineCreateInfo();
    void destroy();
    void init()
    {
        shadersFilePaths.reserve(32);
    }
} VxGraphicsPipelineCreateInfo;

typedef struct VxGraphicsShader
{
    std::string                             filePath;        
    wpt(VxGraphicsDevice)                   wpVxDevice;

    VkResult                                vxLoadShaderResult;
    VkShaderModule                          vkShaderModule;

    ~VxGraphicsShader();
    void destroy();
    void init(spt(VxGraphicsDevice) spVxGraphicsDevice)
    {
        wpVxDevice = spVxGraphicsDevice;
        filePath = "";        
        vxLoadShaderResult = VK_RESULT_MAX_ENUM;
        vkShaderModule = nullptr;
    }
} VxGraphicsShader;

typedef struct VxGraphicsPipeline
{
    wpt(VxGraphicsDevice)                   wpVxDevice;
    VkRenderPass                            vkRenderPass;
    VkPipelineCache                         vkPipelineCache;

    VkResult                                createPipelineLayoutResult;
    VkPipelineLayout                        vkPipelineLayout;

    VkResult                                createPipelineResult;
    VkPipeline                              vkPipeline;

    VkResult                                loadShaderdResult;
    vectorS(VxGraphicsShader)               spVxGraphicsShaders;

    ~VxGraphicsPipeline();
    void destroy();
    void init(spt(VxGraphicsDevice) spVxGraphicsDevice, VkRenderPass renderPass)
    {
        wpVxDevice = spVxGraphicsDevice;
        vkRenderPass = renderPass;
        vkPipelineCache = nullptr;
        createPipelineLayoutResult = VK_RESULT_MAX_ENUM;
        vkPipelineLayout = nullptr;
        createPipelineResult = VK_RESULT_MAX_ENUM;
        vkPipeline = nullptr;
        loadShaderdResult = VK_RESULT_MAX_ENUM;
        spVxGraphicsShaders.reserve(32);
    }
} VxGraphicsPipeline;

VkResult vxCreatePipeline(const spt(VxGraphicsSurface) & spVxGraphicsSurface, const spt(VxGraphicsPipelineCreateInfo) & spCreateInfo, spt(VxGraphicsPipeline) & spVxGraphicsPipeline);

#endif