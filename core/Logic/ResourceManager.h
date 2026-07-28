#pragma once

#define TEXTURES \
    FIELD(box) \
    FIELD(Floortex) \
    FIELD(Grass) \
    FIELD(Tile1) \
    FIELD(Ghost) \
    FIELD(Ghost1) \


enum TextureID {
    TEX_White,
    TEX_Atlas,
#define FIELD(name) TEX_##name,
    TEXTURES
#undef FIELD
    TEX_COUNT
};



#define MESHES \
    FIELD(Box)

enum MeshID {
    MESH_Plane,
    MESH_Cube,
    MESH_Sphere,
    MESH_Cylinder,
    MESH_Capsule,
#define FIELD(name) MESH_##name,
    MESHES
#undef FIELD
    MESH_COUNT
};


#define SHADERS \
    FIELD(standartShader) \
    FIELD(mainPanel) \
    FIELD(shader3) \
    FIELD(terrainShader) \
    FIELD(shader5) \
    FIELD(textShader) \
    FIELD(uiShader)

enum ShaderID {
#define FIELD(name) SHADER_##name,
    SHADERS
#undef FIELD
    SHADER_COUNT
};