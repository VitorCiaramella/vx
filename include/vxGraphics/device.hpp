#ifndef VX_GRAPHICS_DEVICE_HEADER
#define VX_GRAPHICS_DEVICE_HEADER

typedef struct VxGraphicsDeviceQueueFamily
{
    //~VxGraphicsDeviceQueueFamily();

    VkQueueFamilyProperties                 vkQueueFamily;
    uint32_t                                queueFamilyIndex;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                vkCreateCommandPoolResult;
    VkCommandPool                           vkCommandPool;
} VxGraphicsDeviceQueueFamily;
#define initVxGraphicsDeviceQueueFamily(object) \
    object->vkCreateCommandPoolResult = VK_RESULT_MAX_ENUM;

typedef struct VxGraphicsQueue
{
    VkQueue                                 vkQueue;
    uint32_t                                queueIndex;
    uint32_t                                queueFamilyIndex;
} VxGraphicsQueue;
#define initVxGraphicsQueue(object) 

typedef struct VxGraphicsDevice
{
    wpt(VxGraphicsPhysicalDevice)           wpVxPhysicalDevice;
    VkDevice                                vkDevice;
    std::vector<spt(VxGraphicsQueue)>       spVxQueues;
    std::vector<spt(VxGraphicsQueueFamily)> spVxQueueFamilies;
} VxGraphicsDevice;
#define initVxGraphicsDevice(object) 


VkResult vxCreateDevices(const upt(VxGraphicsInstance) & upGraphicsInstance);
VkResult vxAllocateCommandBuffer(spt(VxGraphicsInstance) & spVxGraphicsInstance, VkCommandBuffer & commandBuffer);

#endif

