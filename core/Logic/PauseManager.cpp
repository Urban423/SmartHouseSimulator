#include "PauseManager.h"
#include "prefabs.h"

bool PauseManager::isLocalPaused = true;

void PauseManager::Create() {
    Object uiLayout = ECS::createObject();
    uiLayout.transform.position = Vector2(-0.6f, 3.0f);
    uiLayout.AddComponent<UIImage>().color = Color(1, 1, 250, 12);
    uiLayout.GetComponent<UIImage>().setSize(1980, 1080);
    uiLayout.GetComponent<UIImage>().direction = Direction::Horizontal;

    Object pause = ECS::createObject();
    pause.AddComponent<Active>().enabled = true;
    pause.AddComponent<PauseElement>().activityToPause = true;
    pause.AddComponent<UIImage>().setSize(UIElement::Wrap, UIElement::Fill);
    pause.GetComponent<UIImage>().anchor = Anchor::Left;
    pause.GetComponent<UIImage>().pivot = Anchor::Left;
    pause.GetComponent<UIImage>().align = Align::Center;
    pause.GetComponent<UIImage>().direction = Direction::Vertical;
    pause.GetComponent<UIImage>().overflow = false;
    pause.GetComponent<UIImage>().padding = 32;
    pause.GetComponent<UIImage>().color = Color(0, 0, 0, 9);
    pause.setParent(uiLayout);

    Object button1 = ECS::createObject();
    button1.AddComponent<UIImage>().setSize(-(static_cast<int>(-UIElement::Wrap) | static_cast<int>(-UIElement::Fill)), UIElement::Wrap);
    button1.GetComponent<UIImage>().align = Align::Center;
    button1.GetComponent<UIImage>().padding = 12;
    button1.GetComponent<UIImage>().color = Color(255, 0, 255, 0);
    button1.setParent(pause);

    Object text = ECS::createObject();
    text.AddComponent<UIText>().setSize(UIElement::Wrap, UIElement::Wrap);
    text.GetComponent<UIText>().text = "PAUSE";
    text.GetComponent<UIText>().color = 255;
    text.GetComponent<UIText>().fontSize = 64;
    text.GetComponent<UIText>().buildMesh();
    text.setParent(button1);

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
        Object button3 = ECS::createObject();
        button3.AddComponent<Button>().onClickDown = buttons[i].second;
        button3.AddComponent<UIImage>().setSize(-(static_cast<int>(-UIElement::Wrap) | static_cast<int>(-UIElement::Fill)), UIElement::Wrap);
        button3.GetComponent<UIImage>().color = Color(255, 255, 255);
        button3.GetComponent<UIImage>().padding = 32;
        button3.setParent(pause);

        Object text = ECS::createObject();
        text.AddComponent<UIText>().setSize(UIElement::Wrap, UIElement::Wrap);
        text.GetComponent<UIText>().text = buttons[i].first;
        text.GetComponent<UIText>().buildMesh();
        text.setParent(button3);
    }

    //settings
    settings.AddComponent<UIImage>().setSize(UIElement::Fill, UIElement::Fill);
    settings.GetComponent<UIImage>().direction = Direction::Vertical;
    settings.GetComponent<UIImage>().align = Align::Start;
    settings.GetComponent<UIImage>().overflow = false;
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
        [](Object slider) {
            SettingsSystem::GetSettings().sensitivityX = slider.GetComponent<Slider>().value;
            SettingsSystem::Save();
    }).setParent(settings);


    PrefabSystem::getInstance().createSlider("SensitivityY", SettingsSystem::GetSettings().sensitivityY, 
        [](Object slider) {
            SettingsSystem::GetSettings().sensitivityY = slider.GetComponent<Slider>().value;
            SettingsSystem::Save();
    }).setParent(settings);


    PrefabSystem::getInstance().createCheckbox("Fullscreen", SettingsSystem::GetSettings().fullscreen, 
        [](Object box) {
            IOSystem::getWindow().setFullscreen(box.GetComponent<Checkbox>().statement);
            SettingsSystem::GetSettings().fullscreen = box.GetComponent<Checkbox>().statement;
            SettingsSystem::Save();
    }).setParent(settings);


    PrefabSystem::getInstance().createCheckbox("VSync", SettingsSystem::GetSettings().vsync, 
        [](Object box) {
            IOSystem::getWindow().setVSync(box.GetComponent<Checkbox>().statement);
            SettingsSystem::GetSettings().vsync = box.GetComponent<Checkbox>().statement;
            SettingsSystem::Save();
    }).setParent(settings);
    

    UISystem::getInstance().Rebuild(uiLayout);
}