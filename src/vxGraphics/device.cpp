#include "commonHeaders.hpp"

#include <algorithm>

void VxGraphicsDeviceQueueFamily::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    if (vkCommandPool != nullptr)
    {
        GetAndAssertSharedPointer(spVxGraphicsDevice,wpVxGraphicsDevice);
        AssertNotNull(spVxGraphicsDevice->vkDevice);
        vkDestroyCommandPool(spVxGraphicsDevice->vkDevice, vkCommandPool, nullptr);
        vkCommandPool = nullptr;
    }
}

VxGraphicsDeviceQueueFamily::~VxGraphicsDeviceQueueFamily()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsQueue::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
}

VxGraphicsQueue::~VxGraphicsQueue()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxGraphicsDevice::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    spVxSemaphores.clear();
    spVxQueueFamilies.clear();
    spVxQueues.clear();

    if (spVxMemoryAllocator != nullptr)
    {
        spVxMemoryAllocator->destroy();
        spVxMemoryAllocator = nullptr;
    }

    if (vkDevice != nullptr)
    {
        vkDeviceWaitIdle(vkDevice);
        vkDestroyDevice(vkDevice, nullptr);
        vkDevice = nullptr;
    }
}

VxGraphicsDevice::~VxGraphicsDevice()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

VkResult VxGraphicsDevice::createVxSemaphore(spt(VxGraphicsDevice) spVxGraphicsDevice, spt(VxSemaphore) & spVxSemaphore)
{
    spVxSemaphore = nsp<VxSemaphore>(spVxGraphicsDevice);
    VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    StoreAndAssertVkResultP(spVxSemaphore->createSemaphoreResult, vkCreateSemaphore, this->vkDevice, &createInfo, nullptr, &spVxSemaphore->vkSemaphore);
    this->spVxSemaphores.insert(spVxSemaphore);
    return VK_SUCCESS;
}

void VxGraphicsDevice::destroyVxSemaphore(spt(VxSemaphore) spVxSemaphore)
{
    this->spVxSemaphores.erase(spVxSemaphore);
    spVxSemaphore->destroy();
}

VxSemaphore::~VxSemaphore()
{
    vxLogInfo2("Destructor call", "Memory");
    destroy();
}

void VxSemaphore::destroy()
{
    vxLogInfo2("Destroy call", "Memory");
    AssertNotNull(this);
    GetAndAssertSharedPointer(spVxSurfaceDevice, wpVxDevice);
    createSemaphoreResult = VK_RESULT_MAX_ENUM;
    vkDestroySemaphore(spVxSurfaceDevice->vkDevice, vkSemaphore, nullptr);
    vkSemaphore = nullptr;
}
