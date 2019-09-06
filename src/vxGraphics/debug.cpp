#include "commonHeaders.hpp"

#ifdef _DEBUG && VK_USE_PLATFORM_MACOS_MVK
#include <MoltenVK/vk_mvk_moltenvk.h>
#endif

void VxGraphicsDebug::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    if (vkDebugReportCallback != nullptr)
    {
        GetAndAssertSharedPointer(spVxGraphicsInstance, wpVxGraphicsInstance);
        AssertNotNull(spVxGraphicsInstance->vkInstance);
        vxLogInfo2("Destroying vkDebugReportCallback...", "Memory");
        PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkDestroyDebugReportCallbackEXT");
        vkDestroyDebugReportCallbackEXT(spVxGraphicsInstance->vkInstance, vkDebugReportCallback, nullptr);
        vkDebugReportCallback = nullptr;
        vxLogInfo2("vkDebugReportCallback destroyed.", "Memory");
    }
}

VxGraphicsDebug::~VxGraphicsDebug()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
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

static inline const char* string_VkObjectType(VkObjectType input_value)
{
    switch ((VkObjectType)input_value)
    {
        case VK_OBJECT_TYPE_QUERY_POOL:
            return "VK_OBJECT_TYPE_QUERY_POOL";
        case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:
            return "VK_OBJECT_TYPE_OBJECT_TABLE_NVX";
        case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
            return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
        case VK_OBJECT_TYPE_SEMAPHORE:
            return "VK_OBJECT_TYPE_SEMAPHORE";
        case VK_OBJECT_TYPE_SHADER_MODULE:
            return "VK_OBJECT_TYPE_SHADER_MODULE";
        case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
            return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
        case VK_OBJECT_TYPE_SAMPLER:
            return "VK_OBJECT_TYPE_SAMPLER";
        case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:
            return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX";
        case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
            return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
        case VK_OBJECT_TYPE_IMAGE:
            return "VK_OBJECT_TYPE_IMAGE";
        case VK_OBJECT_TYPE_UNKNOWN:
            return "VK_OBJECT_TYPE_UNKNOWN";
        case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
            return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
        case VK_OBJECT_TYPE_COMMAND_BUFFER:
            return "VK_OBJECT_TYPE_COMMAND_BUFFER";
        case VK_OBJECT_TYPE_BUFFER:
            return "VK_OBJECT_TYPE_BUFFER";
        case VK_OBJECT_TYPE_SURFACE_KHR:
            return "VK_OBJECT_TYPE_SURFACE_KHR";
        case VK_OBJECT_TYPE_INSTANCE:
            return "VK_OBJECT_TYPE_INSTANCE";
        case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
            return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
        case VK_OBJECT_TYPE_IMAGE_VIEW:
            return "VK_OBJECT_TYPE_IMAGE_VIEW";
        case VK_OBJECT_TYPE_DESCRIPTOR_SET:
            return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
        case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
            return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
        case VK_OBJECT_TYPE_COMMAND_POOL:
            return "VK_OBJECT_TYPE_COMMAND_POOL";
        case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
            return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
        case VK_OBJECT_TYPE_DISPLAY_KHR:
            return "VK_OBJECT_TYPE_DISPLAY_KHR";
        case VK_OBJECT_TYPE_BUFFER_VIEW:
            return "VK_OBJECT_TYPE_BUFFER_VIEW";
        case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
            return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
        case VK_OBJECT_TYPE_FRAMEBUFFER:
            return "VK_OBJECT_TYPE_FRAMEBUFFER";
        case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
            return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
        case VK_OBJECT_TYPE_PIPELINE_CACHE:
            return "VK_OBJECT_TYPE_PIPELINE_CACHE";
        case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
            return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
        case VK_OBJECT_TYPE_DEVICE_MEMORY:
            return "VK_OBJECT_TYPE_DEVICE_MEMORY";
        case VK_OBJECT_TYPE_FENCE:
            return "VK_OBJECT_TYPE_FENCE";
        case VK_OBJECT_TYPE_QUEUE:
            return "VK_OBJECT_TYPE_QUEUE";
        case VK_OBJECT_TYPE_DEVICE:
            return "VK_OBJECT_TYPE_DEVICE";
        case VK_OBJECT_TYPE_RENDER_PASS:
            return "VK_OBJECT_TYPE_RENDER_PASS";
        case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
            return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
        case VK_OBJECT_TYPE_EVENT:
            return "VK_OBJECT_TYPE_EVENT";
        case VK_OBJECT_TYPE_PIPELINE:
            return "VK_OBJECT_TYPE_PIPELINE";
        default:
            return "Unhandled VkObjectType";
    }
}

