#include "commonHeaders.hpp"

void VxGraphicsPipelineCreateInfo::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
}

VxGraphicsPipelineCreateInfo::~VxGraphicsPipelineCreateInfo()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsShader::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    GetAndAssertSharedPointer(spVxDevice, wpVxDevice);
    if (vkShaderModule != nullptr)
    {
        vkDestroyShaderModule(spVxDevice->vkDevice, vkShaderModule, nullptr);
        vkShaderModule = nullptr;
    }
}

VxGraphicsShader::~VxGraphicsShader()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsPipeline::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    GetAndAssertSharedPointer(spVxDevice, wpVxDevice);
    if (vkPipeline != nullptr)
    {
        vkDestroyPipeline(spVxDevice->vkDevice, vkPipeline, nullptr);
        vkPipeline = nullptr;
    }
    if (vkPipelineLayout != nullptr)
    {
        vkDestroyPipelineLayout(spVxDevice->vkDevice, vkPipelineLayout, nullptr);
        vkPipelineLayout = nullptr;
    }
}

VxGraphicsPipeline::~VxGraphicsPipeline()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

VkResult vxCreatePipelineLayout(const spt(VxGraphicsDevice) & spVxGraphicsDevice, VkPipelineLayout * vkPipelineLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	AssertVkResult(vkCreatePipelineLayout, spVxGraphicsDevice->vkDevice, &pipelineLayoutCreateInfo, nullptr, vkPipelineLayout);
    return VK_SUCCESS;
}

VkResult vxGetShaderModule(spt(VxGraphicsInstance) spVxGraphicsInstance, std::string shadersFilePath, VkShaderModule * rpVkShaderModule)
{
	for(auto & spVxGraphicsShader : spVxGraphicsInstance->spVxGraphicsShaders)
	{
		if (strcmp(shadersFilePath.c_str(), spVxGraphicsShader->filePath.c_str()) == 0) 
		{
			(*rpVkShaderModule) = spVxGraphicsShader->vkShaderModule;
			return VK_SUCCESS;
		}           
	}
	return VK_ERROR_INVALID_SHADER_NV;
}

VkResult vxCreatePipeline(const spt(VxGraphicsSurface) & spVxGraphicsSurface, const spt(VxGraphicsPipelineCreateInfo) & spCreateInfo, spt(VxGraphicsPipeline) & spVxGraphicsPipeline)
{
	GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsSurface->wpVxGraphicsInstance);
    auto spVxGraphicsDevice = spVxGraphicsSurface->spVxSurfaceDevice;
    spVxGraphicsPipeline = nsp<VxGraphicsPipeline>(spVxGraphicsDevice, spVxGraphicsSurface->spVxGraphicsSwapchain->vkRenderPass);

    StoreAndAssertVkResultP(spVxGraphicsPipeline->createPipelineLayoutResult, vxCreatePipelineLayout, spVxGraphicsDevice, &spVxGraphicsPipeline->vkPipelineLayout);

	VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

	auto stages = std::vector<VkPipelineShaderStageCreateInfo>(2);
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	AssertVkResult(vxGetShaderModule, spVxGraphicsInstance, spCreateInfo->shadersFilePaths[0], &stages[0].module);
	stages[0].pName = "main";
	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	AssertVkResult(vxGetShaderModule, spVxGraphicsInstance, spCreateInfo->shadersFilePaths[1], &stages[1].module);
	stages[1].pName = "main";

	createInfo.stageCount = stages.size();
	createInfo.pStages = stages.data();

	VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	createInfo.pVertexInputState = &vertexInput;

	VkVertexInputBindingDescription stream = { 0, 32, VK_VERTEX_INPUT_RATE_VERTEX };
	VkVertexInputAttributeDescription attrs[3] = {};

	attrs[0].location = 0;
	attrs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attrs[0].offset = 0;
	attrs[1].location = 1;
	attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attrs[1].offset = 12;
	attrs[2].location = 2;
	attrs[2].format = VK_FORMAT_R32G32_SFLOAT;
	attrs[2].offset = 24;

	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &stream;
	vertexInput.vertexAttributeDescriptionCount = 3;
	vertexInput.pVertexAttributeDescriptions = attrs;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	createInfo.pInputAssemblyState = &inputAssembly;

	VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	createInfo.pViewportState = &viewportState;

	VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterizationState.lineWidth = 1.f;
	createInfo.pRasterizationState = &rasterizationState;

	VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.pMultisampleState = &multisampleState;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	createInfo.pDepthStencilState = &depthStencilState;

	VkPipelineColorBlendAttachmentState colorAttachmentState = {};
	colorAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorAttachmentState;
	createInfo.pColorBlendState = &colorBlendState;

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
	dynamicState.pDynamicStates = dynamicStates;
	createInfo.pDynamicState = &dynamicState;

	createInfo.layout = spVxGraphicsPipeline->vkPipelineLayout;
	createInfo.renderPass = spVxGraphicsPipeline->vkRenderPass;

	StoreAndAssertVkResultP(spVxGraphicsPipeline->createPipelineResult, vkCreateGraphicsPipelines, spVxGraphicsDevice->vkDevice, spVxGraphicsPipeline->vkPipelineCache, 1, &createInfo, nullptr, &spVxGraphicsPipeline->vkPipeline);

    return VK_SUCCESS;
}



