#ifndef VX_GRAPHICS_MESH_HEADER
#define VX_GRAPHICS_MESH_HEADER

#include <vxCommon/vxCommon.hpp>

/*
Based on https://github.com/zeux/meshoptimizer/blob/master/demo/main.cpp by Arseny Kapoulkine
*/

struct VxVertex
{
	float px, py, pz;
	float nx, ny, nz;
	float tx, ty;
};

struct VxMesh
{
	std::vector<VxVertex> vertices;
	std::vector<uint32_t> indices;
};

union VxTriangle {
	VxVertex v[3];
	char data[sizeof(VxVertex) * 3];
};

VxMesh vxLoadMesh(std::string filePath);

#endif