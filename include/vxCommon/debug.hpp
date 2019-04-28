#ifndef VX_COMMON_DEBUG_HEADER
#define VX_COMMON_DEBUG_HEADER

#ifndef __FUNCTION_NAME__
    #ifdef WIN32   //WINDOWS
        #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
        #define __FUNCTION_NAME__   __func__ 
    #endif
#endif

typedef struct VxDebug 
{
    ~VxDebug();
    void init()
    {

    }
} VxDebug;

#ifdef _DEBUGINFO
    #define vxLogInfo(message) vxDebugLog(message, nullptr, "INFO", __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogInfo2(message, category) vxDebugLog(message, category, "INFO", __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogInfo3(message, category, ...) vxDebugLog(message, category, "INFO", __FUNCTION_NAME__, __FILE__, __LINE__, __VA_ARGS__)
#else
    #define vxLogInfo(message) 
    #define vxLogInfo2(message, category) 
    #define vxLogInfo3(message, category, ...) 
#endif

#ifdef _DEBUG
    #define vxLogDebug(message) vxDebugLog(message, nullptr, "DEBUG", __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogDebug2(message, category) vxDebugLog(message, category, "DEBUG", __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogDebug3(message, category, ...) vxDebugLog(message, category, "DEBUG", __FUNCTION_NAME__, __FILE__, __LINE__, __VA_ARGS__)
#else
    #define vxLogDebug(message) 
    #define vxLogDebug2(message, category) 
    #define vxLogDebug3(message, category, ...)
#endif

#ifdef _DEBUG
    #define vxLogWarning(message) vxDebugLog(message, nullptr, "WARNING", __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogWarning2(message, category) vxDebugLog(message, category, "WARNING", __FUNCTION_NAME__, __FILE__, __LINE__)
    #define vxLogWarning3(message, category, ...) vxDebugLog(message, category, "WARNING", __FUNCTION_NAME__, __FILE__, __LINE__, __VA_ARGS__)
#else
    #define vxLogWarning(message) 
    #define vxLogWarning2(message, category) 
    #define vxLogWarning3(message, category, ...)
#endif

#define vxLogError(message) vxDebugLog(message, nullptr, "ERROR", __FUNCTION_NAME__, __FILE__, __LINE__)
#define vxLogError2(message, category) vxDebugLog(message, category, "ERROR", __FUNCTION_NAME__, __FILE__, __LINE__)
#define vxLogError3(message, category, ...) vxDebugLog(message, category, "ERROR", __FUNCTION_NAME__, __FILE__, __LINE__, __VA_ARGS__)

void vxDebugLog(const char* message, const char* category, const char* logType, const char* functionName, const char* fileName, const int line, ...);

#endif
