#include <iostream>
#include <vector>
#include <string>

#define VK_USE_PLATFORM_MACOS_MVK
#define VK_USE_PLATFORM_METAL_EXT

#include <vxGraphics/vxGraphics.hpp>

//# GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

using namespace std;


void vx_glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

VxResult vx_glfxGetWindow(GLFWwindow* &window)
{
    glfwSetErrorCallback(vx_glfwErrorCallback);

    if (!glfwInit())
    {
        return VxResult::VX_ERROR;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

    if (!window)
    {
        return VxResult::VX_ERROR;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    return VxResult::VX_SUCCESS;
}

VxResult vx_glfxAwaitWindowClose(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return VxResult::VX_SUCCESS;
}

int main()
{
    VxResult vxresult;

    VxGraphicsInstanceCreateInfo graphicsInstanceCreateInfo = {};
    graphicsInstanceCreateInfo.applicationName = "vxGraphics";
    graphicsInstanceCreateInfo.applicationVersion = VX_MAKE_VERSION(0,1,0);
    graphicsInstanceCreateInfo.engineName = "vxGraphics";
    graphicsInstanceCreateInfo.engineVersion = VX_MAKE_VERSION(0,1,0);
    graphicsInstanceCreateInfo.apiVersion = VK_API_VERSION_1_1;
    #ifdef _DEBUG
    graphicsInstanceCreateInfo.desiredLayersToEnable.push_back("VK_LAYER_KHRONOS_validation");
    graphicsInstanceCreateInfo.desiredLayersToEnable.push_back("VK_LAYER_LUNARG_standard_validation");
    #endif

    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_8BIT_STORAGE_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_16BIT_STORAGE_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);

    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    #ifdef VK_USE_PLATFORM_MACOS_MVK
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back("VK_MVK_macos_surface");  
    #endif
    #ifdef VK_USE_PLATFORM_WIN32_KHR
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    #endif
    #ifdef _DEBUG
    graphicsInstanceCreateInfo.desiredExtensionsToEnable.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    #endif

    graphicsInstanceCreateInfo.desiredDeviceCount = 2;
    graphicsInstanceCreateInfo.desiredQueueCountPerDevice = 20;

    upt(VxGraphicsInstance) upGraphicsInstance;
    vxresult = vxCreateGraphicsInstance(&graphicsInstanceCreateInfo, upGraphicsInstance);
    if (vxresult != VxResult::VX_SUCCESS)
    {
        return -1;
    }

    VkCommandBuffer commandBuffer;
    vxresult = vxAllocateCommandBuffer(upGraphicsInstance, commandBuffer);
    if (vxresult != VxResult::VX_SUCCESS)
    {
        return -1;
    }
    //vkResetCommandPool(device,commandPool,0);
    //vkDestroyCommandPool(device,commandPool,nullptr);
    //vkResetCommandBuffer(commandBuffer,0);
    //vkFreeCommandBuffers(device,commandPool,commandBufferCount,pCommandBuffers);
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    auto queue = upGraphicsInstance->devices[0].queues[0].queue;
    VkSubmitInfo submits = {};
    uint32_t submitCount = 1;
    VkFence fence = VK_NULL_HANDLE;
    vkQueueSubmit(queue,submitCount,&submits,fence);

    vkEndCommandBuffer(commandBuffer);

    vkDestroyInstance(upGraphicsInstance->vkInstance, nullptr);

    GLFWwindow* window;
    if (vx_glfxGetWindow(window) == VxResult::VX_SUCCESS)
    {
        vx_glfxAwaitWindowClose(window);
        glfwDestroyWindow(window);
    }
    glfwTerminate();    
}

