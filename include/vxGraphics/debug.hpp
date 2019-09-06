#ifndef VX_GRAPHICS_DEBUG_HEADER
#define VX_GRAPHICS_DEBUG_HEADER

#include "instance.hpp"
#include <string>

#ifdef _DEBUG
    #define vxLogVkResult(vkResult) vxGraphicsDebugLogVkResult(vkResult, nullptr, __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogVkResult2(vkResult, vulkanFunctionName) vxGraphicsDebugLogVkResult(vkResult, vulkanFunctionName, __FUNCTION_NAME__, __FILE__, __LINE__)
#else
    #define vxLogVkResult(vkResult)
    #define vxLogVkResult2(vkResult)
#endif
void vxGraphicsDebugLogVkResult(const VkResult vkResult, const char* vulkanFunctionName, const char* functionName, const char* fileName, const int line);

struct VxGraphicsInstance;

typedef struct VxGraphicsDebug 
{   
    wpt(VxGraphicsInstance)                 wpVxGraphicsInstance;

    VkResult                                vkCreateDebugReportCallbackResult;
    VkDebugReportCallbackEXT                vkDebugReportCallback;

    VkResult                                vkCreateDebugMessengerResult;
    VkDebugUtilsMessengerEXT                vkDebugMessenger;
    PFN_vkCreateDebugUtilsMessengerEXT      vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT     vkDestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT        vkSubmitDebugUtilsMessageEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT        vkCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT          vkCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT       vkCmdInsertDebugUtilsLabelEXT;
    PFN_vkSetDebugUtilsObjectNameEXT        vkSetDebugUtilsObjectNameEXT;

    ~VxGraphicsDebug();
    void destroy();
    void init(spt(VxGraphicsInstance) spVxGraphicsInstance)
    {
        wpVxGraphicsInstance = spVxGraphicsInstance;
        vkCreateDebugReportCallbackResult = VK_RESULT_MAX_ENUM;
        vkDebugReportCallback = nullptr;
        vkCreateDebugMessengerResult = VK_RESULT_MAX_ENUM;
        vkDebugMessenger = nullptr;
        vkCreateDebugUtilsMessengerEXT = nullptr;
        vkDestroyDebugUtilsMessengerEXT = nullptr;
        vkSubmitDebugUtilsMessageEXT = nullptr;
        vkCmdBeginDebugUtilsLabelEXT = nullptr;
        vkCmdEndDebugUtilsLabelEXT = nullptr;
        vkCmdInsertDebugUtilsLabelEXT = nullptr;
        vkSetDebugUtilsObjectNameEXT = nullptr;
    }
} VxGraphicsDebug;

#ifdef _DEBUG
    #define vxCreateDebugReportCallback(spVxGraphicsDebug) _vxCreateDebugReportCallback(spVxGraphicsDebug)
#else
    #define vxCreateDebugReportCallback(spVxGraphicsDebug) vxGetVkSuccess()
#endif

VkResult _vxCreateDebugReportCallback(const spt(VxGraphicsDebug) & spVxGraphicsDebug);

#endif

