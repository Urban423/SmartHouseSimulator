#include "OverlayManager.h"
#include "prefabs.h"

SystemState OverlayManager::state = SystemState::Menu;

Object createSettings(Object settings) {
    settings.AddComponent<Active>().enabled = true;
    auto& settingsOverlay = settings.AddComponent<OverlayElement>();
    settingsOverlay.useState = false;
    settingsOverlay.state = SystemState::Paused;
    
    settings.AddComponent<UIPopup>();
    settings.AddComponent<UIImage>().setSize(UISizeFlags::Fill, UISizeFlags::Fill);
    settings.GetComponent<UIImage>().pivot = Anchor::Top;
    settings.GetComponent<UIImage>().anchor = Anchor::Top;
    settings.GetComponent<UIImage>().direction = Direction::Vertical;
    settings.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
    settings.GetComponent<UIImage>().alignY = UIAlignFlags::Start;
    settings.GetComponent<UIImage>().overflow = true;
    settings.GetComponent<UIImage>().padding = 28;
    settings.GetComponent<UIImage>().spacing = 20;
    settings.GetComponent<UIImage>().color = Color(0, 0, 0, 50);

    Object settingsText = ECS::createObject();
    settingsText.AddComponent<UIText>().text = "SETTINGS";
    settingsText.GetComponent<UIText>().color = 255;
    settingsText.GetComponent<UIText>().fontSize = 64;
    settingsText.GetComponent<UIText>().buildMesh();
    settingsText.setParent(settings);

    PrefabSystem::getInstance().createWidgetLabel("SensitivityX", Object(), false, 255).setParent(settings);
    PrefabSystem::getInstance().createSlider(SettingsSystem::GetSettings().sensitivityX, 
        [](float value) {
            SettingsSystem::GetSettings().sensitivityX = value;
            SettingsSystem::Save();
    }).setParent(settings);


    PrefabSystem::getInstance().createWidgetLabel("SensitivityY", Object(), false, 255).setParent(settings);
    PrefabSystem::getInstance().createSlider(SettingsSystem::GetSettings().sensitivityY, 
        [](float value) {
            SettingsSystem::GetSettings().sensitivityY = value;
            SettingsSystem::Save();
    }).setParent(settings);

    // std::vector<std::string> windowModes {
    //     "Window",
    //     "Borderless",
    //     "Fullscreen"
    // };
    // PrefabSystem::getInstance().createRadioGroup(windowModes, (int)SettingsSystem::GetSettings().winMode, 
    //     [](int mode) {
    //     IOSystem::getWindow().setWindowMode((WindowMode)mode);
    //     SettingsSystem::GetSettings().winMode = (WindowMode)mode;
    //     SettingsSystem::Save();
    // }).setParent(settings);

    // PrefabSystem::getInstance().createWidgetLabel("Resolution", PrefabSystem::getInstance().createDropdown(0, resolutions,  
    //     [](int resolution) {
    //         auto [width, height] = IOSystem::getSupportedResolutions()[resolution];
    //         IOSystem::getWindow().setSize(width, height);
    //         SettingsSystem::Save();
    // })).setParent(settings);



    Object resolutionBar = ECS::createObject();
    resolutionBar.AddComponent<UIBox>().direction = Direction::Horizontal;
    resolutionBar.GetComponent<UIBox>().alignX = UIAlignFlags::Justify;
    resolutionBar.GetComponent<UIBox>().setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
    resolutionBar.setParent(settings);

    Object resolutionText = ECS::createObject();
    resolutionText.AddComponent<UIText>().text = "Resolution Scale";
    resolutionText.GetComponent<UIText>().color = 255;
    resolutionText.GetComponent<UIText>().buildMesh();
    resolutionText.setParent(resolutionBar);

    // auto& resolutions = IOSystem::getSupportedResolutions();  for(auto b : resolutions) printf("%dx%d\n", b.first, b.second);
    // int w = resolutions.begin()->first * SettingsSystem::GetSettings().resolution;
    // int h = resolutions.begin()->second * SettingsSystem::GetSettings().resolution;
    
    Object resolutionStateText = ECS::createObject();
    resolutionStateText.AddComponent<UIText>().text = std::to_string((int)(100 * SettingsSystem::GetSettings().resolution)) + "%" ;
    resolutionStateText.GetComponent<UIText>().color = 255;
    resolutionStateText.GetComponent<UIText>().buildMesh();
    resolutionStateText.setParent(resolutionBar);
    int resolutionTextID = resolutionStateText.getID();

    PrefabSystem::getInstance().createFixedSlider(SettingsSystem::GetSettings().resolution, 16, 0.25, 1.0f, 
        [resolutionTextID](float value) {
            if(SettingsSystem::GetSettings().resolution == value) return;

            Rect window = IOSystem::getWindow().getInnerSize();
            Rect renderSize = window * value;
            
            int width = renderSize.width();
            int height = renderSize.height();
            Span<Camera> cameras = ECS::GetComponents<Camera>();
            for(auto& cam : cameras) {
                TextureManager::ResizeFrameBuffer(cam.frameBufferIndex, width, height);
            }

            auto& text = ECS::GetComponent<UIText>(resolutionTextID);
            text.text = std::to_string((int)(100 * value)) + "%";
            text.buildMesh();
            SettingsSystem::GetSettings().resolution = value;
            SettingsSystem::Save();
    }).setParent(settings);

    PrefabSystem::getInstance().createWidgetLabel("Fullscreen", PrefabSystem::getInstance().createCheckbox(SettingsSystem::GetSettings().fullscreen, 
        [](bool checked) {
            IOSystem::getWindow().setFullscreen(checked);
            SettingsSystem::GetSettings().fullscreen = checked;
            SettingsSystem::Save();
    }), true, 255).setParent(settings);


    PrefabSystem::getInstance().createWidgetLabel("VSync", PrefabSystem::getInstance().createCheckbox(SettingsSystem::GetSettings().vsync,
        [](bool checked) {
            IOSystem::getWindow().setVSync(checked);
            SettingsSystem::GetSettings().vsync = checked;
            SettingsSystem::Save();
    }), true, 255).setParent(settings);

    return settings;
}

