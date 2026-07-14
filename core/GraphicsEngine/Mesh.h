#pragma once
#include "Vector2.h"
#include "Vector3.h"

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
	Mesh(Mesh&& mesh) : index(mesh.index),
		index_size(mesh.index_size),
		vertex(mesh.vertex),
		vertex_size(mesh.vertex_size),
		materials(mesh.materials),
		number_of_materials(mesh.number_of_materials),
		id(mesh.id)
	{
		mesh.index = nullptr;
		mesh.vertex = nullptr;
		mesh.materials = nullptr;
		mesh.id = -1;
	}
	~Mesh();

	void syncWithGPU();
	Vector2 calculateAndRebuildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing);
private:
	void buildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing, Vector2 offset);
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