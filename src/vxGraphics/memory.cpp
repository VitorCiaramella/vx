#include "commonHeaders.hpp"

#define VMA_IMPLEMENTATION
#include "../../extern/VulkanMemoryAllocator/src/vk_mem_alloc.h"


VxMemoryAllocator::~VxMemoryAllocator()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxMemoryAllocator::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    for(auto && spMemoryBuffer : spMemoryBuffers)
    {
        if (spMemoryBuffer->data != nullptr)
        {
            vmaUnmapMemory(vmaAllocator, spMemoryBuffer->vmaAllocation);
            spMemoryBuffer->data = nullptr;
        }
        if (spMemoryBuffer->vkBuffer != nullptr)
        {
            vmaDestroyBuffer(vmaAllocator, spMemoryBuffer->vkBuffer, spMemoryBuffer->vmaAllocation);
            spMemoryBuffer->vkBuffer = nullptr;
        }
    }
    spMemoryBuffers.clear();
    if (vmaAllocator != nullptr)
    {
        vmaDestroyAllocator(vmaAllocator);
        vmaAllocator = nullptr;
    }
}
 
VxMemoryBuffer::~VxMemoryBuffer()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxMemoryBuffer::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);

    GetAndAssertSharedPointer(spVxMemoryAllocator, wpVxMemoryAllocator);
    if (data != nullptr)
    {
        vmaUnmapMemory(spVxMemoryAllocator->vmaAllocator, vmaAllocation);
        data = nullptr;
    }
    if (vkBuffer != nullptr)
    {
        vmaDestroyBuffer(spVxMemoryAllocator->vmaAllocator, vkBuffer, vmaAllocation);
        vkBuffer = nullptr;
    }
}

VkResult vxCreateMemoryAllocator(spt(VxGraphicsDevice) spVxGraphicsDevice, spt(VxMemoryAllocator) & spVxMemoryAllocator)
{
    spVxMemoryAllocator = nsp<VxMemoryAllocator>();
    GetAndAssertSharedPointerVk(spVxPhysicalDevice, spVxGraphicsDevice->wpVxPhysicalDevice);

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = spVxPhysicalDevice->vkPhysicalDevice;
    allocatorInfo.device = spVxGraphicsDevice->vkDevice;
    spVxMemoryAllocator->createAllocatorResult = vmaCreateAllocator(&allocatorInfo, &spVxMemoryAllocator->vmaAllocator);
    return spVxMemoryAllocator->createAllocatorResult;
}

VkResult vxCreateVertexBuffer(spt(VxMemoryAllocator) spVxMemoryAllocator, VkDeviceSize bufferSize, spt(VxMemoryBuffer) & spMemoryBuffer)
{
    spMemoryBuffer = nsp<VxMemoryBuffer>(spVxMemoryAllocator);
    spVxMemoryAllocator->spMemoryBuffers.push_back(spMemoryBuffer);
    
    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    StoreAndAssertVkResultP(spMemoryBuffer->createBufferResult, vmaCreateBuffer, spVxMemoryAllocator->vmaAllocator, &bufferInfo, &allocInfo, &spMemoryBuffer->vkBuffer, &spMemoryBuffer->vmaAllocation, nullptr);
    StoreAndAssertVkResultP(spMemoryBuffer->mapMemoryResult, vmaMapMemory, spVxMemoryAllocator->vmaAllocator, spMemoryBuffer->vmaAllocation, &spMemoryBuffer->data);

    return VK_SUCCESS;
}

VkResult vxCreateIndexBuffer(spt(VxMemoryAllocator) spVxMemoryAllocator, VkDeviceSize bufferSize, spt(VxMemoryBuffer) & spMemoryBuffer)
{
    spMemoryBuffer = nsp<VxMemoryBuffer>(spVxMemoryAllocator);
    spVxMemoryAllocator->spMemoryBuffers.push_back(spMemoryBuffer);

    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    StoreAndAssertVkResultP(spMemoryBuffer->createBufferResult, vmaCreateBuffer, spVxMemoryAllocator->vmaAllocator, &bufferInfo, &allocInfo, &spMemoryBuffer->vkBuffer, &spMemoryBuffer->vmaAllocation, nullptr);
    StoreAndAssertVkResultP(spMemoryBuffer->mapMemoryResult, vmaMapMemory, spVxMemoryAllocator->vmaAllocator, spMemoryBuffer->vmaAllocation, &spMemoryBuffer->data);

    return VK_SUCCESS;
}
