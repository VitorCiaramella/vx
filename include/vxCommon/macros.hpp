#include <memory>

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
    (((major) < <  22) | ((minor) < <  12) | (patch))

#define VX_CURRENT_VERSION VX_MAKE_VERSION(0,1,0)

//pointer types
#define rpt(objectType) objectType*
#define upt(objectType) std::unique_ptr< objectType >
#define wpt(objectType) std::weak_ptr< objectType >
#define spt(objectType) std::shared_ptr< objectType >
//new pointers
#define nrp(objectType,...) new objectType(__VA_ARGS__)
#define nup(objectType,...) std::make_unique< objectType >(__VA_ARGS__)
#define nwp(objectType,...) std::weak_ptr< objectType >(__VA_ARGS__)
//#define nsp(objectType,...) std::make_shared< objectType >(__VA_ARGS__)

template < typename objectType, typename... Args >
std::shared_ptr< objectType > nsp(Args... args) 
{
    auto obj = std::make_shared< objectType >();
    obj->init(args...);
    return obj;
}

#define GetAndAssertSharedPointer(variable, wpObject) \
    if (wpObject.expired()) \
    { \
        vxLogWarning3("Trying to access expired pointer %s", "Memory", #wpObject); \
        return; \
    } \
    auto variable = wpObject.lock(); 

#define GetAndAssertSharedPointer2(variable, wpObject, returnValue) \
    if (wpObject.expired()) \
    { \
        vxLogWarning3("Trying to access expired pointer %s", "Memory", #wpObject); \
        return returnValue; \
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

#define AssertNotNull(object) \
    {\
        if (object == nullptr) \
        { \
            vxLogWarning3("Assert failed. %s is null.", "Memory", #object); \
            return; \
        } \
    }

#define AssertVxResult(vxFunction, ...) \
    {\
        auto vxresult = vxFunction(__VA_ARGS__); \
        if (vxresult != VxResult::VX_SUCCESS) \
        { \
            return vxresult; \
        } \
    }

//vectors
#define vector(value_type) std::vector< value_type >
#define vectorR(value_type) std::vector< rpt(value_type) >
#define vectorW(value_type) std::vector< std::weak_ptr< value_type > >
#define vectorS(value_type) vectorOfSharedPointers< value_type >

template < typename value_type >
struct vectorOfSharedPointers
{
    private:
        std::vector< std::shared_ptr< value_type > > storage;
    public:
        typedef typename std::vector< std::shared_ptr< value_type > >::iterator              iterator;
        typedef typename std::vector< const std::shared_ptr< value_type > >::iterator        const_iterator;
        typedef typename std::shared_ptr< value_type >                                     &reference;
        typedef typename std::shared_ptr< value_type >                              const  &const_reference;

        void push_back(const std::shared_ptr< value_type >& val)
        {
            storage.push_back(val);
        }
        void push_back(std::shared_ptr< value_type >&& val)
        {
            storage.push_back(val);
        }
        void clear() noexcept
        {
            for (auto && item : storage)
            {
                item->destroy();
            }
            storage.clear();
        }
        void reserve(size_t n)
        {            
            storage.reserve(n);
        }
        void resize(size_t n)
        {            
            storage.resize(n);
        }
        reference operator[] (size_t n)
        {
            return storage[n];
        }
        const_reference operator[] (size_t n) const
        {
            return storage[n];
        }
        iterator begin() noexcept
        {            
            return storage.begin();
        }
        const_iterator cbegin() const noexcept
        {
            return storage.cbegin();
        }
        iterator end() noexcept
        {
            return storage.end();
        }
        const_iterator cend() const noexcept
        {
            return storage.cend();
        }

};

#define arraySize(array) (sizeof(array) / sizeof((array)[0]))

#define strToNewCharArray(str) strdup(str.c_str())

#define CASE_ID_RETURN_NAME(x) case x: return #x

#endif
