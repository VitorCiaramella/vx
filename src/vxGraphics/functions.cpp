#include <vxGraphics/vxGraphics.hpp>

#include <algorithm>

#include "internals.hpp"


VkResult vxSelectPhysicalDevicesForCompute(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    for (auto && vxPhysicalDevice : upGraphicsInstance->vxAvailablePhysicalDevices) 
    {
        bool deviceHasAllDesiredExtensions = true;
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            bool containExtension = false;
            for (auto && vkExtension : vxPhysicalDevice.vkAvailableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
                {
                    containExtension = true;
                    break;
                }           
            }
            if (!containExtension)
            {
                deviceHasAllDesiredExtensions = false;
                break;
            }
        }

        if (true
            //TODO: support required and nice to have extensions
            //&& deviceHasAllDesiredExtensions 
            //TODO: no devices with Vulkan 1.1, support minimum parameter
            //&& availableDevice.deviceProperties.apiVersion >= upGraphicsInstance->rpCreateInfo->apiVersion
            && vxPhysicalDevice.supportsCompute
            )
        {
            //prioritize discrete devices
            //TODO: create a sorting based on device type and memory
            if (vxPhysicalDevice.vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                upGraphicsInstance->vxSelectedPhysicalDevices.insert(upGraphicsInstance->vxSelectedPhysicalDevices.begin(), &vxPhysicalDevice);            
            }
            else
            {
                upGraphicsInstance->vxSelectedPhysicalDevices.push_back(&vxPhysicalDevice);
            }           
        }
    }
    return VK_SUCCESS;
}

VkResult vxCreateDevices(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    printf("\nCreating Device\n");
 
    uint32_t desiredDeviceCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredDeviceCount,(uint32_t)1,(uint32_t)upGraphicsInstance->vxSelectedPhysicalDevices.size());
    for (auto && vxPhysicalDevice : upGraphicsInstance->vxSelectedPhysicalDevices) 
    {
        VxGraphicsDevice vxDevice = {};
        initVxGraphicsDevice(vxDevice);
        vxDevice.vxPhysicalDevice = vxPhysicalDevice;

        //TODO: instead of global queue count, do per desired device type
        uint32_t desiredQueueCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredQueueCountPerDevice,(uint32_t)1,(uint32_t)100);

        VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

        auto rpQueueCreateInfos = nrp(std::vector<VkDeviceQueueCreateInfo>);
        for (auto && queueFamily : vxPhysicalDevice->vxQueueFamilies) 
        {
            if (queueFamily.supportsPresentation
                && queueFamily.supportsGraphics)
            {
                uint32_t queueCount = std::clamp(desiredQueueCount,(uint32_t)1,queueFamily.vkQueueFamily.queueCount);
                desiredQueueCount = std::clamp(desiredQueueCount-queueCount,(uint32_t)0,(uint32_t)100);
                float queuePriority = 1.f;
                auto queuePriorities = nrp(std::vector<float>,queueCount,queuePriority);

                VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                queueCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;
                queueCreateInfo.queueCount = queueCount;
                queueCreateInfo.pQueuePriorities = queuePriorities->data();
                rpQueueCreateInfos->push_back(queueCreateInfo);

                vxDevice.vxQueueFamilies.push_back(queueFamily);
                for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
                {
                    VxGraphicsQueue vxQueue = {};
                    initVxGraphicsQueue(vxQueue);
                    vxQueue.queueFamilyIndex = queueFamily.queueFamilyIndex;
                    vxQueue.queueIndex = queueIndex;
                    vxDevice.vxQueues.push_back(vxQueue);
                }                
            }
            if (desiredQueueCount == 0)
            {
                break;
            }
        }
        deviceCreateInfo.queueCreateInfoCount = rpQueueCreateInfos->size();
        deviceCreateInfo.pQueueCreateInfos = rpQueueCreateInfos->data();
        deviceCreateInfo.pEnabledFeatures = &vxPhysicalDevice->vkPhysicalDeviceFeatures;

        auto extensionsToEnable = nrp(std::vector<char*>);
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            auto extensionSupported = false;
            for (auto && vkExtension : vxPhysicalDevice->vkAvailableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
                {
                    extensionSupported = true;
                    break;
                }           
            }
            if (extensionSupported)
            {
                extensionsToEnable->push_back(strToNewCharArray(desiredExtensionToEnable));
                printf("Desired Extension supported: %s\n", desiredExtensionToEnable.c_str());
            }
            else
            {
                //TODO: Handle this
                printf("Desired Extension not supported: %s\n", desiredExtensionToEnable.c_str());
            }
        }
        deviceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
        deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();
        
        StoreAndAssertVkResultP(upGraphicsInstance, vkCreateDevice, vxDevice.vxPhysicalDevice->vkPhysicalDevice, &deviceCreateInfo, nullptr, &vxDevice.vkDevice);
        for (auto && vxQueue : vxDevice.vxQueues) 
        {
            vkGetDeviceQueue(vxDevice.vkDevice, vxQueue.queueFamilyIndex, vxQueue.queueIndex, &vxQueue.vkQueue);
        }
        for (auto && vxQueueFamily : vxDevice.vxQueueFamilies) 
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = vxQueueFamily.queueFamilyIndex;                    
            StoreAndAssertVkResult(vxQueueFamily, vkCreateCommandPool, vxDevice.vkDevice, &commandPoolCreateInfo, nullptr, &vxQueueFamily.vkCommandPool);
        }

        upGraphicsInstance->vxDevices.push_back(vxDevice);

        if (upGraphicsInstance->vxDevices.size() >= desiredDeviceCount) 
        {
            break;
        }        
    }
    return VK_SUCCESS;
}

