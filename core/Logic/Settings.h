#pragma once
#include "Transform.h"
#include "IOSystem.h"

#define SETTINGS \
    FIELD(float, sensitivityX, 0.5f) \
    FIELD(float, sensitivityY, 0.5f) \
    FIELD(WindowMode, winMode, WindowMode::Windowmode) \
    FIELD(bool, vsync, false) \


#define FIELD(type, name, value) type name = value;
struct Settings {
    SETTINGS
};
#undef FIELD

class SettingsSystem {
public:
    inline static Settings& GetSettings() { return getInstace().settings; }

    static void Save();
    static Settings& Load();
private:
    inline static SettingsSystem& getInstace() { static SettingsSystem settingsSystem; return settingsSystem; }
private:
    Settings settings;
};