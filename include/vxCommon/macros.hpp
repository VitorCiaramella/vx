#ifndef VX_COMMON_MACROS
#define VX_COMMON_MACROS

#define VX_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))

#define VX_CURRENT_VERSION VX_MAKE_VERSION(0,1,0)

//pointer types
#define rpt(object) object*
#define upt(object) std::unique_ptr<object>
#define wpt(object) std::weak_ptr<object>
#define spt(object) std::shared_ptr<object>
//new pointers
#define nrp(object,...) new object(__VA_ARGS__)
#define nup(object,...) std::make_unique<object>(__VA_ARGS__)
//#define vxWP(object) std::weak_ptr<object>
#define nsp(object,...) std::make_shared<object>(__VA_ARGS__)

//vectors
#define vectorR(object) std::vector<rpt(object)>
#define vectorU(object) std::vector<upt(object)>
#define nvectorU(object,...) std::vector<upt(object)>(__VA_ARGS__)
#define vectorS(object) std::vector<spt(object)>
#define nvectorS(object,...) std::vector<spt(object)>(__VA_ARGS__)

#endif