VkResult vxCreateSwapchain(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxSwapchain = &upGraphicsInstance->vxMainSwapchain;
    
    //TODO: how to choose best format?
    vxSwapchain->vkFormat = upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceFormats[0];
    vxSwapchain->vxDevice = &(upGraphicsInstance->vxDevices[0]);
    vxSwapchain->vxSurface = &(upGraphicsInstance->vxMainSurface);

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = vxSwapchain->vxSurface->vkSurface;

    uint32_t desiredImageCount = 2;
	createInfo.minImageCount = std::clamp(desiredImageCount, upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.minImageCount, upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.maxImageCount==0 ? desiredImageCount : upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.maxImageCount);;

	createInfo.imageFormat = vxSwapchain->vkFormat.format;
	createInfo.imageColorSpace = vxSwapchain->vkFormat.colorSpace;

    auto windowSize = vxGetWindowSize(upGraphicsInstance);
    if (upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        createInfo.imageExtent.width = std::clamp(windowSize.width, upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.minImageExtent.width, upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.maxImageExtent.width);
        createInfo.imageExtent.height = std::clamp(windowSize.height, upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.minImageExtent.height, upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.maxImageExtent.height);
    }
    else
    {
        createInfo.imageExtent = upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.currentExtent;
    }

	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; //is the number of queue families having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
	createInfo.pQueueFamilyIndices = nullptr; //s an array of queue family indices having access to the images(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT

	createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    auto oldSwapchain = vxSwapchain->vkSwapchain;
    createInfo.oldSwapchain = oldSwapchain;

	createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    //if not supported, use current transformation
    if (!(upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.supportedTransforms & createInfo.preTransform))
    {
        createInfo.preTransform = upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.currentTransform;
    }

    // Find the first supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < 4; i++) {
        if (upGraphicsInstance->vxMainSurface.vxSurfaceDevice->vkSurfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            createInfo.compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }    

	createInfo.clipped = VK_TRUE;
    //TODO: parametrize the device
    StoreAndAssertVkResultP(vxSwapchain, vkCreateSwapchainKHR, vxSwapchain->vxDevice->vkDevice, &createInfo, nullptr, &vxSwapchain->vkSwapchain);

    if (oldSwapchain != VK_NULL_HANDLE)
    {
        //TODO: parametrize the device
        vkDestroySwapchainKHR(vxSwapchain->vxDevice->vkDevice, oldSwapchain, nullptr);
    }

	VkAttachmentDescription attachments[1] = {};
	attachments[0].format = upGraphicsInstance->vxMainSwapchain.vkFormat.format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference colorAttachments = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachments;
	VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	renderPassCreateInfo.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
	renderPassCreateInfo.pAttachments = attachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	StoreAndAssertVkResultP(upGraphicsInstance, vkCreateRenderPass, upGraphicsInstance->vxMainSwapchain.vxDevice->vkDevice, &renderPassCreateInfo, 0, &upGraphicsInstance->vkRenderPass);

    uint32_t imageCount;
    StoreAndAssertVkResultP(vxSwapchain, vkGetSwapchainImagesKHR, vxSwapchain->vxDevice->vkDevice, vxSwapchain->vkSwapchain, &imageCount, nullptr);
    if (imageCount > 0) 
    {
        vxSwapchain->vkImages = std::vector<VkImage>(imageCount);
        StoreAndAssertVkResultP(vxSwapchain, vkGetSwapchainImagesKHR, vxSwapchain->vxDevice->vkDevice, vxSwapchain->vkSwapchain, &imageCount, vxSwapchain->vkImages.data());

        vxSwapchain->vkImageViews = std::vector<VkImageView>(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            createInfo.image = vxSwapchain->vkImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = vxSwapchain->vkFormat.format;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;
            StoreAndAssertVkResultP(vxSwapchain, vkCreateImageView, vxSwapchain->vxDevice->vkDevice, &createInfo, nullptr, &vxSwapchain->vkImageViews[i]);
        }

        vxSwapchain->vkFramebuffers = std::vector<VkFramebuffer>(imageCount);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            createInfo.renderPass = upGraphicsInstance->vkRenderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &vxSwapchain->vkImageViews[i];
            createInfo.width = windowSize.width;
            createInfo.height = windowSize.height;
            createInfo.layers = 1;
            StoreAndAssertVkResultP(vxSwapchain, vkCreateFramebuffer, vxSwapchain->vxDevice->vkDevice, &createInfo, nullptr, &vxSwapchain->vkFramebuffers[i]);
        }
    }
    return VK_SUCCESS;
}

