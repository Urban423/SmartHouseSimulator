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
    if (this != &mesh) {
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

void Mesh::syncWithGPU() { id = MeshManager::addMesh(*this); }

inline Vector2 calculateStringMeshSize(std::string& text, float glyphWidth, float glyphHeight, float letterSpacing, float lineSpacing, int& specialChars) {
    specialChars = 0;

    int lineCounter = 1;
    int glyphInRaw = 0;
    int maxGlyphInRaw = 0;
    for (auto letter : text) {
        if (letter == '\n') {
            maxGlyphInRaw = std::max(maxGlyphInRaw, glyphInRaw);
            ++specialChars;
            ++lineCounter;
            glyphInRaw = 0;
            continue;
        }
        ++glyphInRaw;
    }
    maxGlyphInRaw = std::max(maxGlyphInRaw, glyphInRaw);
    return { maxGlyphInRaw * glyphWidth + (letterSpacing * (maxGlyphInRaw - 1)), lineCounter * glyphHeight + (lineSpacing * (lineCounter - 1))};
}

void Mesh::buildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing, Vector2 offset) {
    float x = offset.x;
    float y = offset.y;
    int v = 0;
    int ind = 0;
    for (auto letter: text) {
        if (letter == '\n') {
            y += fontSize + lineSpacing;
            x = offset.x;
            continue;
        }

        int glyph = letter;
        std::string test = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~ ÇüéâäàÉæÆôöòåçêèèïîìÄAûùÿÖÜø£Ø×ƒ        Ë               ë       АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя";
        for (int j = 0; j < test.size(); j++) {
            if (letter == test[j]) {
                glyph = j;
                break;
            }
        }
        int cellX = glyph % 16;
        int cellY = glyph / 16;
        float u0 = cellX / 16.0f;
        float u1 = (cellX + 1) / 16.0f;
        float v0 = 1.0f - (cellY + 1) / 16.0f;
        float v1 = 1.0f - cellY / 16.0f;

        vertex[v + 0].pos = {x, -fontSize - y, 0};
        vertex[v + 1].pos = {x + fontSize, -fontSize - y, 0};
        vertex[v + 2].pos = {x + fontSize, -y, 0};
        vertex[v + 3].pos = {x, -y, 0};
        x += fontSize + letterSpacing;

        vertex[v + 0].uv = {u0, v0};
        vertex[v + 1].uv = {u1, v0};
        vertex[v + 2].uv = {u1, v1};
        vertex[v + 3].uv = {u0, v1};

        vertex[v + 0].normal = {0, 0, 1};
        vertex[v + 1].normal = {0, 0, 1};
        vertex[v + 2].normal = {0, 0, 1};
        vertex[v + 3].normal = {0, 0, 1};

        index[ind + 0] = v + 0;
        index[ind + 1] = v + 2;
        index[ind + 2] = v + 1;

        index[ind + 3] = v + 3;
        index[ind + 4] = v + 2;
        index[ind + 5] = v + 0;
        v += 4;
        ind += 6;
    }
}

Vector2 Mesh::calculateAndRebuildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing) {
    int specialChars = 0;
    int charCount = text.size();
    Vector2 textSize = calculateStringMeshSize(text, fontSize, fontSize, letterSpacing, lineSpacing, specialChars);

    vertex_size = (charCount - specialChars) * 4;
    vertex = new Vertex[vertex_size];
    index_size = (charCount - specialChars) * 6;
    index = new int[index_size];
    materials = new unsigned int[1];
    materials[0] = index_size;

    Vector2 offset = -textSize / 2;
    buildTextMesh(text, fontSize, letterSpacing, lineSpacing, offset);
    
    syncWithGPU();
    return textSize;
}