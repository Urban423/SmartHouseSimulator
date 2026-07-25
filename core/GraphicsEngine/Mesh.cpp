#include "Mesh.h"
#include "MeshManager.h"

Mesh::Mesh(int* index, int index_size, Vertex* vertex, int vertex_size, unsigned int* materials, unsigned int number_of_materials) {
    indices.assign(index, index + index_size);
    vertices.assign(vertex, vertex + vertex_size);
    this->materials.assign(materials, materials + number_of_materials); 
    syncWithGPU();
}

Mesh::Mesh(Mesh&& mesh) noexcept : indices(std::move(mesh.indices)), vertices(std::move(mesh.vertices)), materials(std::move(mesh.materials)), vao(mesh.vao), iao(mesh.iao) {
    mesh.vao = nullptr;
    mesh.iao = nullptr;
}

Mesh::~Mesh() {
    delete vao;
    delete iao;
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept {
    if (this != &mesh) {
        delete vao;
        delete iao;

        indices = std::move(mesh.indices);
        vertices = std::move(mesh.vertices);
        materials = std::move(mesh.materials);

        vao = mesh.vao;
        iao = mesh.iao;

        mesh.vao = nullptr;
        mesh.iao = nullptr;
    }
    return *this;
}

void Mesh::syncWithGPU() { 
    if (vao) {
        vao->destroy();
        delete vao;
        vao = nullptr;
    }
    if (iao) {
        iao->destroy();
        delete iao;
        iao = nullptr;
    }
    
    if (!vao) {
        vao = GraphicsEngine::createVertexArrayObject({
            vertices.data(),
            sizeof(Vertex),
            static_cast<unsigned int>(vertices.size())
        });
    }

    if (!iao) {
        iao = GraphicsEngine::createIndexArrayObject({
            reinterpret_cast<unsigned int*>(indices.data()),
            static_cast<unsigned int>(indices.size())
        });
    }
}
	
void Mesh::setMeshOnPipeline() {
    GraphicsEngine::setVertexArrayObject(vao);
    GraphicsEngine::setIndexArrayObject(iao);
}

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

        vertices[v + 0].pos = {x, -fontSize - y, 0};
        vertices[v + 1].pos = {x + fontSize, -fontSize - y, 0};
        vertices[v + 2].pos = {x + fontSize, -y, 0};
        vertices[v + 3].pos = {x, -y, 0};
        x += fontSize + letterSpacing;

        vertices[v + 0].uv = {u0, v0};
        vertices[v + 1].uv = {u1, v0};
        vertices[v + 2].uv = {u1, v1};
        vertices[v + 3].uv = {u0, v1};

        vertices[v + 0].normal = {0, 0, 1};
        vertices[v + 1].normal = {0, 0, 1};
        vertices[v + 2].normal = {0, 0, 1};
        vertices[v + 3].normal = {0, 0, 1};

        indices[ind + 0] = v + 0;
        indices[ind + 1] = v + 2;
        indices[ind + 2] = v + 1;

        indices[ind + 3] = v + 3;
        indices[ind + 4] = v + 2;
        indices[ind + 5] = v + 0;
        v += 4;
        ind += 6;
    }
}

Vector2 Mesh::calculateAndRebuildTextMesh(std::string& text, float fontSize, float letterSpacing, float lineSpacing) {
    int specialChars = 0;
    int charCount = text.size();
    Vector2 textSize = calculateStringMeshSize(text, fontSize, fontSize, letterSpacing, lineSpacing, specialChars);

    int indicesSize = (charCount - specialChars) * 6;
    vertices.resize((charCount - specialChars) * 4);
    indices.resize(indicesSize);
    materials.resize(1);
    materials[0] = indicesSize;

    Vector2 offset = -textSize / 2;
    buildTextMesh(text, fontSize, letterSpacing, lineSpacing, offset);
    syncWithGPU();
    return textSize;
}