VkResult vxLoadShader(VkDevice device, std::string filePath, VkShaderModule * shaderModule)
{
	FILE* file = fopen(filePath.c_str(), "rb");
	assert(file);

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	assert(length >= 0);
	fseek(file, 0, SEEK_SET);

	char* buffer = new char[length];
	assert(buffer);

	size_t rc = fread(buffer, 1, length, file);
	assert(rc == size_t(length));
	fclose(file);

	VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	createInfo.codeSize = length;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer);

	auto vkResult = vkCreateShaderModule(device, &createInfo, 0, shaderModule);

	delete[] buffer;

	return vkResult;
}

VkResult vxLoadShaders(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxDevice = &(upGraphicsInstance->vxDevices[0]);
    
    upGraphicsInstance->vxShaders = std::vector<VxGraphicsShader>(upGraphicsInstance->rpCreateInfo->shadersFilePaths.size());
    uint32_t index = 0;
    for (auto && shadersFilePath : upGraphicsInstance->rpCreateInfo->shadersFilePaths) 
    {
        auto vxShader = &(upGraphicsInstance->vxShaders[index++]);
        vxShader->vxDevice = vxDevice;
        vxShader->filePath = shadersFilePath;
        StoreAndAssertVkResultP(vxShader, vxLoadShader, vxShader->vxDevice->vkDevice, vxShader->filePath, &vxShader->vkShaderModule);
    }
    return VK_SUCCESS;
}

VkResult vxCreatePipelineLayout(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    upGraphicsInstance->vxPipelineLayout.vxDevice = &(upGraphicsInstance->vxDevices[0]);
    auto vxPipelineLayout = &upGraphicsInstance->vxPipelineLayout;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	StoreAndAssertVkResultP(vxPipelineLayout, vkCreatePipelineLayout, vxPipelineLayout->vxDevice->vkDevice, &pipelineLayoutCreateInfo, nullptr, &vxPipelineLayout->vkPipelineLayout);
    return VK_SUCCESS;
}

VkResult vxCreatePipeline(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    auto vxPipeline = &(upGraphicsInstance->vxPipeline);
    vxPipeline->vxDevice = &(upGraphicsInstance->vxDevices[0]);
    vxPipeline->vxPipelineLayout = &(upGraphicsInstance->vxPipelineLayout);
    vxPipeline->vkRenderPass = upGraphicsInstance->vkRenderPass;
    vxPipeline->vkPipelineCache = nullptr;

	VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

	auto stages = std::vector<VkPipelineShaderStageCreateInfo>(2);
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = upGraphicsInstance->vxShaders[0].vkShaderModule;
	stages[0].pName = "main";
	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = upGraphicsInstance->vxShaders[1].vkShaderModule;
	stages[1].pName = "main";

	createInfo.stageCount = stages.size();
	createInfo.pStages = stages.data();

	VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	createInfo.pVertexInputState = &vertexInput;

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

	createInfo.layout = vxPipeline->vxPipelineLayout->vkPipelineLayout;
	createInfo.renderPass = vxPipeline->vkRenderPass;

	StoreAndAssertVkResultP(vxPipeline, vkCreateGraphicsPipelines, vxPipeline->vxDevice->vkDevice, vxPipeline->vkPipelineCache, 1, &createInfo, nullptr, &vxPipeline->vkPipeline);

    return VK_SUCCESS;
}

VxResult vxGraphicsRun(upt(VxGraphicsInstance) & upGraphicsInstance)
{
    AssertVkVxResult(vxAwaitWindowClose, upGraphicsInstance);

    return VxResult::VX_SUCCESS;
}

