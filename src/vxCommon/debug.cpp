#include "commonHeaders.hpp"

void vxDebugLog(const char* message, const char* category, const char* logType, const char* functionName, const char* fileName, const int line, ...)
{
    char newMessage[4096];
    va_list args;
    va_start(args, line);
    vsnprintf(newMessage, arraySize(newMessage), message, args);
    va_end(args);

    char text[4096];
    if (category == nullptr)
    {
        snprintf(text, arraySize(text), "%s: %s\n  at %s\n  at %i:%s", logType, newMessage, functionName, line, fileName);
    }
    else
    {
        snprintf(text, arraySize(text), "%s: (%s) %s\n  at %s\n  at %i:%s", logType, category, newMessage, functionName, line, fileName);
    }
    
    printf("%s\n", text);

    #ifdef WIN32
        OutputDebugStringA(text);
    #endif
}
