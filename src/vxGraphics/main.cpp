#include <iostream>
#include <vector>
#include <string>

#include <vxGraphics/vxGraphics.hpp>


using namespace std;

int main()
{
    VxResult vxresult;

    VxGraphicsInstanceCreateInfo graphicsInstanceCreateInfo = {};
    graphicsInstanceCreateInfo.applicationName = "vxGraphics";
    graphicsInstanceCreateInfo.applicationVersion = VX_MAKE_VERSION(0,1,0);
    graphicsInstanceCreateInfo.engineName = "vxGraphics";
    graphicsInstanceCreateInfo.engineVersion = VX_MAKE_VERSION(0,1,0);
    graphicsInstanceCreateInfo.apiVersion = VK_API_VERSION_1_1;

    graphicsInstanceCreateInfo.mainWindowWidth = 1024;
    graphicsInstanceCreateInfo.mainWindowHeight = 768;

    #ifdef _DEBUG
    //graphicsInstanceCreateInfo.desiredLayersToEnable.push_back("VK_LAYER_KHRONOS_validation");
    graphicsInstanceCreateInfo.desiredLayersToEnable.push_back("VK_LAYER_LUNARG_standard_validation");
    #endif

    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_8BIT_STORAGE_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_16BIT_STORAGE_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);   
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);

    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    #ifdef VK_USE_PLATFORM_MACOS_MVK
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);  
    #endif
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif
    #ifdef _DEBUG
    //graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    #endif

    graphicsInstanceCreateInfo.desiredDeviceCount = 1;
    graphicsInstanceCreateInfo.desiredQueueCountPerDevice = 1;

	graphicsInstanceCreateInfo.shadersFilePaths.push_back("triangle.vert.spv");
	graphicsInstanceCreateInfo.shadersFilePaths.push_back("triangle.frag.spv");

    upt(VxGraphicsInstance) upGraphicsInstance;
    vxresult = vxCreateGraphicsInstance(&graphicsInstanceCreateInfo, upGraphicsInstance);
    if (vxresult != VxResult::VX_SUCCESS)
    {
        return -1;
    }

    vxGraphicsRun(upGraphicsInstance);
    
    vxGraphicsDestroyInstance(upGraphicsInstance);
    vxGraphicsTerminate(upGraphicsInstance);

}

