#include "Mesh.h"
#include "MeshManager.h"

Mesh CreatePlane() {
    Mesh mesh;
    mesh.vertices = {
        {{-0.5f, -0.5f, 0.0f}, {0, 0, 1}, {0, 0}, {0, 0}},
        {{ 0.5f, -0.5f, 0.0f}, {0, 0, 1}, {1, 0}, {1, 0}},
        {{-0.5f,  0.5f, 0.0f}, {0, 0, 1}, {0, 1}, {0, 1}},
        {{ 0.5f,  0.5f, 0.0f}, {0, 0, 1}, {1, 1}, {1, 1}},
    };

    mesh.indices = {
        0, 2, 1,
        1, 2, 3
    };

    mesh.materials = { 6 };
    mesh.syncWithGPU();
    return mesh;
}


Mesh CreateCube() {
    Mesh mesh;
    mesh.vertices = {
        // Front
        {{-0.5f, -0.5f,  0.5f}, { 0,  0,  1}, {0, 0}, {0, 0}},
        {{ 0.5f, -0.5f,  0.5f}, { 0,  0,  1}, {1, 0}, {1, 0}},
        {{-0.5f,  0.5f,  0.5f}, { 0,  0,  1}, {0, 1}, {0, 1}},
        {{ 0.5f,  0.5f,  0.5f}, { 0,  0,  1}, {1, 1}, {1, 1}},

        // Back
        {{ 0.5f, -0.5f, -0.5f}, { 0,  0, -1}, {0, 0}, {0, 0}},
        {{-0.5f, -0.5f, -0.5f}, { 0,  0, -1}, {1, 0}, {1, 0}},
        {{ 0.5f,  0.5f, -0.5f}, { 0,  0, -1}, {0, 1}, {0, 1}},
        {{-0.5f,  0.5f, -0.5f}, { 0,  0, -1}, {1, 1}, {1, 1}},

        // Left
        {{-0.5f, -0.5f, -0.5f}, {-1,  0,  0}, {0, 0}, {0, 0}},
        {{-0.5f, -0.5f,  0.5f}, {-1,  0,  0}, {1, 0}, {1, 0}},
        {{-0.5f,  0.5f, -0.5f}, {-1,  0,  0}, {0, 1}, {0, 1}},
        {{-0.5f,  0.5f,  0.5f}, {-1,  0,  0}, {1, 1}, {1, 1}},

        // Right
        {{ 0.5f, -0.5f,  0.5f}, { 1,  0,  0}, {0, 0}, {0, 0}},
        {{ 0.5f, -0.5f, -0.5f}, { 1,  0,  0}, {1, 0}, {1, 0}},
        {{ 0.5f,  0.5f,  0.5f}, { 1,  0,  0}, {0, 1}, {0, 1}},
        {{ 0.5f,  0.5f, -0.5f}, { 1,  0,  0}, {1, 1}, {1, 1}},

        // Top
        {{-0.5f,  0.5f,  0.5f}, { 0,  1,  0}, {0, 0}, {0, 0}},
        {{ 0.5f,  0.5f,  0.5f}, { 0,  1,  0}, {1, 0}, {1, 0}},
        {{-0.5f,  0.5f, -0.5f}, { 0,  1,  0}, {0, 1}, {0, 1}},
        {{ 0.5f,  0.5f, -0.5f}, { 0,  1,  0}, {1, 1}, {1, 1}},

        // Bottom
        {{-0.5f, -0.5f, -0.5f}, { 0, -1,  0}, {0, 0}, {0, 0}},
        {{ 0.5f, -0.5f, -0.5f}, { 0, -1,  0}, {1, 0}, {1, 0}},
        {{-0.5f, -0.5f,  0.5f}, { 0, -1,  0}, {0, 1}, {0, 1}},
        {{ 0.5f, -0.5f,  0.5f}, { 0, -1,  0}, {1, 1}, {1, 1}},
    };

    mesh.indices = {
         0,  2,  1,   1,  2,  3,   // Front
         4,  6,  5,   5,  6,  7,   // Back
         8, 10,  9,   9, 10, 11,   // Left
        12, 14, 13,  13, 14, 15,   // Right
        16, 18, 17,  17, 18, 19,   // Top
        20, 22, 21,  21, 22, 23    // Bottom
    };

    mesh.materials = { 36 };
    mesh.syncWithGPU();
    return mesh;
}

Mesh CreateSphere(float radius, uint32_t stacks, uint32_t slices) {
    return CreateCapsule(0, radius, stacks, slices);
}


