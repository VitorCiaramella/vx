#include "commonHeaders.hpp"

#include <algorithm>

/*
VkResult vxCreateDevices(const upt(VxGraphicsInstance) & upGraphicsInstance)
{
    printf("\nCreating Device\n");
 
    uint32_t desiredDeviceCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredDeviceCount,(uint32_t)1,(uint32_t)upGraphicsInstance->vxSelectedPhysicalDevices.size());
    for (auto && vxPhysicalDevice : upGraphicsInstance->vxSelectedPhysicalDevices) 
    {
        VxGraphicsDevice vxDevice = {};
        initVxGraphicsDevice(vxDevice);
        vxDevice.wpVxPhysicalDevice = vxPhysicalDevice;

        //TODO: instead of global queue count, do per desired device type
        uint32_t desiredQueueCount = std::clamp(upGraphicsInstance->rpCreateInfo->desiredQueueCountPerDevice,(uint32_t)1,(uint32_t)100);

        VkDeviceCreateInfo deviceCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

        auto rpQueueCreateInfos = nrp(std::vector<VkDeviceQueueCreateInfo>);
        for (auto && queueFamily : vxPhysicalDevice->vxQueueFamilies) 
        {
            if (queueFamily.supportsPresentation
                && queueFamily.supportsGraphics)
            {
                uint32_t queueCount = std::clamp(desiredQueueCount,(uint32_t)1,queueFamily.vkQueueFamily.queueCount);
                desiredQueueCount = std::clamp(desiredQueueCount-queueCount,(uint32_t)0,(uint32_t)100);
                float queuePriority = 1.f;
                auto queuePriorities = nrp(std::vector<float>,queueCount,queuePriority);

                VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
                queueCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;
                queueCreateInfo.queueCount = queueCount;
                queueCreateInfo.pQueuePriorities = queuePriorities->data();
                rpQueueCreateInfos->push_back(queueCreateInfo);

                vxDevice.vxQueueFamilies.push_back(queueFamily);
                for (uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++)
                {
                    VxGraphicsQueue vxQueue = {};
                    initVxGraphicsQueue(vxQueue);
                    vxQueue.queueFamilyIndex = queueFamily.queueFamilyIndex;
                    vxQueue.queueIndex = queueIndex;
                    vxDevice.vxQueues.push_back(vxQueue);
                }                
            }
            if (desiredQueueCount == 0)
            {
                break;
            }
        }
        deviceCreateInfo.queueCreateInfoCount = rpQueueCreateInfos->size();
        deviceCreateInfo.pQueueCreateInfos = rpQueueCreateInfos->data();
        deviceCreateInfo.pEnabledFeatures = &vxPhysicalDevice->vkPhysicalDeviceFeatures;

        auto extensionsToEnable = nrp(std::vector<char*>);
        for (auto && desiredExtensionToEnable : upGraphicsInstance->rpCreateInfo->desiredExtensionsToEnable) 
        {
            auto extensionSupported = false;
            for (auto && vkExtension : vxPhysicalDevice->vkAvailableExtensions) 
            {
                if (strcmp(desiredExtensionToEnable.c_str(), vkExtension.extensionName) == 0) 
                {
                    extensionSupported = true;
                    break;
                }           
            }
            if (extensionSupported)
            {
                extensionsToEnable->push_back(strToNewCharArray(desiredExtensionToEnable));
                printf("Desired Extension supported: %s\n", desiredExtensionToEnable.c_str());
            }
            else
            {
                //TODO: Handle this
                printf("Desired Extension not supported: %s\n", desiredExtensionToEnable.c_str());
            }
        }
        deviceCreateInfo.enabledExtensionCount = extensionsToEnable->size();
        deviceCreateInfo.ppEnabledExtensionNames = extensionsToEnable->data();
        
        StoreAndAssertVkResultP(upGraphicsInstance, vkCreateDevice, vxDevice.wpVxPhysicalDevice->vkPhysicalDevice, &deviceCreateInfo, nullptr, &vxDevice.vkDevice);
        for (auto && vxQueue : vxDevice.vxQueues) 
        {
            vkGetDeviceQueue(vxDevice.vkDevice, vxQueue.queueFamilyIndex, vxQueue.queueIndex, &vxQueue.vkQueue);
        }
        for (auto && vxQueueFamily : vxDevice.vxQueueFamilies) 
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = vxQueueFamily.queueFamilyIndex;                    
            StoreAndAssertVkResult(vxQueueFamily, vkCreateCommandPool, vxDevice.vkDevice, &commandPoolCreateInfo, nullptr, &vxQueueFamily.vkCommandPool);
        }

        upGraphicsInstance->vxDevices.push_back(vxDevice);

        if (upGraphicsInstance->vxDevices.size() >= desiredDeviceCount) 
        {
            break;
        }        
    }
    return VK_SUCCESS;
}

*/