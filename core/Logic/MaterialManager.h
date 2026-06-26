#pragma once
#include "Transform.h"
#include <vector>

class MaterialManager
{
public:
    inline static short CreateMaterial(const Material &mat)
    {
        materials.push_back(mat);
        return (short)(materials.size() - 1);
    }

    inline static Material &Get(short id)
    {
        return materials[id];
    }

private:
    inline static std::vector<Material> materials;
};