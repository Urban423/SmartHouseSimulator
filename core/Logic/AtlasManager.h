#pragma once
#include "IOSystem.h"

class AtlasManager {
public:
    inline static AtlasManager& getInstance() { static AtlasManager atlasManager; return atlasManager; }
    inline static TTFAtlas& GetAtlas() { return getInstance().atlas; }
    inline static void SetAtlas(TTFAtlas& atlas) { getInstance().atlas = atlas; }
private:
    TTFAtlas atlas;
};