#include "commonHeaders.hpp"
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

VxDebug VxDebugInstance;

void vxInitDebugInstance(std::string debugFilePath)
{
    VxDebugInstance.debugFilePath = debugFilePath;
    VxDebugInstance.init();
}

void VxDebug::init()
{
    if (debugFilePath.length()>0)
    {
        auto debugFilePath1 = boost::filesystem::absolute(boost::filesystem::path(debugFilePath));
        boost::filesystem::create_directories(debugFilePath1.parent_path());

        std::fstream debugFileStream;
        debugFileStream.open(debugFilePath,  std::fstream::in | std::fstream::out | std::fstream::trunc);
        if(!debugFileStream.fail())
        {
            debugFileStream << "\n";
            debugFileStream.close();
        }
    }
}

void VxDebug::log(const char* message, const char* category, const char* logType, const char* functionName, const char* fileName, const int line, ...)
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

    if (debugFilePath.length()>0)
    {
        std::fstream debugFileStream;
        debugFileStream.open(debugFilePath, std::fstream::in | std::fstream::out | std::fstream::app);
        if(debugFileStream.fail())
        {
            debugFileStream.open(debugFilePath,  std::fstream::in | std::fstream::out | std::fstream::trunc);
        }
        if(!debugFileStream.fail())
        {
            debugFileStream << text << "\n";
            debugFileStream.close();
        }
    }

    #ifdef WIN32
        OutputDebugStringA(text);
    #endif
}
