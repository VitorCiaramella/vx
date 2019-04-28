#include "commonHeaders.hpp"

VxGraphicsDebug::~VxGraphicsDebug()
{
    vxLogInfo2("Destructor call", "Memory");
    if (vkDebugReportCallback != nullptr)
    {
        GetAndAssertSharedPointer(spVxGraphicsInstance, wpVxGraphicsInstance);
        AssertTrue(spVxGraphicsInstance->vkInstance!=nullptr);
        vxLogInfo2("Destroying vkDebugReportCallback...", "Memory");
        PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(spVxGraphicsInstance->vkInstance, vkDebugReportCallback, nullptr);
        vkDebugReportCallback = nullptr;
        vxLogInfo2("vkDebugReportCallback destroyed.", "Memory");
    }
}

const char * VkResultToString(VkResult vkResult)
{
    switch(vkResult)
    {
        CASE_ID_RETURN_NAME(VK_SUCCESS);
        CASE_ID_RETURN_NAME(VK_NOT_READY);    
        CASE_ID_RETURN_NAME(VK_TIMEOUT);    
        CASE_ID_RETURN_NAME(VK_EVENT_SET);    
        CASE_ID_RETURN_NAME(VK_EVENT_RESET);    
        CASE_ID_RETURN_NAME(VK_INCOMPLETE);    
        CASE_ID_RETURN_NAME(VK_ERROR_OUT_OF_HOST_MEMORY);    
        CASE_ID_RETURN_NAME(VK_ERROR_OUT_OF_DEVICE_MEMORY);    
        CASE_ID_RETURN_NAME(VK_ERROR_INITIALIZATION_FAILED);    
        CASE_ID_RETURN_NAME(VK_ERROR_DEVICE_LOST);    
        CASE_ID_RETURN_NAME(VK_ERROR_MEMORY_MAP_FAILED);    
        CASE_ID_RETURN_NAME(VK_ERROR_LAYER_NOT_PRESENT);    
        CASE_ID_RETURN_NAME(VK_ERROR_EXTENSION_NOT_PRESENT);    
        CASE_ID_RETURN_NAME(VK_ERROR_FEATURE_NOT_PRESENT);    
        CASE_ID_RETURN_NAME(VK_ERROR_INCOMPATIBLE_DRIVER);    
        CASE_ID_RETURN_NAME(VK_ERROR_TOO_MANY_OBJECTS);    
        CASE_ID_RETURN_NAME(VK_ERROR_FORMAT_NOT_SUPPORTED);    
        CASE_ID_RETURN_NAME(VK_ERROR_FRAGMENTED_POOL);    
        CASE_ID_RETURN_NAME(VK_ERROR_OUT_OF_POOL_MEMORY);    
        CASE_ID_RETURN_NAME(VK_ERROR_INVALID_EXTERNAL_HANDLE);    
        CASE_ID_RETURN_NAME(VK_ERROR_SURFACE_LOST_KHR);    
        CASE_ID_RETURN_NAME(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);    
        CASE_ID_RETURN_NAME(VK_SUBOPTIMAL_KHR);    
        CASE_ID_RETURN_NAME(VK_ERROR_OUT_OF_DATE_KHR);    
        CASE_ID_RETURN_NAME(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);    
        CASE_ID_RETURN_NAME(VK_ERROR_VALIDATION_FAILED_EXT);    
        CASE_ID_RETURN_NAME(VK_ERROR_INVALID_SHADER_NV);    
        CASE_ID_RETURN_NAME(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);    
        CASE_ID_RETURN_NAME(VK_ERROR_FRAGMENTATION_EXT);    
        CASE_ID_RETURN_NAME(VK_ERROR_NOT_PERMITTED_EXT);    
        CASE_ID_RETURN_NAME(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);    
        CASE_ID_RETURN_NAME(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);    
        CASE_ID_RETURN_NAME(VK_RESULT_RANGE_SIZE);  
        CASE_ID_RETURN_NAME(VK_RESULT_MAX_ENUM);   
        default: return NULL;
    }
}

std::string VkDebugReportFlagsToString(VkDebugReportFlagsEXT flags)
{
    std::string result = "";
    auto buildDebugReportTypeString = [](VkDebugReportFlagsEXT flags, VkDebugReportFlagBitsEXT flag, std::string flagText, std::string & flagsText)
    {
        if (flags & flag)
        {
            if (flagText.size()>0) flagsText += ",";
            flagsText += flagText;
        }
    };
    buildDebugReportTypeString(flags, VK_DEBUG_REPORT_ERROR_BIT_EXT, "Error", result);
    buildDebugReportTypeString(flags, VK_DEBUG_REPORT_WARNING_BIT_EXT, "Warning", result);
    buildDebugReportTypeString(flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT, "PerfWarning", result);
    buildDebugReportTypeString(flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, "Information", result);
    return result;
}

VkBool32 debugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    auto debugReportType = VkDebugReportFlagsToString(flags);
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        vxLogError3("%s: %s", "Vulkan", debugReportType.c_str(), pMessage);

        if (!(strstr(pMessage, "Device Extension VK_KHR_surface is not supported by this layer."))
            &!(strstr(pMessage, "Device Extension VK_MVK_macos_surface is not supported by this layer."))
            )
        {
            auto a = 0;//breakpoint
        }
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        vxLogWarning3("%s: %s", "Vulkan", debugReportType.c_str(), pMessage);
    }
    else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        vxLogInfo3("%s: %s", "Vulkan", debugReportType.c_str(), pMessage);
    }
	return VK_FALSE;
}

VkResult _vxCreateDebugReportCallback(const spt(VxGraphicsDebug) & spVxGraphicsDebug)
{
    vxLogInfo2("Creating debug report callback", "Vulkan");
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsDebug->wpVxGraphicsInstance);
    AssertTrueVkResult(spVxGraphicsInstance->vkInstance!=nullptr);
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCreateDebugReportCallbackEXT");
    AssertTrueVkResult(vkCreateDebugReportCallbackEXT!=nullptr);
    VkDebugReportCallbackCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT };
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfo.pfnCallback = &debugReportCallback;
	StoreAndAssertVkResultP(spVxGraphicsDebug->vkCreateDebugReportCallbackResult, vkCreateDebugReportCallbackEXT, spVxGraphicsInstance->vkInstance, &createInfo, nullptr, &spVxGraphicsDebug->vkDebugReportCallback);
    vxLogInfo2("Debug report callback created.", "Vulkan");
    return VK_SUCCESS;
}

void vxGraphicsDebugLogVkResult(const VkResult vkResult, const char* vulkanFunctionName, const char* functionName, const char* fileName, const int line)
{
    auto vkResultAsText = VkResultToString(vkResult);
    if (vulkanFunctionName == nullptr)
    {
        vxLogError2(vkResultAsText, "Vulkan");
    }
    else
    {
        char message[1024];
        snprintf(message, arraySize(message), "%s:%s", vkResultAsText, vulkanFunctionName);
        vxLogError2(message, "Vulkan");
    }    
}

