#include <vxGraphics/vxGraphics.hpp>

//TODO: make it a function pointer
void vxErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

VxResult vxCreateWindow(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    glfwSetErrorCallback(vxErrorCallback);

    if (!glfwInit())
    {
        return VxResult::VX_ERROR;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window = glfwCreateWindow(upGraphicsInstance->rpCreateInfo->mainWindowWidth, upGraphicsInstance->rpCreateInfo->mainWindowHeight, "My Title", NULL, NULL);

    if (!window)
    {
        return VxResult::VX_ERROR;
    }

    upGraphicsInstance->mainWindow = window;

    return VxResult::VX_SUCCESS;
}

VkExtent2D vxGetWindowSize(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    VkExtent2D result;
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(upGraphicsInstance->mainWindow, &width, &height);
    result.width = width;
    result.height = height;
    return result;
}

VkResult vxAwaitWindowClose(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    if (upGraphicsInstance->vxWindowLoopFunction == nullptr)
    {
        return VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE;
    }
    while (!glfwWindowShouldClose(upGraphicsInstance->mainWindow))
    {
        glfwPollEvents();
        upGraphicsInstance->windowSize = vxGetWindowSize(upGraphicsInstance);
        auto loopResult = upGraphicsInstance->vxWindowLoopFunction(upGraphicsInstance);        
        if (loopResult != VxWindowLoopResult::VX_WL_CONTINUE)
        {
            break;
        }
    }
    return VkResult::VK_SUCCESS;
}
