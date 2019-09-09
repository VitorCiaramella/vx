#ifndef VX_GRAPHICS_MEMORY_HEADER
#define VX_GRAPHICS_MEMORY_HEADER

#include <vulkan/vulkan.hpp>
#include <vxCommon/vxCommon.hpp>
#include <vxGraphics/vxGraphics.hpp>
#include "../../extern/VulkanMemoryAllocator/src/vk_mem_alloc.h"

struct VxMemoryAllocator;
struct VxGraphicsDevice;

typedef struct VxMemoryBuffer
{
    wpt(VxMemoryAllocator) wpVxMemoryAllocator;
    VkResult createBufferResult;
    VkBuffer vkBuffer;
    VmaAllocation vmaAllocation;
    VkResult mapMemoryResult;
    void* data;

    ~VxMemoryBuffer();
    void destroy();
    void init(spt(VxMemoryAllocator) spVxMemoryAllocator)
    {
        wpVxMemoryAllocator = spVxMemoryAllocator;
        createBufferResult = VK_RESULT_MAX_ENUM;
        vkBuffer = nullptr;
        vmaAllocation = nullptr;        
        mapMemoryResult = VK_RESULT_MAX_ENUM;
        data = nullptr;
    }
} VxMemoryBuffer;

typedef struct VxMemoryAllocator
{
    VkResult                    createAllocatorResult;
    VmaAllocator                vmaAllocator;
    vectorS(VxMemoryBuffer)     spMemoryBuffers; 

    ~VxMemoryAllocator();
    void destroy();
    void init()
    {
        createAllocatorResult = VK_RESULT_MAX_ENUM;
        vmaAllocator = nullptr;
        spMemoryBuffers.reserve(32);
    }
} VxMemoryAllocator;

VkResult vxCreateMemoryAllocator(spt(VxGraphicsDevice) spVxGraphicsDevice, spt(VxMemoryAllocator) & spVxMemoryAllocator);
VkResult vxCreateVertexBuffer(spt(VxMemoryAllocator) spVxMemoryAllocator, VkDeviceSize bufferSize, spt(VxMemoryBuffer) & spMemoryBuffer);
VkResult vxCreateIndexBuffer(spt(VxMemoryAllocator) spVxMemoryAllocator, VkDeviceSize bufferSize, spt(VxMemoryBuffer) & spMemoryBuffer);


#endif

