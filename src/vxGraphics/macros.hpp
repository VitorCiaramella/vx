#ifndef VX_GRAPHICS_MACROS
#define VX_GRAPHICS_MACROS

#define AssertVkResult(vkFunction, ...) \
    {\
        auto vkResult = vkFunction(__VA_ARGS__); \
        if (vkResult != VK_SUCCESS) \
        { \
            vxLogVkResult2(vkResult, #vkFunction); \
            return vkResult; \
        } \
    }

#define AssertTrueVkResult(check) \
    {\
        if (!(check)) \
        { \
            vxLogWarning2("Assert failed.","Assert"); \
            return VK_ERROR_VALIDATION_FAILED_EXT; \
        } \
    }

#define AssertNotNullVkResult(object) \
    {\
        if (object == nullptr) \
        { \
            vxLogWarning3("Assert failed. #s is null.", "Memory", #object); \
            return VK_ERROR_VALIDATION_FAILED_EXT; \
        } \
    }

#define StoreAndAssertVkResultP(resultStorage, vkFunction, ...) \
    resultStorage = vkFunction(__VA_ARGS__); \
    if (resultStorage != VK_SUCCESS) \
    { \
        vxLogVkResult2(resultStorage, #vkFunction); \
        return resultStorage; \
    } 

#define GetAndAssertSharedPointerVk(variable, wpObject) \
    if (wpObject.expired()) \
    { \
        vxLogWarning3("Trying to access expired pointer %s", "Memory", #wpObject); \
        return VK_ERROR_INVALID_EXTERNAL_HANDLE; \
    } \
    auto variable = wpObject.lock(); 

#define AssertVkResultVxWindowLoop(vkFunction, ...) \
    {\
        auto vkresult = vkFunction(__VA_ARGS__); \
        if (vkresult != VK_SUCCESS) \
        { \
            return VxWindowLoopResult::VX_WL_STOP;\
        } \
    }

#define AssertVkVxResult(vkFunction, ...) \
    {\
        auto vkresult = vkFunction(__VA_ARGS__); \
        if (vkresult != VK_SUCCESS) \
        { \
            return VxResult::VX_ERROR; \
        } \
    }

#define CopyResultItemsSP(storage, storageProperty, itemType, items, itemCount) \
    {\
        storage.resize(itemCount); \
        for (uint32_t i=0; i<itemCount; i++) \
        { \
            storage[i] = nsp<itemType>(); \
            storage[i]->storageProperty = items[i]; \
        } \
    }

#define AppendResultItemsSP(storage, storageProperty, itemType, items, itemCount) \
    {\
        auto baseIndex = storage.size(); \\
        storage.resize(itemCount+storage.size()); \
        for (uint32_t i=0; i<itemCount; i++) \
        { \
            storage[baseIndex+i] = nsp<itemType>(); \
            storage[baseIndex+i]->storageProperty = items[i]; \
        } \
    }

#endif
