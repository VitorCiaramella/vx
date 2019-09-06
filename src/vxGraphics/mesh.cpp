#include "commonHeaders.hpp"
#include "../../extern/meshoptimizer/src/meshoptimizer.h"
#include "../../extern/meshoptimizer/tools/fast_obj.h"
#include <string>

#define FAST_OBJ_IMPLEMENTATION
#include "../../extern/meshoptimizer/tools/fast_obj.h"

bool vxIsMeshValid(const VxMesh& mesh)
{
	size_t index_count = mesh.indices.size();
	size_t vertex_count = mesh.vertices.size();

	if (index_count % 3 != 0)
		return false;

	const unsigned int* indices = &mesh.indices[0];

	for (size_t i = 0; i < index_count; ++i)
		if (indices[i] >= vertex_count)
			return false;

	return true;
}

VxMesh vxLoadMesh(std::string filePath)
{
    auto path = filePath.c_str();

	fastObjMesh* obj = fast_obj_read(path);
	if (!obj)
	{
		printf("Error loading %s: file not found\n", path);
		return VxMesh();
	}

	size_t total_indices = 0;

	for (unsigned int i = 0; i < obj->face_count; ++i)
		total_indices += 3 * (obj->face_vertices[i] - 2);

	std::vector<VxVertex> vertices(total_indices);

	size_t vertex_offset = 0;
	size_t index_offset = 0;

	for (unsigned int i = 0; i < obj->face_count; ++i)
	{
		for (unsigned int j = 0; j < obj->face_vertices[i]; ++j)
		{
			fastObjIndex gi = obj->indices[index_offset + j];

			VxVertex v =
			    {
			        obj->positions[gi.p * 3 + 0],
			        obj->positions[gi.p * 3 + 1],
			        obj->positions[gi.p * 3 + 2],
			        obj->normals[gi.n * 3 + 0],
			        obj->normals[gi.n * 3 + 1],
			        obj->normals[gi.n * 3 + 2],
			        obj->texcoords[gi.t * 2 + 0],
			        obj->texcoords[gi.t * 2 + 1],
			    };

			// triangulate polygon on the fly; offset-3 is always the first polygon vertex
			if (j >= 3)
			{
				vertices[vertex_offset + 0] = vertices[vertex_offset - 3];
				vertices[vertex_offset + 1] = vertices[vertex_offset - 1];
				vertex_offset += 2;
			}

			vertices[vertex_offset] = v;
			vertex_offset++;
		}

		index_offset += obj->face_vertices[i];
	}

	fast_obj_destroy(obj);

	VxMesh mesh;

    /*
    Remove redundant vertices
    */
	std::vector<unsigned int> remap(total_indices);
	size_t total_vertices = meshopt_generateVertexRemap(&remap[0], NULL, total_indices, &vertices[0], total_indices, sizeof(VxVertex));
	mesh.indices.resize(total_indices);
	meshopt_remapIndexBuffer(&mesh.indices[0], NULL, total_indices, &remap[0]);
	mesh.vertices.resize(total_vertices);
	meshopt_remapVertexBuffer(&mesh.vertices[0], &vertices[0], total_indices, sizeof(VxVertex), &remap[0]);
    /*
    Reorder triangles maximizing locality 
    */
	meshopt_optimizeVertexCache(&mesh.indices[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size());
    /*
    Reorder triangles reducing overdraw (outer triangles gets priority over inner triangles)
    */
	const float kThreshold = 1.01f; // allow up to 1% worse Average Cache Miss Rate to get more reordering opportunities for overdraw
	meshopt_optimizeOverdraw(&mesh.indices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(VxVertex), kThreshold);
    /*
    Reorder the vertices in the vertex buffer to try to improve the locality of reference
    */
	meshopt_optimizeVertexFetch(&mesh.vertices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0], mesh.vertices.size(), sizeof(VxVertex));


    //auto isValid = vxIsMeshValid(mesh);

	return mesh;
}

