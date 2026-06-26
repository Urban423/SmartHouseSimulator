#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include <string>

struct Vertex
{
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;
	Vector2 uv1;
};

class Mesh
{
public:
	Mesh() : index(nullptr), index_size(0), vertex(nullptr), vertex_size(0), materials(nullptr), number_of_materials(0) {}
	Mesh(int *index, int index_size,
		 Vertex *vertex, int vertex_size,
		 unsigned int *materials, unsigned int number_of_materials);
	Mesh(Mesh &&mesh)
		: index(mesh.index), index_size(mesh.index_size),
		  vertex(mesh.vertex), vertex_size(mesh.vertex_size),
		  materials(mesh.materials), number_of_materials(mesh.number_of_materials)
	{
		mesh.index = nullptr;
		mesh.vertex = nullptr;
		mesh.materials = nullptr;
	}
	~Mesh();

	void syncWithGPU();
	void rebuildTextMesh(std::string &text);

public:
	Mesh &operator=(Mesh &&mesh);

public:
	int *index;
	int index_size;

	Vertex *vertex;
	int vertex_size;

	unsigned int *materials;
	unsigned int number_of_materials;

	int id;
};