Object createPause(int settingsPanelID) {
    Object pause = ECS::createObject();
    pause.AddComponent<Active>().enabled = true;
    auto& pauseOverlay = pause.AddComponent<OverlayElement>();
    pauseOverlay.useState = true;
    pauseOverlay.state = SystemState::Paused;

    pause.AddComponent<UIPopup>();
    pause.AddComponent<UIImage>().setSize(UISizeFlags::Fill, UISizeFlags::Fill);
    pause.GetComponent<UIImage>().direction = Direction::Vertical;
    pause.GetComponent<UIImage>().alignX = UIAlignFlags::Center;
    pause.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
    pause.GetComponent<UIImage>().overflow = true;
    pause.GetComponent<UIImage>().padding = 32;
    pause.GetComponent<UIImage>().spacing = 10;
    pause.GetComponent<UIImage>().color = Color(0, 0, 0, 50);

    Object text = ECS::createObject();
    text.AddComponent<UIText>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
    text.GetComponent<UIText>().text = "PAUSE";
    text.GetComponent<UIText>().color = 255;
    text.GetComponent<UIText>().fontSize = 64;
    text.GetComponent<UIText>().buildMesh();
    text.setParent(pause);

    std::vector<std::pair<const char*, std::function<void()>>> buttons = {
        { "Resume", [] { OverlayManager::SetState(SystemState::Playing); }},

        { "Settings", [settingsPanelID] {
            auto& active = ECS::GetComponent<Active>(settingsPanelID);
            active.enabled = !active.enabled;
        }},
        { "Exit", [] { OverlayManager::SetState(SystemState::Menu); } },
    };
    for(int i = 0; i < buttons.size(); i++) {
        Object button = PrefabSystem::getInstance().createButton(buttons[i].first, buttons[i].second);
        button.GetComponent<UIImage>().padding = 32;
        button.setParent(pause);
    }

    return pause;
}

