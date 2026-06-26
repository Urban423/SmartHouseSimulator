#include "Mesh.h"
#include "MeshManager.h"

Mesh::Mesh(int *index, int index_size,
           Vertex *vertex, int vertex_size,
           unsigned int *materials, unsigned int number_of_materials)
    : index(index), index_size(index_size),
      vertex(vertex), vertex_size(vertex_size),
      materials(materials), number_of_materials(number_of_materials)
{
    id = MeshManager::addMesh(*this);
}

Mesh::~Mesh() {
    // delete[] index;
    // delete[] vertex;
    // delete[] materials;
};

Mesh &Mesh::operator=(Mesh &&mesh)
{
    if (this != &mesh)
    {
        delete[] index;
        delete[] vertex;
        delete[] materials;

        index = mesh.index;
        index_size = mesh.index_size;
        vertex = mesh.vertex;
        vertex_size = mesh.vertex_size;
        materials = mesh.materials;
        number_of_materials = mesh.number_of_materials;

        mesh.index = nullptr;
        mesh.vertex = nullptr;
        mesh.materials = nullptr;
    }
    return *this;
}

void Mesh::syncWithGPU()
{
    id = MeshManager::addMesh(*this);
}

void Mesh::rebuildTextMesh(std::string &text)
{
    int specialChars = 0;
    const float size = 0.4f;
    const float spacing = 1.5f * size;
    const float linesSpacing =  1.8f * size;
    int charCount = text.size();
    vertex_size = charCount * 4;
    index_size = charCount * 6;

    materials = new unsigned int[1];
    vertex = new Vertex[vertex_size];
    index = new int[index_size];
    materials[0] = index_size;
    float y = 0;
    float x = 0;
    int v = 0;
    int ind = 0;
    for (int i = 0; i < charCount; i++) {
        if (text[i] == '\n') {
            ++specialChars;
            y += linesSpacing;
            x = 0;
            continue;
        }

        // quad positions
        vertex[v + 0].pos = {x - size, -size - y, 0};
        vertex[v + 1].pos = {x + size, -size - y, 0};
        vertex[v + 2].pos = {x + size, size - y, 0};
        vertex[v + 3].pos = {x - size, size - y, 0};
        x += spacing;

        // UV (0–1 inside glyph cell, atlas handled in shader)
        float h = 1;
        float w = 0;
        vertex[v + 0].uv = {w, w};
        vertex[v + 1].uv = {h, w};
        vertex[v + 2].uv = {h, h};
        vertex[v + 3].uv = {w, h};

        // glyph index
        int glyph = text[i];
        std::string test = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~ ÇüéâäàÉæÆôöòåçêèèïîìÄAûùÿÖÜø£Ø×ƒ        Ë               ë       АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя";
        for (int j = 0; j < test.size(); j++) {
            if (text[i] == test[j]) {
                glyph = j;
                break;
            }
        }
        for (int j = 0; j < 4; j++)
        {
            vertex[v + j].uv1 = Vector2((float)glyph, 0.0f);
            vertex[v + j].normal = {0, 0, 1};
        }

        // indices
        index[ind + 0] = v + 0;
        index[ind + 1] = v + 2;
        index[ind + 2] = v + 1;

        index[ind + 3] = v + 3;
        index[ind + 4] = v + 2;
        index[ind + 5] = v + 0;
        v += 4;
        ind += 6;
    }
    vertex_size -= specialChars * 4;
    index_size -= specialChars * 6;
    syncWithGPU();
}