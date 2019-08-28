#ifndef VX_APPLICATION_INSTANCE_HEADER
#define VX_APPLICATION_INSTANCE_HEADER

#include <vxCommon/vxCommon.hpp>
#include <vxGraphics/vxGraphics.hpp>

typedef struct VxApplicationInstanceCreateInfo
{    
    void*     rpMainWindowHandle;

    ~VxApplicationInstanceCreateInfo();
    void destroy();
    void init()
    {
        rpMainWindowHandle = nullptr;
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