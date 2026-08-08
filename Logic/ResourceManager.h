#pragma once

#define TEXTURES \
    FIELD(box) \
    FIELD(ball) \
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
    FIELD(Box) \
    FIELD(Untitled) \

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
    FIELD(terrainShader) \
    FIELD(textShader) \
    FIELD(uiShader) \
    FIELD(voxelShader) \



enum ShaderID {
#define FIELD(name) SHADER_##name,
    SHADERS
#undef FIELD
    SHADER_COUNT
};