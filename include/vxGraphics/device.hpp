#ifndef VX_GRAPHICS_DEVICE_HEADER
#define VX_GRAPHICS_DEVICE_HEADER

#include "instance.hpp"

struct VxGraphicsDevice;
struct VxGraphicsPhysicalDevice;

typedef struct VxGraphicsDeviceQueueFamily
{
    VkQueueFamilyProperties                 vkQueueFamily;
    uint32_t                                queueFamilyIndex;
    bool                                    supportsPresentation;
    bool                                    supportsCompute;
    bool                                    supportsGraphics;

    VkResult                                vkCreateCommandPoolResult;
    VkCommandPool                           vkCommandPool;

    VkResult                                vkAllocateCommandBufferResult;
    VkCommandBuffer                         vkCommandBuffer;

    wpt(VxGraphicsDevice)                   wpVxGraphicsDevice;

    ~VxGraphicsDeviceQueueFamily();
    void destroy();
    void init(spt(VxGraphicsDevice) spVxGraphicsDevice)
    {
        wpVxGraphicsDevice = spVxGraphicsDevice;
        vkQueueFamily = {};
        queueFamilyIndex = 0;
        supportsPresentation = false;
        supportsCompute = false;
        supportsGraphics = false;
        vkCreateCommandPoolResult = VK_RESULT_MAX_ENUM;
        vkCommandPool = nullptr;
    }
} VxGraphicsDeviceQueueFamily;

typedef struct VxGraphicsQueue
{
    wpt(VxGraphicsDeviceQueueFamily)        wpVxGraphicsDeviceQueueFamily;
    VkQueue                                 vkQueue;
    uint32_t                                queueIndex;
    uint32_t                                queueFamilyIndex;

    ~VxGraphicsQueue();
    void destroy();
    void init(spt(VxGraphicsDeviceQueueFamily) spVxGraphicsDeviceQueueFamily)
    {
        wpVxGraphicsDeviceQueueFamily = spVxGraphicsDeviceQueueFamily;
        vkQueue = {};
        queueIndex = 0;
        queueFamilyIndex = 0;
    }
} VxGraphicsQueue;

typedef struct VxSemaphore
{
    wpt(VxGraphicsDevice)                   wpVxDevice;
    VkResult                                createSemaphoreResult;
    VkSemaphore                             vkSemaphore;

    ~VxSemaphore();
    void destroy();
    void init(spt(VxGraphicsDevice) spVxSurfaceDevice)
    {
        wpVxDevice = spVxSurfaceDevice;
        createSemaphoreResult = VK_RESULT_MAX_ENUM;
        vkSemaphore = nullptr;
    }
} VxSemaphore;

typedef struct VxGraphicsDevice
{
    wpt(VxGraphicsPhysicalDevice)           wpVxPhysicalDevice;
    VkResult                                createDeviceResult;
    VkDevice                                vkDevice;
    vectorS(VxGraphicsQueue)                spVxQueues;
    vectorS(VxGraphicsDeviceQueueFamily)    spVxQueueFamilies;
    setS(VxSemaphore)                       spVxSemaphores;

    VkResult                                createMemoryAllocatorResult;
    spt(VxMemoryAllocator)                  spVxMemoryAllocator;

    VkResult createVxSemaphore(spt(VxGraphicsDevice) spVxGraphicsDevice, spt(VxSemaphore) & spVxSemaphore);
    void destroyVxSemaphore(spt(VxSemaphore) spVxSemaphore);
    
    ~VxGraphicsDevice();
    void destroy();
    void init()
    {
        vkDevice = nullptr;
        spVxQueues.reserve(32);
        spVxQueueFamilies.reserve(32);

        createMemoryAllocatorResult = VK_RESULT_MAX_ENUM;
        spVxMemoryAllocator = nullptr;
    }
} VxGraphicsDevice;



//VkResult vxCreateDevices(const upt(VxGraphicsInstance) & upGraphicsInstance);
//VkResult vxAllocateCommandBuffer(spt(VxGraphicsInstance) & spVxGraphicsInstance, VkCommandBuffer & commandBuffer);

#endif

