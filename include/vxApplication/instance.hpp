#ifndef VX_APPLICATION_INSTANCE_HEADER
#define VX_APPLICATION_INSTANCE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vxGraphics/vxGraphics.hpp>
#include <string>

struct VxApplicationInstanceCreateInfo;
struct VxApplicationInstance;

struct VxGraphicsInstance;
enum class VxWindowLoopResult;

typedef struct VxApplicationInstanceCreateInfo
{    
    void*           rpMainWindowHandle;
    std::string     resourcePath;

    ~VxApplicationInstanceCreateInfo();
    void destroy();
    void init()
    {
        rpMainWindowHandle = nullptr;
        resourcePath = "";
    }
} VxApplicationInstanceCreateInfo;

typedef struct VxApplicationInstance
{    
    spt(VxApplicationInstanceCreateInfo) spCreateInfo;
    spt(VxGraphicsInstance) spVxGraphicsInstance;
    
    VxWindowLoopResult draw();

    ~VxApplicationInstance();
    void destroy();
    void init()
    {
    }
} VxApplicationInstance;

VxResult vxCreateApplicationInstance(spt(VxApplicationInstanceCreateInfo) spCreateInfo, spt(VxApplicationInstance) & spVxApplicationInstance);

#endif