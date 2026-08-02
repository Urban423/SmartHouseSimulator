#include "prefabs.h"

Object PrefabSystem::createWidgetLabel(const char* label, Object widget, bool horizontal, Color textColor) {
    Object commonParent = ECS::createObject();
    commonParent.AddComponent<UIBox>().setSize(UISizeFlags::Fill | UISizeFlags::Wrap, UISizeFlags::Wrap);
    commonParent.GetComponent<UIBox>().direction = horizontal ? Direction::Horizontal: Direction::Vertical;
    commonParent.GetComponent<UIBox>().alignX = UIAlignFlags::Start;
    commonParent.GetComponent<UIBox>().alignY = UIAlignFlags::Center;
    commonParent.GetComponent<UIBox>().spacing = 20;

    Object labelText = ECS::createObject();
    labelText.AddComponent<UIText>().text = label;
    labelText.GetComponent<UIText>().color = textColor;
    labelText.GetComponent<UIText>().buildMesh();
    labelText.setParent(commonParent);

    if(widget.valid()) widget.setParent(commonParent);
    return commonParent;
}

Object PrefabSystem::createInputField(const char* startText, size_t maxLength, std::function<bool(char)> charFilter) {
    Object background = ECS::createObject();
    Interactable& interactable = background.AddComponent<Interactable>();
    background.AddComponent<InputField>().maxLength = maxLength;
    background.GetComponent<InputField>().charFilter = charFilter;
    background.AddComponent<UIImage>().setSize(UISizeFlags::Fill, 55);
    background.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
    background.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
    background.GetComponent<UIImage>().padding = 12;
    background.GetComponent<UIImage>().scalable = false;
    background.GetComponent<UIImage>().color = Color(255, 255, 255);

    Object text = ECS::createObject();
    text.AddComponent<UIText>().text = startText;
    text.GetComponent<UIText>().buildMesh();
    text.setParent(background);
    return background;
}

Object PrefabSystem::createButton(const char* label, std::function<void()> onClickDown) {
    Object button = ECS::createObject();
    button.AddComponent<UIImage>().padding = 12;
    button.GetComponent<UIImage>().setSize(UISizeFlags::Wrap | UISizeFlags::Fill, UISizeFlags::Wrap);
    button.GetComponent<UIImage>().color = Color(111, 111, 111);
    button.AddComponent<Interactable>();
    if (onClickDown) {
        button.GetComponent<Interactable>().onClickDown = onClickDown;
    }
    button.AddComponent<SelectableStyle>();

    if(label != nullptr) {
        Object labelText = ECS::createObject();
        labelText.AddComponent<UIText>().text = label;
        labelText.GetComponent<UIText>().buildMesh();
        labelText.setParent(button);
    }
    return button;
}

Object PrefabSystem::createSlider(float startStatement, std::function<void(float)> onChangeEnd) {
    Object slider = ECS::createObject();
    slider.AddComponent<Interactable>();
    slider.AddComponent<Slider>().value = startStatement;
    if (onChangeEnd) {
        slider.GetComponent<Slider>().onDragEnd = onChangeEnd;
    }
    slider.AddComponent<UIImage>().setSize(UISizeFlags::Fill, 22);
    slider.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
    slider.GetComponent<UIImage>().color = 255;

    Object handle = createButton(nullptr, nullptr);
    handle.AddComponent<UIImage>().setSize(44, 44);
    handle.GetComponent<SelectableStyle>().normal = 200;
    handle.setParent(slider);

    return slider;
}

Object PrefabSystem::createFixedSlider(float startStatement, int fixedPositions, float minValue, float maxValue, std::function<void(float)> onDrag) {
    Object fixedSlider = ECS::createObject();
    fixedSlider.AddComponent<Interactable>();
    fixedSlider.AddComponent<Slider>().value = startStatement;
    fixedSlider.GetComponent<Slider>().minValue = minValue;
    fixedSlider.GetComponent<Slider>().maxValue = maxValue;
    fixedSlider.GetComponent<Slider>().fixedPositions = fixedPositions;
    if (onDrag) fixedSlider.GetComponent<Slider>().onDrag = onDrag;
    fixedSlider.AddComponent<UIImage>().setSize(UISizeFlags::Fill, 22);
    fixedSlider.GetComponent<UIImage>().alignX = UIAlignFlags::Justify;
    fixedSlider.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
    fixedSlider.GetComponent<UIImage>().color = 255;
    fixedSlider.GetComponent<UIImage>().padding = {19, 19, 0, 0};
    fixedSlider.GetComponent<UIImage>().scalable = false;

    Object handle = createButton(nullptr, nullptr);
    handle.AddComponent<UIPopup>();
    handle.AddComponent<UIImage>().setSize(44, 44);
    handle.GetComponent<SelectableStyle>().normal = 200;
    handle.setParent(fixedSlider);

    for (int i = 0; i < fixedPositions; ++i) {
        Object mark = ECS::createObject();
        auto& img = mark.AddComponent<UIImage>();
        img.setSize(6, 6);
        img.color = 180;
        mark.setParent(fixedSlider);
    }

    return fixedSlider;
}

