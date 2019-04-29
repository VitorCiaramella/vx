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
    spVxQueueFamilies.clear();
    spVxQueues.clear();
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