Mesh CreateCylinder(uint32_t slices) {
    Mesh mesh;
    const float radius = 0.5f;
    const float halfHeight = 0.5f;
    for (uint32_t y = 0; y <= 1; ++y) {
        float v = (float)y;
        float py = y ? halfHeight : -halfHeight;

        for (uint32_t x = 0; x <= slices; ++x) {
            float u = (float)x / slices;
            float theta = u * 2.0f * Math::PI;

            float c = cosf(theta);
            float s = sinf(theta);

            Vector3 pos(radius * c, py, radius * s);
            Vector3 normal(c, 0.0f, s);

            mesh.vertices.push_back({ pos, normal.normalized(), {u, v}, {u, v} });
        }
    }
    for (uint32_t x = 0; x < slices; ++x) {
        uint32_t i0 = x;
        uint32_t i1 = x + 1;
        uint32_t i2 = x + slices + 1;
        uint32_t i3 = i2 + 1;

        mesh.indices.insert(mesh.indices.end(), {
            i0, i1, i2,
            i1, i3, i2
        });
    }


    uint32_t topCenter = (uint32_t)mesh.vertices.size();
    mesh.vertices.push_back({ {0.0f, halfHeight, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}, {0.5f, 0.5f}});
    for (uint32_t x = 0; x <= slices; ++x) {
        float u = (float)x / slices;
        float theta = u * 2.0f * Math::PI;

        float c = cosf(theta);
        float s = sinf(theta);

        mesh.vertices.push_back({ {radius * c, halfHeight, radius * s}, {0.0f, 1.0f, 0.0f}, {(c + 1.0f) * 0.5f, (s + 1.0f) * 0.5f}, {(c + 1.0f) * 0.5f, (s + 1.0f) * 0.5f} });
    }

    for (uint32_t x = 0; x < slices; ++x) {
        mesh.indices.insert(mesh.indices.end(), {
            topCenter,
            topCenter + x + 1,
            topCenter + x + 2
        });
    }


    uint32_t bottomCenter = (uint32_t)mesh.vertices.size();
    mesh.vertices.push_back({ {0.0f, -halfHeight, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}, {0.5f, 0.5f} });
    for (uint32_t x = 0; x <= slices; ++x) {
        float u = (float)x / slices;
        float theta = u * 2.0f * Math::PI;

        float c = cosf(theta);
        float s = sinf(theta);

        mesh.vertices.push_back({ {radius * c, -halfHeight, radius * s}, {0.0f, -1.0f, 0.0f}, {(c + 1.0f) * 0.5f, (s + 1.0f) * 0.5f}, {(c + 1.0f) * 0.5f, (s + 1.0f) * 0.5f} });
    }

    for (uint32_t x = 0; x < slices; ++x) {
        mesh.indices.insert(mesh.indices.end(), {
            bottomCenter,
            bottomCenter + x + 2,
            bottomCenter + x + 1
        });
    }

    mesh.materials = { (uint32_t)mesh.indices.size() };
    mesh.syncWithGPU();
    return mesh;
}


Mesh CreateCapsule(float height, float radius, uint32_t stacks, uint32_t slices) {
    Mesh mesh;
    const float halfCylinder = height * 0.5f;
    for (uint32_t y = 0; y <= stacks; ++y) {
        float v = (float)y / stacks;
        float phi = v * (Math::PI * 0.5f);
        float yPos = halfCylinder + cosf(phi) * radius;
        float ringRadius = sinf(phi) * radius;
        for (uint32_t x = 0; x <= slices; ++x) {
            float u = (float)x / slices;
            float theta = u * 2.0f * Math::PI;

            float c = cosf(theta);
            float s = sinf(theta);

            Vector3 normal(sinf(phi) * c, cosf(phi), sinf(phi) * s);
            Vector3 pos(ringRadius * c, yPos, ringRadius * s);
            mesh.vertices.push_back({ pos, normal.normalized(), {u, v * 0.25f}, {u, v * 0.25f} });
        }
    }

    for (uint32_t y = 0; y <= stacks; ++y) {
        float v = (float)y / stacks;
        float phi = Math::PI * 0.5f + v * (Math::PI * 0.5f);
        float yPos = -halfCylinder + cosf(phi) * radius;
        float ringRadius = sinf(phi) * radius;
        for (uint32_t x = 0; x <= slices; ++x) {
            float u = (float)x / slices;
            float theta = u * 2.0f * Math::PI;

            float c = cosf(theta);
            float s = sinf(theta);

            Vector3 normal(sinf(phi) * c, cosf(phi), sinf(phi) * s);
            Vector3 pos(ringRadius * c, yPos, ringRadius * s);
            mesh.vertices.push_back({ pos, normal.normalized(), {u, 0.75f + v * 0.25f}, {u, 0.75f + v * 0.25f} });
        }
    }


    uint32_t rowSize = slices + 1;
    uint32_t rows = (stacks + 1) + 2 + (stacks + 1);
    for (uint32_t y = 0; y < rows - 1; ++y) {
        for (uint32_t x = 0; x < slices; ++x) {
            uint32_t i0 = y * rowSize + x;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = i0 + rowSize;
            uint32_t i3 = i2 + 1;

            mesh.indices.insert(mesh.indices.end(), {
                i0, i2, i1,
                i1, i2, i3
            });
        }
    }

    mesh.materials = { (uint32_t)mesh.indices.size() };
    mesh.syncWithGPU();
    return mesh;
}