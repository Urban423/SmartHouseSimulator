#include "Settings.h"

void SettingsSystem::Save() {
    const Settings& s = getInstace().settings;

    std::string buffer;
    #define FIELD(type, name, value) \
        buffer += #name "=";          \
        buffer += std::to_string(s.name); \
        buffer += "\n";
    SETTINGS
    #undef FIELD

    CFile file = createCFile();
    writeCFile(buffer.data(), buffer.size(), file);
    saveCFile("settings.cfg", file);
}



const char* FindValue(const char* text, const char* key) {
    size_t keyLen = strlen(key);
    while (*text) {
        while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n') ++text;
        if (strncmp(text, key, keyLen) == 0) {
            text += keyLen;
            while (*text == ' ' || *text == '\t') ++text;
            if (*text != '=') continue;
            ++text;
            while (*text == ' ' || *text == '\t') ++text;
            return text;
        }
        while (*text && *text != '\n') ++text;
    }
    return nullptr;
}

Settings& SettingsSystem::Load() {
    CFile file = openCFile("settings.cfg");
    if (file.isEmpty()) return getInstace().settings;

    const char* text = file.getPtr();
    Settings& s = getInstace().settings;

    #define FIELD(type, name, value) if (const char* v = FindValue(text, #name)) s.name = (type)atof(v);
        SETTINGS
    #undef FIELD

    return s;
}