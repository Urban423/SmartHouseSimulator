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

    Object createSlider(const char* label, float startStatement, std::function<void(Object)> onChangeEnd) {
        Object root = ECS::createObject();
        root.AddComponent<UIBox>().setSize(UIElement::Wrap, UIElement::Wrap);
        root.GetComponent<UIBox>().direction = Direction::Horizontal;
        root.GetComponent<UIBox>().align = Align::Center;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(root);

        Object slider = ECS::createObject();
        slider.AddComponent<Slider>().value = startStatement;
        if (onChangeEnd) {
            slider.GetComponent<Slider>().onChangeEnd = [slider, onChangeEnd]() { onChangeEnd(slider); };
        }
        slider.AddComponent<UIImage>().setSize(200, 22);
        slider.GetComponent<UIImage>().direction = Direction::Absolute;
        slider.GetComponent<UIImage>().color = Color(111, 111, 111);
        slider.setParent(root);

        Object handle = ECS::createObject();
        handle.AddComponent<UIImage>().setSize(44, 44);
        handle.setParent(slider);

        return root;
    }

    Object createCheckbox(const char* label, bool startStatement, std::function<void(Object)> onClickDown) {
        Object checkbox = ECS::createObject();
        checkbox.AddComponent<UIBox>().setSize(UIElement::Wrap, UIElement::Wrap);
        checkbox.GetComponent<UIBox>().direction = Direction::Horizontal;
        checkbox.GetComponent<UIBox>().align = Align::Center;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(checkbox);

        Object box = ECS::createObject();
        box.AddComponent<Checkbox>().statement = startStatement;
        if(onClickDown) {
            box.GetComponent<Checkbox>().onClickDown = [box, onClickDown]() { onClickDown(box); };
        }
        box.AddComponent<UIImage>().setSize(UIElement::Wrap, UIElement::Wrap);
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
        inputField.AddComponent<UIBox>().setSize(UIElement::Wrap, UIElement::Wrap);
        inputField.GetComponent<UIBox>().direction = Direction::Horizontal;
        inputField.GetComponent<UIBox>().align = Align::Center;

        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(inputField);

        Object background = ECS::createObject();
        background.AddComponent<InputField>().maxLength = maxLength;
        background.GetComponent<InputField>().charFilter = charFilter;
        background.AddComponent<UIImage>().setSize(UIElement::Wrap, UIElement::Wrap);
        background.GetComponent<UIImage>().padding = 32;
        background.GetComponent<UIImage>().color = Color(255, 255, 255);
        background.setParent(inputField);

        Object text = ECS::createObject();
        text.AddComponent<UIText>().text = startText;
        text.GetComponent<UIText>().buildMesh();
        text.setParent(background);
        return inputField;
    }
};