std::string VkDebugReportFlagsToString(VkDebugReportFlagsEXT flags)
{
    std::string result = "";
    auto buildDebugReportTypeString = [](VkDebugReportFlagsEXT flags, VkDebugReportFlagBitsEXT flag, std::string flagText, std::string & flagsText)
    {
        if (flags & flag)
        {
            if (flagsText.size()>0) flagsText += ",";
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
        vxLogError3("%s: %s", "VulkanValidation", debugReportType.c_str(), pMessage);

        if (!(strstr(pMessage, "Device Extension VK_KHR_surface is not supported by this layer."))
            &!(strstr(pMessage, "Device Extension VK_MVK_macos_surface is not supported by this layer."))
            )
        {
            auto a = 0;//breakpoint
        }
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        vxLogWarning3("%s: %s", "VulkanValidation", debugReportType.c_str(), pMessage);
    }
    else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        vxLogInfo3("%s: %s", "VulkanValidation", debugReportType.c_str(), pMessage);
    }
	return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData) {
    char prefix[64] = "";
    char *message = (char *)malloc(strlen(pCallbackData->pMessage) + 5000);
    assert(message);
    struct demo *demo = (struct demo *)pUserData;

    /*
    if (demo->use_break) {
#ifndef WIN32
        raise(SIGTRAP);
#else
        DebugBreak();
#endif
    }
    */

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        strcat(prefix, "VERBOSE : ");
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        strcat(prefix, "INFO : ");
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        strcat(prefix, "WARNING : ");
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        strcat(prefix, "ERROR : ");
    }

    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        strcat(prefix, "GENERAL");
    } else {
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
            strcat(prefix, "VALIDATION");
            //validation_error = 1;
        }
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
                strcat(prefix, "|");
            }
            strcat(prefix, "PERFORMANCE");
        }
    }

    sprintf(message, "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", prefix, pCallbackData->messageIdNumber,
            pCallbackData->pMessageIdName, pCallbackData->pMessage);
    if (pCallbackData->objectCount > 0) {
        char tmp_message[500];
        sprintf(tmp_message, "\n\tObjects - %d\n", pCallbackData->objectCount);
        strcat(message, tmp_message);
        for (uint32_t object = 0; object < pCallbackData->objectCount; ++object) {
            if (NULL != pCallbackData->pObjects[object].pObjectName && strlen(pCallbackData->pObjects[object].pObjectName) > 0) {
                sprintf(tmp_message, "\t\tObject[%d] - %s, Handle %p, Name \"%s\"\n", object,
                        string_VkObjectType(pCallbackData->pObjects[object].objectType),
                        (void *)(pCallbackData->pObjects[object].objectHandle), pCallbackData->pObjects[object].pObjectName);
            } else {
                sprintf(tmp_message, "\t\tObject[%d] - %s, Handle %p\n", object,
                        string_VkObjectType(pCallbackData->pObjects[object].objectType),
                        (void *)(pCallbackData->pObjects[object].objectHandle));
            }
            strcat(message, tmp_message);
        }
    }
    if (pCallbackData->cmdBufLabelCount > 0) {
        char tmp_message[500];
        sprintf(tmp_message, "\n\tCommand Buffer Labels - %d\n", pCallbackData->cmdBufLabelCount);
        strcat(message, tmp_message);
        for (uint32_t cmd_buf_label = 0; cmd_buf_label < pCallbackData->cmdBufLabelCount; ++cmd_buf_label) {
            sprintf(tmp_message, "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", cmd_buf_label,
                    pCallbackData->pCmdBufLabels[cmd_buf_label].pLabelName, pCallbackData->pCmdBufLabels[cmd_buf_label].color[0],
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[1], pCallbackData->pCmdBufLabels[cmd_buf_label].color[2],
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[3]);
            strcat(message, tmp_message);
        }
    }

#ifdef _WIN32

    in_callback = true;
    if (!demo->suppress_popups)
        MessageBox(NULL, message, "Alert", MB_OK);
    in_callback = false;

#elif defined(ANDROID)

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        __android_log_print(ANDROID_LOG_INFO,  APP_SHORT_NAME, "%s", message);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        __android_log_print(ANDROID_LOG_WARN,  APP_SHORT_NAME, "%s", message);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        __android_log_print(ANDROID_LOG_ERROR, APP_SHORT_NAME, "%s", message);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        __android_log_print(ANDROID_LOG_VERBOSE, APP_SHORT_NAME, "%s", message);
    } else {
        __android_log_print(ANDROID_LOG_INFO,  APP_SHORT_NAME, "%s", message);
    }