Object PrefabSystem::createCheckbox(bool startStatement, std::function<void(bool)> onValueChanged) {
    Object checkbox = ECS::createObject();

    Interactable& interactable = checkbox.AddComponent<Interactable>();
    Toggle& toggle = checkbox.AddComponent<Toggle>();
    toggle.value = startStatement;
    if(onValueChanged) {
        toggle.onValueChanged = onValueChanged;
    }
    UIImage& checkboxBox = checkbox.AddComponent<UIImage>();
    checkboxBox.setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
    checkboxBox.color = Color(255, 255, 255);
    checkboxBox.padding = 12;


    Object checkMark = ECS::createObject();
    checkMark.AddComponent<Active>().enabled = startStatement;
    checkMark.setParent(checkbox);
    UIImage& checkMarkUI = checkMark.AddComponent<UIImage>();
    checkMarkUI.setSize(30, 30);
    checkMarkUI.alignX = UIAlignFlags::Center;
    checkMarkUI.setSize(UISizeFlags::Fill, UISizeFlags::Fill);
    checkMarkUI.color = Color(0, 0, 0);
    return checkbox;
}

Object PrefabSystem::createRadioGroup(const std::vector<std::string>& values, int startValue, std::function<void(int)> onValueSelect) {
    Object radioGroup = ECS::createObject();

    radioGroup.AddComponent<RadioGroup>().onValueSelect = onValueSelect;
    UIBox& uiBox = radioGroup.AddComponent<UIBox>();
    uiBox.setSize(UISizeFlags::Fill, UISizeFlags::Wrap);
    uiBox.direction = Direction::Horizontal;
    uiBox.alignX = UIAlignFlags::Start;
    uiBox.alignY = UIAlignFlags::Center;
    uiBox.padding = 12;

    for(int i = 0; i < values.size(); i++) {
        Object button = createButton(values[i].c_str(), nullptr);
        button.setParent(radioGroup);

        button.GetComponent<UIImage>().setWidth(UISizeFlags::Fill);

        Toggle& toggle = button.AddComponent<Toggle>();
        toggle.value = (i == startValue);
    }
    return radioGroup;
}

// Object createDropdown(int startValue, const std::vector<std::pair<int,int>>& values, std::function<void(int)> onValueSelect) {
//     Object dropdown = ECS::createObject();
//     dropdown.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
//     dropdown.GetComponent<UIImage>().direction = Direction::Vertical;
//     dropdown.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
//     dropdown.GetComponent<UIImage>().alignY = UIAlignFlags::Center;
//     dropdown.GetComponent<UIImage>().overflow = false;
//     dropdown.GetComponent<UIImage>().padding = 16;
//     dropdown.GetComponent<UIImage>().color = Color(255, 255, 255);

//     Object text = ECS::createObject();
//     text.AddComponent<UIText>();
//     text.setParent(dropdown);
    
//     Object dropdownList = ECS::createObject();
//     dropdownList.AddComponent<Active>().enabled = false;
//     dropdownList.AddComponent<UIPopup>();
//     dropdownList.AddComponent<RadioGroup>().onValueSelect = onValueSelect;
//     dropdownList.setParent(dropdown);

//     dropdownList.AddComponent<UIImage>().setSize(UISizeFlags::Wrap, UISizeFlags::Wrap);
//     dropdownList.GetComponent<UIImage>().pivot = Anchor::Top;
//     dropdownList.GetComponent<UIImage>().anchor = Anchor::Bottom;
//     dropdownList.GetComponent<UIImage>().direction = Direction::Vertical;
//     dropdownList.GetComponent<UIImage>().alignX = UIAlignFlags::Start;
//     dropdownList.GetComponent<UIImage>().alignY = UIAlignFlags::Center; 
//     dropdownList.GetComponent<UIImage>().overflow = false;
//     dropdownList.GetComponent<UIImage>().color = Color(255, 255, 255);

//     int dropdownListID = dropdownList.getID();
//     for(int i = 0; i < values.size(); i++) {
//         Object testItem = createButton((std::to_string(values[i].first) + "x" + std::to_string(values[i].second)).c_str(), [dropdownListID] {
//             auto& active = ECS::GetComponent<Active>(dropdownListID);
//             active.enabled = !active.enabled;
//         });
//         testItem.GetComponent<UIImage>().setSize(UISizeFlags::Fill | UISizeFlags::Wrap, UISizeFlags::Wrap);
//         testItem.setParent(dropdownList);
//         if(i == 0) text.GetComponent<UIText>().copyFrom(testItem.getChild(0).GetComponent<UIText>());
//     }

//     dropdown.AddComponent<SelectableStyle>();
//     dropdown.AddComponent<Interactable>().onClickDown = [dropdownListID] {
//         auto& active = ECS::GetComponent<Active>(dropdownListID);
//         active.enabled = !active.enabled;
//     };

//     return dropdown;
// }