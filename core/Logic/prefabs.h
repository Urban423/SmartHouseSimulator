#pragma once
#include "Transform.h"
#include "ECS.h"
#include "Physic.h"
#include "CameraControlSystem.h"
#include "TextureManager.h"
#include "UIManager.h"
#include "MaterialManager.h"

class PrefabSystem {
private:
    short testmat;
	short mainMaterial; 
    int frameBufferMain;
public:
    static PrefabSystem& getInstance() { static PrefabSystem prefabSystem; return prefabSystem; }

    inline void createMaterials() {
        frameBufferMain = TextureManager::CreateFrameBuffer();
        mainMaterial = MaterialManager::CreateMaterial(Material(1, frameBufferMain, 0, 0.1f));
        testmat = MaterialManager::CreateMaterial(Material(0, 3, 0, 0xff));
    }

    inline int getMainMaterial() { return mainMaterial; }

    inline Object createPlayer(bool isClient) {
        Object player = ECS::createObject();
        player.transform.position = Vector3(0, 0, 20);

        player.AddComponent<RenderView>().mesh_index = 2;
        player.GetComponent<RenderView>().materals[0] = testmat;
        player.AddComponent<Rigidbody>().isKinematic;
        player.GetComponent<Rigidbody>().angularLock = AxisLock::X | AxisLock::Y | AxisLock::Z;
        // player.GetComponent<Rigidbody>().linearLock = AxisLock::X | AxisLock::Y | AxisLock::Z; 
        player.AddComponent<SphereCollider>();
        player.AddComponent<CameraControlSystem>(); 
        player.AddComponent<InputComponent>(); 
        player.AddComponent<NetworkIdentity>();

        if (isClient) { createCamera(player); }

        return player;
    }
    
    void createCamera(Object player) {
        Object cam = ECS::createObject();

        cam.transform.position = Vector3(0, 0.1f, 0);
        cam.transform.rotation = Quaternion::FromEuler(13, 0, 0);
        cam.AddComponent<Camera>().color = Color(0.39f, 0.72f, 1.0f);
        cam.GetComponent<Camera>().frameBufferIndex = frameBufferMain;
        cam.GetComponent<Camera>().perpective = true;

        cam.setParent(player);
    }

    Object createButton(const char* label, std::function<void()> onClickDown) {
        Object button = ECS::createObject();
        button.AddComponent<UIImage>().padding = 12;
        button.GetComponent<UIImage>().setHeight(UISizeFlags::Wrap);
        button.GetComponent<UIImage>().color = Color(111, 111, 111);
        button.AddComponent<Button>();
        if (onClickDown) {
            button.GetComponent<Button>().onClickDown = onClickDown;
        }

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(button);
        return button;
    }


    Object createSlider(const char* label, float startStatement, std::function<void(float)> onChangeEnd) {
        Object root = ECS::createObject();
        root.AddComponent<UIBox>().setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
        root.GetComponent<UIBox>().direction = Direction::Horizontal;
        root.GetComponent<UIBox>().alignX = UIAlignFlags::Center;
        root.GetComponent<UIBox>().alignY = UIAlignFlags::Center;
        // root.GetComponent<UIBox>().padding = 12;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(root);

        Object slider = ECS::createObject();
        slider.AddComponent<Slider>().value = startStatement;
        if (onChangeEnd) {
            slider.GetComponent<Slider>().onDragEnd = onChangeEnd;
        }
        slider.AddComponent<UIImage>().setSize(UISizeFlags::Fill, 22);
        slider.GetComponent<UIImage>().direction = Direction::Absolute;
        slider.GetComponent<UIImage>().color = Color(111, 111, 111);
        slider.setParent(root);

        Object handle = ECS::createObject();
        handle.AddComponent<UIImage>().setSize(44, 44);
        handle.setParent(slider);

        return root;
    }

