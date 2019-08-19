#ifndef VX_APPLICATION_INSTANCE_HEADER
#define VX_APPLICATION_INSTANCE_HEADER

typedef struct VxApplicationInstanceCreateInfo
{    
    ~VxApplicationInstanceCreateInfo();
    void destroy();
    void init()
    {
    }
} VxApplicationInstanceCreateInfo;

typedef struct VxApplicationInstance
{    
    ~VxApplicationInstance();
    void destroy();
    void init()
    {
    }

    void test();
} VxApplicationInstance;

#endif