Object createMultiplayerMenu(Object menu) {
    menu.AddComponent<Active>().enabled = true;
    auto& overlay = menu.AddComponent<OverlayElement>();
    overlay.useState = false;

    auto& box = menu.AddComponent<UIImage>();
    box.setSize(UISizeFlags::Fill, UISizeFlags::Fill);
    box.direction = Direction::Vertical;
    box.alignX = UIAlignFlags::Start;
    box.alignY = UIAlignFlags::Start;
    box.padding = 32;
    box.spacing = 20;
    box.color = Color(0, 0, 0, 50);
    
    Object title = ECS::createObject(); 
    auto& text = title.AddComponent<UIText>(); 
    text.text = "Multiplayer"; 
    text.fontSize = 64; 
    text.color = 255;
    text.buildMesh(); 
    title.setParent(menu);


    Object createPanel = ECS::createObject();
    createPanel.setParent(menu);
    auto& createPanelBox = createPanel.AddComponent<UIImage>();
    createPanelBox.setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
    createPanelBox.direction = Direction::Vertical;
    createPanelBox.padding = 16;
    createPanelBox.spacing = 10;
    createPanelBox.color = 50;

    Object host = ECS::createObject(); 
    auto& hostText = host.AddComponent<UIText>(); 
    hostText.text = "Host";
    hostText.color = 255;
    hostText.buildMesh(); 
    host.setParent(createPanel);

    Object portCreate = PrefabSystem::getInstance().createInputField("7777", 5, [](char c) {
            return std::isdigit(static_cast<unsigned char>(c));
    });
    Object portWidget = PrefabSystem::getInstance().createWidgetLabel("Port", portCreate, true, 255);
    portWidget.setParent(createPanel);


    int portCreateID = portCreate.getChild(0).getID();
    PrefabSystem::getInstance().createButton("Create Server", [portCreateID] {
        int port = std::stoi(ECS::GetComponent<UIText>(portCreateID).text);
        ClientServerSystem::getInstance().host(port);
        OverlayManager::SetState(SystemState::Playing);
    }).setParent(createPanel);



    Object joinPanel = ECS::createObject();
    joinPanel.setParent(menu);
    auto& joinBox = joinPanel.AddComponent<UIImage>();
    joinBox.setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
    joinBox.direction = Direction::Vertical;
    joinBox.padding = 16;
    joinBox.spacing = 10;
    joinBox.color = 50;

    Object join = ECS::createObject(); 
    auto& joinText = join.AddComponent<UIText>(); 
    joinText.text = "Join";
    joinText.color = 255;
    joinText.buildMesh(); 
    join.setParent(joinPanel);

    Object joinFields = ECS::createObject();
    joinFields.setParent(joinPanel);
    auto& joinFieldsBox = joinFields.AddComponent<UIBox>();
    joinFieldsBox.setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
    joinFieldsBox.direction = Direction::Horizontal;
    joinFieldsBox.spacing = 10;

    Object joinIPPanel = PrefabSystem::getInstance().createInputField("192.185.186.222", 16, [](char c) {
        return std::isdigit(static_cast<unsigned char>(c)) || c == '.';
    });
    joinIPPanel.GetComponent<UIImage>().setSize(UISizeFlags::Fill | UISizeFlags::Wrap, UISizeFlags::Wrap);
    // Object joinIPPanelWidget = PrefabSystem::getInstance().createWidgetLabel("IP", joinIPPanel, true, 255);
    joinIPPanel.setParent(joinFields);


    Object joinPort = PrefabSystem::getInstance().createInputField("7777", 5, [](char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    });
    joinPort.GetComponent<UIImage>().setSize(UISizeFlags::Fill | UISizeFlags::Wrap, UISizeFlags::Wrap);
    // Object joinPortWidget = PrefabSystem::getInstance().createWidgetLabel("Port", joinPort, true, 255);
    joinPort.setParent(joinFields);

    int joinIPPanelID = joinIPPanel.getChild(0).getID();
    int joinPortID = joinPort.getChild(0).getID();
    Object connectButton = PrefabSystem::getInstance().createButton("Connect", [joinIPPanelID, joinPortID] {
        ClientServerSystem::getInstance().connect(ECS::GetComponent<UIText>(joinIPPanelID).text.c_str(), std::stoi(ECS::GetComponent<UIText>(joinPortID).text));
        OverlayManager::SetState(SystemState::Playing);
    });
    connectButton.setParent(joinPanel);

    return menu;
}

