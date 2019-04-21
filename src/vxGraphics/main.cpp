#include <iostream>
#include <vector>
#include <string>

#include <vulkan/vulkan.h>

//#define GLFW_INCLUDE_VULKAN 

#include <GLFW/glfw3.h>

using namespace std;

typedef enum class VxResult {
    VX_SUCCESS = 0,
    VX_ERROR_INITIALIZATION_FAILED = -3,
} VxResult;

void vx_glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

VxResult vx_glfxGetWindow(GLFWwindow* &window)
{
    glfwSetErrorCallback(vx_glfwErrorCallback);

    if (!glfwInit())
    {
        return VxResult::VX_ERROR_INITIALIZATION_FAILED;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

    if (!window)
    {
        return VxResult::VX_ERROR_INITIALIZATION_FAILED;
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
    VkInstance instance;
    VkResult result;
    VkInstanceCreateInfo info = {};
    uint32_t instance_layer_count;

    result = vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);
    std::cout << instance_layer_count << " layers found!\n";
    if (instance_layer_count > 0) {
        std::unique_ptr<VkLayerProperties[]> instance_layers(new VkLayerProperties[instance_layer_count]);
        result = vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers.get());
        for (int i = 0; i < instance_layer_count; ++i) {
            std::cout << instance_layers[i].layerName << "\n";
        }
    }

    const char * names[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    info.enabledLayerCount = 1;
    info.ppEnabledLayerNames = names;

    result = vkCreateInstance(&info, NULL, &instance);
    std::cout << "vkCreateInstance result: " << result  << "\n";

    vkDestroyInstance(instance, nullptr);
    
    GLFWwindow* window;
    if (vx_glfxGetWindow(window) == VxResult::VX_SUCCESS)
    {
        vx_glfxAwaitWindowClose(window);
        glfwDestroyWindow(window);
    }
    glfwTerminate();    
}

