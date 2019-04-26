#ifndef VX_GRAPHICS_MACROS
#define VX_GRAPHICS_MACROS

#define StoreAndAssertVkResult(resultStorage, vkFunction, ...) \
    {\
        auto vkresult = vkFunction(__VA_ARGS__); \
        resultStorage.vkFunction##Result = vkresult;\
        if (vkresult != VK_SUCCESS) \
        { \
            return vkresult; \
        } \
    }

#define StoreAndAssertVkResultP(resultStorage, vkFunction, ...) \
    {\
        auto vkresult = vkFunction(__VA_ARGS__); \
        resultStorage->vkFunction##Result = vkresult;\
        if (vkresult != VK_SUCCESS) \
        { \
            return vkresult; \
        } \
    }

#define AssertVkResult(vkFunction, ...) \
    {\
        auto vkresult = vkFunction(__VA_ARGS__); \
        if (vkresult != VK_SUCCESS) \
        { \
            return vkresult; \
        } \
    }

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

#define CopyResultItems(storage, storageProperty, itemType, items, itemCount) \
    {\
        storage = std::vector<itemType>(itemCount); \
        for (uint32_t i=0; i<itemCount; i++) \
        { \
            init##itemType(storage[i]); \
            storage[i].storageProperty = items[i]; \
        } \
    }

#define AppendResultItems(storage, storageProperty, itemType, items, itemCount) \
    {\
        for (uint32_t i=0; i<itemCount; i++) \
        { \
            itemType storageItem; \
            init##itemType(storageItem); \
            storageItem.storageProperty = items[i]; \
            storage.push_back(storageItem); \
        } \
    }

#endif