Object createMenu(int settingsPanelID, int multiplayerMenu) {
    Object menu = ECS::createObject();
    menu.AddComponent<Active>().enabled = true;
    auto& menuOverlay = menu.AddComponent<OverlayElement>();
    menuOverlay.useState = true;
    menuOverlay.state = SystemState::Menu;

    menu.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap | UISizeFlags::Fill);
    menu.GetComponent<UIImage>().direction = Direction::Vertical;
    menu.GetComponent<UIImage>().pivot = Anchor::Center;
    menu.GetComponent<UIImage>().anchor = Anchor::Center;
    menu.GetComponent<UIImage>().alignX = UIAlignFlags::Center;
    menu.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
    menu.GetComponent<UIImage>().overflow = true;
    menu.GetComponent<UIImage>().padding = 32;
    menu.GetComponent<UIImage>().spacing = 10;
    menu.GetComponent<UIImage>().color = Color(0, 0, 0, 50);

    Object text = ECS::createObject();
    text.AddComponent<UIText>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
    text.GetComponent<UIText>().text = "MENU";
    text.GetComponent<UIText>().color = 255;
    text.GetComponent<UIText>().fontSize = 64;
    text.GetComponent<UIText>().buildMesh();
    text.setParent(menu);

    std::vector<std::pair<const char*, std::function<void()>>> buttons = {
        { "Play Offline", [] { OverlayManager::SetState(SystemState::Playing); }},

        { "Play Online", [multiplayerMenu, settingsPanelID] { 
            auto& activeMenu = ECS::GetComponent<Active>(multiplayerMenu);
            activeMenu.enabled = !activeMenu.enabled;

            auto& activeSettings = ECS::GetComponent<Active>(settingsPanelID);
            activeSettings.enabled = false;
        }},

        { "Settings", [multiplayerMenu, settingsPanelID] {
            auto& activeMenu = ECS::GetComponent<Active>(multiplayerMenu);
            activeMenu.enabled = false;

            auto& activeSettings = ECS::GetComponent<Active>(settingsPanelID);
            activeSettings.enabled = !activeSettings.enabled;
        }},
        { "Exit", [] { IOSystem::getWindow().onDestroy(); } },
    };
    for(int i = 0; i < buttons.size(); i++) {
        Object button = PrefabSystem::getInstance().createButton(buttons[i].first, buttons[i].second);
        button.GetComponent<UIImage>().padding = 32;
        button.setParent(menu);
    }

    return menu;
}

void OverlayManager::Create() {
    Object uiLayout = ECS::createObject();
    uiLayout.transform.position = Vector2(-0.6f, 3.0f);
    uiLayout.AddComponent<UILayout>();
    uiLayout.AddComponent<UIImage>().color = Color(1, 1, 250, 12);
    uiLayout.GetComponent<UIImage>().direction = Direction::Horizontal;
    uiLayout.GetComponent<UIImage>().alignX = UIAlignFlags::Center;
    uiLayout.GetComponent<UIImage>().alignY = UIAlignFlags::Start;
    uiLayout.GetComponent<UIImage>().spacing = 10;
    uiLayout.GetComponent<UIImage>().padding = 10;

    Object uiLeft = ECS::createObject();
    uiLeft.AddComponent<UIBox>().setSize(UISizeFlags::Wrap, UISizeFlags::Fill);
    uiLeft.GetComponent<UIBox>().direction = Direction::Vertical;
    uiLeft.GetComponent<UIBox>().alignX = UIAlignFlags::Center;
    uiLeft.GetComponent<UIBox>().alignY = UIAlignFlags::Center;
    uiLeft.GetComponent<UIBox>().overflow = true;
    uiLeft.setParent(uiLayout);

    Object uiRight = ECS::createObject();
    uiRight.AddComponent<UIBox>().setSize(UISizeFlags::Fill, UISizeFlags::Fill);
    uiRight.GetComponent<UIBox>().direction = Direction::Vertical;
    uiRight.GetComponent<UIBox>().alignX = UIAlignFlags::Start;
    uiRight.GetComponent<UIBox>().alignY = UIAlignFlags::Start;
    uiRight.GetComponent<UIBox>().overflow = true;
    uiRight.setParent(uiLayout);

    Object settings = ECS::createObject();
    int settingsID = settings.getID();

    Object multiplayerMenu = ECS::createObject();
    int multiplayerMenuID = multiplayerMenu.getID();

    createMenu(settingsID, multiplayerMenuID).setParent(uiLeft);
    createPause(settingsID).setParent(uiLeft);
    createMultiplayerMenu(multiplayerMenu).setParent(uiRight);
    createSettings(settings).setParent(uiRight);

    SetState(SystemState::Menu);
}