#else

    printf("%s\n", message);
    fflush(stdout);

#endif

    free(message);

    // Don't bail out, but keep going.
    return false;
}

VkResult _vxCreateDebugReportCallback(const spt(VxGraphicsDebug) & spVxGraphicsDebug)
{
    vxLogInfo2("Creating debug report callback", "Vulkan");
    GetAndAssertSharedPointerVk(spVxGraphicsInstance, spVxGraphicsDebug->wpVxGraphicsInstance);
    AssertNotNullVkResult(spVxGraphicsInstance->vkInstance);
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCreateDebugReportCallbackEXT");
    AssertNotNullVkResult(vkCreateDebugReportCallbackEXT);
    VkDebugReportCallbackCreateInfoEXT createInfo = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT };
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfo.pfnCallback = &debugReportCallback;
	StoreAndAssertVkResultP(spVxGraphicsDebug->vkCreateDebugReportCallbackResult, vkCreateDebugReportCallbackEXT, spVxGraphicsInstance->vkInstance, &createInfo, nullptr, &spVxGraphicsDebug->vkDebugReportCallback);
    vxLogInfo2("Debug report callback created.", "Vulkan");


    vxLogInfo2("Getting debug messenger functions", "Vulkan");
    spVxGraphicsDebug->vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCreateDebugUtilsMessengerEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkCreateDebugUtilsMessengerEXT);
    spVxGraphicsDebug->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkDestroyDebugUtilsMessengerEXT);
    spVxGraphicsDebug->vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkSubmitDebugUtilsMessageEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkSubmitDebugUtilsMessageEXT);
    spVxGraphicsDebug->vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCmdBeginDebugUtilsLabelEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkCmdBeginDebugUtilsLabelEXT);
    spVxGraphicsDebug->vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCmdEndDebugUtilsLabelEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkCmdEndDebugUtilsLabelEXT);
    spVxGraphicsDebug->vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkCmdInsertDebugUtilsLabelEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkCmdInsertDebugUtilsLabelEXT);
    spVxGraphicsDebug->vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkSetDebugUtilsObjectNameEXT");
    AssertNotNullVkResult(spVxGraphicsDebug->vkSetDebugUtilsObjectNameEXT);
    vxLogInfo2("Debug messenger functions obtained.", "Vulkan");

    #ifdef _DEBUG
    vxLogInfo2("Creating debug messenger callbacks", "Vulkan");
    VkDebugUtilsMessengerCreateInfoEXT dbgMessengerCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    dbgMessengerCreateInfo.pNext = NULL;
    dbgMessengerCreateInfo.flags = 0;
    dbgMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ;//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    dbgMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    dbgMessengerCreateInfo.pfnUserCallback = debug_messenger_callback;
    dbgMessengerCreateInfo.pUserData = spVxGraphicsDebug.get();
	StoreAndAssertVkResultP(spVxGraphicsDebug->vkCreateDebugMessengerResult, spVxGraphicsDebug->vkCreateDebugUtilsMessengerEXT, spVxGraphicsInstance->vkInstance, &dbgMessengerCreateInfo, nullptr, &spVxGraphicsDebug->vkDebugMessenger);
    vxLogInfo2("Debug report messenger callbacks created.", "Vulkan");
    #endif

    #ifdef _DEBUG && VK_USE_PLATFORM_MACOS_MVK
    PFN_vkGetMoltenVKConfigurationMVK vkGetMoltenVKConfigurationMVK = (PFN_vkGetMoltenVKConfigurationMVK)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkGetMoltenVKConfigurationMVK");
    AssertNotNullVkResult(vkGetMoltenVKConfigurationMVK);
    PFN_vkSetMoltenVKConfigurationMVK vkSetMoltenVKConfigurationMVK = (PFN_vkSetMoltenVKConfigurationMVK)vkGetInstanceProcAddr(spVxGraphicsInstance->vkInstance, "vkSetMoltenVKConfigurationMVK");
    AssertNotNullVkResult(vkSetMoltenVKConfigurationMVK);
    MVKConfiguration mvkConfig;
    size_t pConfigurationSize = sizeof(MVKConfiguration);
    AssertVkResult(vkGetMoltenVKConfigurationMVK, spVxGraphicsInstance->vkInstance, &mvkConfig, &pConfigurationSize);
    mvkConfig.debugMode = true;
    AssertVkResult(vkSetMoltenVKConfigurationMVK, spVxGraphicsInstance->vkInstance, &mvkConfig, &pConfigurationSize);
    #endif

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

