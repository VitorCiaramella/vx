#include "commonHeaders.hpp"

VkResult vxCreateSemaphore(VkDevice device, VkSemaphore * semaphore)
{
	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	semaphore = nullptr;
	return vkCreateSemaphore(device, &createInfo, nullptr, semaphore);
}