    Object createCheckbox(const char* label, bool startStatement, std::function<void(bool)> onValueChanged) {
        Object checkbox = ECS::createObject();
        checkbox.AddComponent<UIBox>().setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
        checkbox.GetComponent<UIBox>().direction = Direction::Horizontal;
        checkbox.GetComponent<UIBox>().alignX = UIAlignFlags::Justify;
        checkbox.GetComponent<UIBox>().alignY = UIAlignFlags::Center;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(checkbox);

        Object box = ECS::createObject();
        box.AddComponent<Checkbox>().checked = startStatement;
        if(onValueChanged) {
            box.GetComponent<Checkbox>().onValueChanged = onValueChanged;
        }
        box.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
        box.GetComponent<UIImage>().color = Color(255, 255, 255);
        box.GetComponent<UIImage>().padding = 12;
        box.setParent(checkbox);

        Object checkMark = ECS::createObject();
        checkMark.AddComponent<Active>().enabled = startStatement;
        checkMark.AddComponent<UIImage>().setSize(30, 30);
        checkMark.GetComponent<UIImage>().color = Color(0, 0, 0);
        checkMark.setParent(box);
        return checkbox;
    }

    Object createInputField(const char* label, const char* startText, size_t maxLength, std::function<bool(char)> charFilter) {
        Object inputField = ECS::createObject();
        inputField.AddComponent<UIBox>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
        inputField.GetComponent<UIBox>().direction = Direction::Horizontal;
        inputField.GetComponent<UIBox>().alignX = UIAlignFlags::Center;
        inputField.GetComponent<UIBox>().alignY = UIAlignFlags::Center;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(inputField);

        Object background = ECS::createObject();
        background.AddComponent<InputField>().maxLength = maxLength;
        background.GetComponent<InputField>().charFilter = charFilter;
        background.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
        background.GetComponent<UIImage>().padding = 32;
        background.GetComponent<UIImage>().color = Color(255, 255, 255);
        background.setParent(inputField);

        Object text = ECS::createObject();
        text.AddComponent<UIText>().text = startText;
        text.GetComponent<UIText>().buildMesh();
        text.setParent(background);
        return inputField;
    }

    Object createDropdown(const char* label, const char* baseText, const std::vector<std::pair<int,int>>& values, std::function<void()> onChange) {
        Object commonParent = ECS::createObject();
        commonParent.AddComponent<UIBox>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
        commonParent.GetComponent<UIBox>().direction = Direction::Horizontal;
        commonParent.GetComponent<UIBox>().alignX = UIAlignFlags::Start;
        commonParent.GetComponent<UIBox>().alignY = UIAlignFlags::Center;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(commonParent);


        Object dropdown = ECS::createObject();
        dropdown.AddComponent<UIBox>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
        dropdown.GetComponent<UIBox>().direction = Direction::Absolute;
        dropdown.GetComponent<UIBox>().alignX = UIAlignFlags::Start;
        dropdown.GetComponent<UIBox>().alignY = UIAlignFlags::Center;
        dropdown.setParent(commonParent);
        
        Object dropdownButton = ECS::createObject();
        dropdownButton.AddComponent<Dropdown>();
        dropdownButton.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
        dropdownButton.GetComponent<UIImage>().direction = Direction::Vertical;
        dropdownButton.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
        dropdownButton.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
        dropdownButton.GetComponent<UIImage>().overflow = false;
        dropdownButton.GetComponent<UIImage>().padding = 12;
        dropdownButton.GetComponent<UIImage>().color = Color(255, 255, 255);
        dropdownButton.setParent(dropdown);

        Object dropdownList = ECS::createObject();
        dropdownButton.GetComponent<Dropdown>().listID = dropdownList.getID();
        dropdownList.AddComponent<Active>().enabled = false;
        dropdownList.AddComponent<UIImage>().setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
        dropdownList.GetComponent<UIImage>().pivot = Anchor::Bottom;
        dropdownList.GetComponent<UIImage>().anchor = Anchor::Bottom;
        dropdownList.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
        dropdownList.GetComponent<UIImage>().alignY = UIAlignFlags::Center; 
        dropdownList.GetComponent<UIImage>().direction = Direction::Absolute;
        dropdownList.GetComponent<UIImage>().overflow = false;
        dropdownList.GetComponent<UIImage>().padding = 1;
        dropdownList.GetComponent<UIImage>().color = Color(255, 255, 255);
        dropdownList.setParent(dropdownButton);

        for(int i = 0; i < values.size(); i++) {
            Object testItem = createButton((std::to_string(values[i].first) + "x" + std::to_string(values[i].second)).c_str(), onChange);
            testItem.GetComponent<UIImage>().setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
            testItem.setParent(dropdownList);
        }


        Object text = ECS::createObject();
        text.AddComponent<UIText>().text = baseText;
        text.GetComponent<UIText>().buildMesh();
        text.setParent(dropdownButton);

        return commonParent;
    }
};