VxResult vxGraphicsTerminate(upt(VxGraphicsInstance) & upGraphicsInstance)
{
    glfwTerminate();    

    return VxResult::VX_SUCCESS;
}

VxResult vxGraphicsDestroyInstance(upt(VxGraphicsInstance) & upGraphicsInstance)
{
    if (upGraphicsInstance->vxDevices.size()>0 && upGraphicsInstance->vxDevices[0].vkDevice != nullptr)
    {
        auto device = upGraphicsInstance->vxDevices[0].vkDevice;
        vkDeviceWaitIdle(device);
    }
    if (upGraphicsInstance->acquireSemaphore != nullptr)
    {
        vkDestroySemaphore(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->acquireSemaphore, nullptr);
    }
    if (upGraphicsInstance->releaseSemaphore != nullptr)
    {
    	vkDestroySemaphore(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->releaseSemaphore, nullptr);
    }
    if (upGraphicsInstance->vxPipeline.vkPipeline != nullptr)
    {
        vkDestroyPipeline(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->vxPipeline.vkPipeline, nullptr);
    }
    if (upGraphicsInstance->vxPipeline.vxPipelineLayout != nullptr 
        && upGraphicsInstance->vxPipeline.vxPipelineLayout->vkPipelineLayout != nullptr)
    {
    	vkDestroyPipelineLayout(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->vxPipeline.vxPipelineLayout->vkPipelineLayout, nullptr);
    }

	for (auto && vxShader : upGraphicsInstance->vxShaders)
		vkDestroyShaderModule(upGraphicsInstance->vxDevices[0].vkDevice, vxShader.vkShaderModule, nullptr);
    
	for (auto && vkImageView : upGraphicsInstance->vxMainSwapchain.vkImageViews)
		vkDestroyImageView(upGraphicsInstance->vxDevices[0].vkDevice, vkImageView, nullptr);

	for (auto && vkFramebuffer : upGraphicsInstance->vxMainSwapchain.vkFramebuffers)
		vkDestroyFramebuffer(upGraphicsInstance->vxDevices[0].vkDevice, vkFramebuffer, nullptr);

    if (upGraphicsInstance->vkRenderPass != nullptr)
    {
        vkDestroyRenderPass(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->vkRenderPass, nullptr);
    }
    if (upGraphicsInstance->vxMainSwapchain.vkSwapchain != nullptr)
    {
    	vkDestroySwapchainKHR(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->vxMainSwapchain.vkSwapchain, nullptr);
    }

    if (upGraphicsInstance->vxDevices.size()>0 && upGraphicsInstance->vxDevices[0].vkDevice != nullptr)
    {
        vkDestroyCommandPool(upGraphicsInstance->vxDevices[0].vkDevice, upGraphicsInstance->vxDevices[0].vxQueueFamilies[0].vkCommandPool, nullptr);
        vkDestroyDevice(upGraphicsInstance->vxDevices[0].vkDevice, nullptr);
    }    
    if (upGraphicsInstance->vxMainSurface.vkSurface != nullptr)
    {
        vkDestroySurfaceKHR(upGraphicsInstance->vkInstance, upGraphicsInstance->vxMainSurface.vkSurface, nullptr);
    }
    if (upGraphicsInstance->mainWindow != nullptr)
    {
        glfwDestroyWindow(upGraphicsInstance->mainWindow);
    }
    if (upGraphicsInstance->vkDebugReportCallback != nullptr)
    {
        PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(upGraphicsInstance->vkInstance, "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(upGraphicsInstance->vkInstance, upGraphicsInstance->vkDebugReportCallback, nullptr);
    }
    if (upGraphicsInstance->vkInstance !=  nullptr)
    {
        vkDestroyInstance(upGraphicsInstance->vkInstance, nullptr);
    }
    upGraphicsInstance.release();
    return VxResult::VX_SUCCESS;
}

VkResult vxAllocateCommandBuffer(const upt(VxGraphicsInstance) & upGraphicsInstance, VkCommandBuffer & commandBuffer)
{
    auto device = upGraphicsInstance->vxDevices[0];

    VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocateInfo.commandPool = device.vxQueueFamilies[0].vkCommandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    auto commandBuffers = nup(VkCommandBuffer[],allocateInfo.commandBufferCount);
    AssertVkResult(vkAllocateCommandBuffers, device.vkDevice, &allocateInfo, commandBuffers.get());

    commandBuffer = commandBuffers[0];

    return VK_SUCCESS;
}


