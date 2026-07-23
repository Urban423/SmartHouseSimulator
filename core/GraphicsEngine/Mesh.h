#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "GraphicsEngine.h"

struct Vertex {
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;
	Vector2 uv1;
};

class Mesh {
public:
	Mesh() {};
	Mesh(int *index, int index_size, Vertex *vertex, int vertex_size, unsigned int *materials, unsigned int number_of_materials);
	Mesh(Mesh&& mesh) noexcept;
	Mesh &operator=(Mesh &&mesh) noexcept;
	~Mesh();

	void syncWithGPU();
	void setMeshOnPipeline();
	int getNumberOfMaterials() { return materials.size(); }
	unsigned int getMaterialSize(int id) { 
		if(id >= materials.size()) return 0;
		return materials[id]; 
	};
	Vector2 calculateAndRebuildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing);
private:
	void buildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing, Vector2 offset);
public:
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> materials;

    VertexArrayObject* vao = nullptr;
    IndexArrayObject* iao = nullptr;
};