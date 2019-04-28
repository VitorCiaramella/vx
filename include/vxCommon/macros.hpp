#ifndef VX_COMMON_MACROS
#define VX_COMMON_MACROS

#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define VX_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))

#define VX_CURRENT_VERSION VX_MAKE_VERSION(0,1,0)

//pointer types
#define rpt(objectType) objectType*
#define upt(objectType) std::unique_ptr<objectType>
#define wpt(objectType) std::weak_ptr<objectType>
#define spt(objectType) std::shared_ptr<objectType>
//new pointers
#define nrp(objectType,...) new objectType(__VA_ARGS__)
#define nup(objectType,...) std::make_unique<objectType>(__VA_ARGS__)
#define nwp(objectType,...) std::weak_ptr<objectType>(__VA_ARGS__)
//#define nsp(objectType,...) std::make_shared<objectType>(__VA_ARGS__)

template <typename objectType>
std::shared_ptr<objectType> nsp() 
{
    auto obj = std::make_shared<objectType>();
    obj->init();
    return obj;
}

#define GetAndAssertSharedPointer(variable, wpObject) \
    if (wpObject.expired()) \
    { \
        vxLogWarning3("Trying to access expired pointer %s", "Memory", #wpObject); \
        return; \
    } \
    auto variable = wpObject.lock(); 

#define AssertTrue(check) \
    {\
        if (!(check)) \
        { \
            vxLogWarning2("Assert failed.","Assert"); \
            return; \
        } \
    }

//vectors
#define vector(object) std::vector<object>
#define vectorR(object) std::vector<rpt(object)>
#define vectorS(object) std::vector<spt(object)>
#define vectorU(object) std::vector<upt(object)>
#define vectorW(object) std::vector<wpt(object)>
#define nvectorU(object,...) std::vector<upt(object)>(__VA_ARGS__)
#define nvectorS(object,...) std::vector<spt(object)>(__VA_ARGS__)

#define AssertVxResult(vxFunction, ...) \
    {\
        auto vxresult = vxFunction(__VA_ARGS__); \
        if (vxresult != VxResult::VX_SUCCESS) \
        { \
            return vxresult; \
        } \
    }

#define arraySize(array) (sizeof(array) / sizeof((array)[0]))

#define strToNewCharArray(str) strdup(str.c_str())

#define CASE_ID_RETURN_NAME(x) case x: return #x

#endif
