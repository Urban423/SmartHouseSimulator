#include "UIManager.h"

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

void buildTextMesh(Mesh& out, std::string& text, float fontSize, float letterSpacing, float lineSpacing, Vector2 offset) {
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

        out.vertices[v + 0].pos = {x, -fontSize - y, 0};
        out.vertices[v + 1].pos = {x + fontSize, -fontSize - y, 0};
        out.vertices[v + 2].pos = {x + fontSize, -y, 0};
        out.vertices[v + 3].pos = {x, -y, 0};
        x += fontSize + letterSpacing;

        out.vertices[v + 0].uv = {u0, v0};
        out.vertices[v + 1].uv = {u1, v0};
        out.vertices[v + 2].uv = {u1, v1};
        out.vertices[v + 3].uv = {u0, v1};

        out.vertices[v + 0].normal = {0, 0, 1};
        out.vertices[v + 1].normal = {0, 0, 1};
        out.vertices[v + 2].normal = {0, 0, 1};
        out.vertices[v + 3].normal = {0, 0, 1};

        out.indices[ind + 0] = v + 0;
        out.indices[ind + 1] = v + 2;
        out.indices[ind + 2] = v + 1;

        out.indices[ind + 3] = v + 3;
        out.indices[ind + 4] = v + 2;
        out.indices[ind + 5] = v + 0;
        v += 4;
        ind += 6;
    }
}

Vector2 calculateAndRebuildTextMesh(Mesh& out, std::string& text, float fontSize, float letterSpacing, float lineSpacing, UIAlignFlags align) {
    int specialChars = 0;
    int charCount = text.size();
    Vector2 textSize = calculateStringMeshSize(text, fontSize, fontSize, letterSpacing, lineSpacing, specialChars);

    int indicesSize = (charCount - specialChars) * 6;
    out.vertices.resize((charCount - specialChars) * 4);
    out.indices.resize(indicesSize);
    out.materials.resize(1);
    out.materials[0] = indicesSize;

    Vector2 offset = -textSize / 2;
    buildTextMesh(out, text, fontSize, letterSpacing, lineSpacing, offset);
    out.syncWithGPU();
    return textSize;
}

size_t getMeshCursorIndex(Mesh& mesh, Vector2 mousePos) {
    float bestDist2 = FLT_MAX;
    size_t bestVertex = SIZE_MAX;
    Vector2 bestCenter;

    for (size_t v = 0; v < mesh.vertices.size(); v += 4) {
        float left   = mesh.vertices[v + 0].pos.x;
        float right  = mesh.vertices[v + 1].pos.x;
        float bottom = mesh.vertices[v + 0].pos.y;
        float top    = mesh.vertices[v + 2].pos.y;

        Vector2 center = { (left + right) * 0.5f, (bottom + top) * 0.5f };

        float dx = mousePos.x - center.x;
        float dy = mousePos.y - center.y;
        float dist2 = dx * dx + dy * dy;

        if (dist2 < bestDist2) {
            bestDist2 = dist2;
            bestVertex = v;
            bestCenter = center;
        }
    }
    if(mousePos.x > bestCenter.x) {
        return bestVertex / 4 + 1;
    }
    return bestVertex / 4;
}

Vector2 getMeshCursorPos(Mesh& mesh, size_t cursorIndex) {
    size_t topLeftVertexIndex = cursorIndex * 4;
    if(topLeftVertexIndex >= mesh.vertices.size()) {
        float right  = mesh.vertices[mesh.vertices.size() - 2].pos.x;
        float bottom = mesh.vertices[mesh.vertices.size() - 2].pos.y;
        float top    = mesh.vertices[mesh.vertices.size() - 3].pos.y;
        return {right, (bottom + top) * 0.5f};
    }

    float left   = mesh.vertices[topLeftVertexIndex + 0].pos.x;
    float bottom = mesh.vertices[topLeftVertexIndex + 0].pos.y;
    float top    = mesh.vertices[topLeftVertexIndex + 2].pos.y;
    float y = (bottom + top) * 0.5f;

    if(topLeftVertexIndex == 0) {
        return {left, y};
    }

    float right = mesh.vertices[topLeftVertexIndex - 2].pos.x;
    return {(left + right) * 0.5f, y};
}