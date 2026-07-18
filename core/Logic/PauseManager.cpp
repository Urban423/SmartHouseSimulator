#include "PauseManager.h"
#include "prefabs.h"

bool PauseManager::isLocalPaused = true;

void PauseManager::Create() {
    Object uiLayout = ECS::createObject();
    uiLayout.transform.position = Vector2(-0.6f, 3.0f);
    uiLayout.AddComponent<UIImage>().color = Color(1, 1, 250, 12);
    uiLayout.GetComponent<UIImage>().setSize(1980, 1080);
    uiLayout.GetComponent<UIImage>().direction = Direction::Horizontal;
    uiLayout.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
    uiLayout.GetComponent<UIImage>().alignY = UIAlignFlags::Start;

    Object pause = ECS::createObject();
    pause.AddComponent<Active>().enabled = true;
    pause.AddComponent<PauseElement>().activityToPause = true;
    pause.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Fill);
    pause.GetComponent<UIImage>().direction = Direction::Vertical;
    pause.GetComponent<UIImage>().alignX = UIAlignFlags::Center;
    pause.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
    pause.GetComponent<UIImage>().overflow = true;
    pause.GetComponent<UIImage>().padding = 32;
    pause.GetComponent<UIImage>().color = Color(0, 0, 0, 8);
    pause.setParent(uiLayout);

    Object text = ECS::createObject();
    text.AddComponent<UIText>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
    text.GetComponent<UIText>().text = "PAUSE";
    text.GetComponent<UIText>().color = 255;
    text.GetComponent<UIText>().fontSize = 64;
    text.GetComponent<UIText>().buildMesh();
    text.setParent(pause);

    Object settings = ECS::createObject();
    settings.AddComponent<Active>().enabled = true;
    settings.AddComponent<PauseElement>().activityToPause = false;

    int settingsPanelID = settings.getID();
    std::vector<std::pair<const char*, std::function<void()>>> buttons = {
        { "Resume", [] { PauseManager::SetPaused(false); }},

        { "Settings", [settingsPanelID] {
            auto& active = ECS::GetComponent<Active>(settingsPanelID);
            active.enabled = !active.enabled;
        }},
        { "Exit", [] { IOSystem::getWindow().onDestroy(); } },
    };
    for(int i = 0; i < buttons.size(); i++) {
        Object button = PrefabSystem::getInstance().createButton(buttons[i].first, buttons[i].second);\
        button.GetComponent<UIImage>().padding = 32;
        button.setParent(pause);
    }

    //settings
    settings.AddComponent<UIImage>().setSize(UISizeFlags::Fill, UISizeFlags::Fill);
    settings.GetComponent<UIImage>().direction = Direction::Vertical;
    settings.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
    settings.GetComponent<UIImage>().alignY = UIAlignFlags::Start;
    settings.GetComponent<UIImage>().overflow = true;
    settings.GetComponent<UIImage>().padding = 32;
    settings.GetComponent<UIImage>().color = Color(0, 0, 0, 6);
    settings.setParent(uiLayout);

    Object settingsText = ECS::createObject();
    settingsText.AddComponent<UIText>().text = "SETTINGS";
    settingsText.GetComponent<UIText>().fontSize = 64;
    settingsText.GetComponent<UIText>().buildMesh();
    settingsText.setParent(settings);

    PrefabSystem::getInstance().createInputField("Port", "7777", 4, [](char c) { 
        return std::isdigit(static_cast<unsigned char>(c)); 
    }).setParent(settings);


    PrefabSystem::getInstance().createSlider("SensitivityX", SettingsSystem::GetSettings().sensitivityX, 
        [](float value) {
            SettingsSystem::GetSettings().sensitivityX = value;
            SettingsSystem::Save();
    }).setParent(settings);


    PrefabSystem::getInstance().createSlider("SensitivityY", SettingsSystem::GetSettings().sensitivityY, 
        [](float value) {
            SettingsSystem::GetSettings().sensitivityY = value;
            SettingsSystem::Save();
    }).setParent(settings);

    PrefabSystem::getInstance().createCheckbox("Fullscreen", SettingsSystem::GetSettings().fullscreen, 
        [](bool checked) {
            IOSystem::getWindow().setFullscreen(checked);
            SettingsSystem::GetSettings().fullscreen = checked;
            SettingsSystem::Save();
    }).setParent(settings);


    PrefabSystem::getInstance().createCheckbox("VSync", SettingsSystem::GetSettings().vsync, 
        [](bool checked) {
            IOSystem::getWindow().setVSync(checked);
            SettingsSystem::GetSettings().vsync = checked;
            SettingsSystem::Save();
    }).setParent(settings);

    auto resolutions = IOSystem::getSupportedResolutions();
    // printf("supported resolutions: %d\n", resolutions.size()); for(auto& res : resolutions) { printf("Supported resolution: %d x %d\n", res.first, res.second); }
    PrefabSystem::getInstance().createDropdown("Resolution", "111111111111", resolutions,  
        []() {
            printf("re\n");
            // SettingsSystem::Save();
            // auto [width, height] = IOSystem::getPlatform().getSupportedResolutions()[index];
            // IOSystem::getWindow().setSize(width, height);
    }).setParent(settings);


    UISystem::getInstance().Rebuild(uiLayout);
    return;
}