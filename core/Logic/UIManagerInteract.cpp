#include "UIManager.h"

template<typename T>
inline bool updateInteractable(T& widget, const Vector2& mousePos) {
    int objID = widget.object.getID();
    if (!ECS::isActive(objID)) {
        widget.hovered = false;
        widget.pressed = false;
        return false;
    }

    UIElement* ui = TryGetUIElement(objID);
    if(!ui) return false;
    bool inside = ui->contain(mousePos);

    if(inside && !widget.pressed && IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton)) {
        widget.pressed = true;
        if(widget.onClickDown) widget.onClickDown();
    }
    else if(widget.pressed && IOSystem::getKeyBoard().GetKeyUp(KeyCode_LeftMouseButton)) { 
        widget.pressed = false;
        if(widget.onClickUp) widget.onClickUp();
    }

    if(inside && !widget.hovered) {
        widget.hovered = true;
        if(widget.onHoverEnter) widget.onHoverEnter();
    }
    else if(!inside && widget.hovered) {
        widget.hovered = false;
        if(widget.onHoverExit) widget.onHoverExit();
    }
    return inside;
}


void UISystem::Update() {
    Vector2 screenSize = ECS::GetComponents<UIImage>()[0].getComputedSize();
    Vector2 mousePos = IOSystem::getInput().pointerPosition;
	Rect winRect = IOSystem::getWindow().getInnerSize();

    float invWidth  = 1.0f / (winRect.right - winRect.left);
    float invHeight = 1.0f / (winRect.bottom - winRect.top);
    float nx = (mousePos.x - winRect.left) * invWidth;
    float ny = (mousePos.y - winRect.top) * invHeight;

	mousePos = {  (nx - 0.5f) * screenSize.x, (0.5f - ny) * screenSize.y };
    // printf("Width: %f Height: %f\n", winRect.right - winRect.left,  winRect.bottom - winRect.top); 

    bool clickDown = IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton);
    bool click = IOSystem::getKeyBoard().GetKey(KeyCode_LeftMouseButton);
    if(clickDown) focusedInputField = Object();
    
    CursorType cursor = CursorType::Arrow;

    Span<Button> uiButtons = ECS::GetComponents<Button>();
    for(auto& button : uiButtons) {
        bool inside = updateInteractable(button, mousePos);
        if(inside) cursor = CursorType::Hand;

        int objID = button.object.getID();
        if(ECS::HasComponent<UIImage>(objID)) {
            Color color = button.normalColor;
            if(button.hovered) color = button.hoverColor;
            if(button.pressed) color = button.pressedColor;
            ECS::GetComponent<UIImage>(objID).color = color;
        }
    }

    Span<Checkbox> checkboxs = ECS::GetComponents<Checkbox>();
    for(auto& checkbox : checkboxs) {
        bool inside = updateInteractable(checkbox, mousePos);

        if(inside) cursor = CursorType::Hand;
        if(checkbox.pressed && clickDown) {
            checkbox.checked = !checkbox.checked;
            if(checkbox.onValueChanged) checkbox.onValueChanged(checkbox.checked);

            Object child = checkbox.object.getChild(0);
            if(child.valid() && child.HasComponent<Active>()) {
                child.GetComponent<Active>().enabled = checkbox.checked;
            }
        }
    }


    Span<Slider> uiSliders = ECS::GetComponents<Slider>();
    for(auto& slider : uiSliders) {
        bool insideTrack = updateInteractable(slider, mousePos);

        Object blobObject = slider.object.getChild(0);
        UIElement* blobUI = TryGetUIElement(blobObject.getID());
        bool inside = insideTrack || blobUI && blobUI->contain(mousePos);

        if(inside) cursor = CursorType::Hand;
        if (inside && clickDown) slider.dragging = true;

        if (slider.dragging) {
            if (click) {
                int objID = slider.object.getID();
                UIElement* ui = TryGetUIElement(objID);
                Vector2 size = ui->getComputedSize();
                Vector2 center = ui->getOffset();
                float left = center.x - size.x * 0.5f;
                float t = (mousePos.x - (center.x - size.x * 0.5f)) / size.x;

                slider.value = clamp(0.0f, 1.0f, t);
                slider.calculate();
            }
            else {
                slider.dragging = false;
                if (slider.onDragEnd) slider.onDragEnd(slider.value);
            }
        }
    }

    Span<Dropdown> dropDownes = ECS::GetComponents<Dropdown>();
    for(auto& dropDown : dropDownes) {
        bool inside = updateInteractable(dropDown, mousePos);

        if(inside) cursor = CursorType::Hand;
        if(!clickDown) continue;

        if(dropDown.opened) {
            dropDown.opened = false;
        }
        else if(inside) {
            dropDown.opened = true;
        }

        if(ECS::HasComponent<Active>(dropDown.listID)) {
            ECS::GetComponent<Active>(dropDown.listID).enabled = dropDown.opened;
        }
    }

    if(!focusedInputField.valid()) {
        Span<InputField> inputFields = ECS::GetComponents<InputField>();
        for(auto& inputField : inputFields) {
            bool inside = updateInteractable(inputField, mousePos);
            if(inside) cursor = CursorType::Text;
            if(inside && clickDown) {
               focusedInputField = inputField.object;
            }
        }
    }
    IOSystem::getPlatform().setCursor(cursor);

    if(focusedInputField.valid() && focusedInputField.HasComponent<InputField>()) {
        Object textInput = focusedInputField.getChild(0);
        if(!textInput.valid() || !textInput.HasComponent<UIText>()) return;

        InputField& field = focusedInputField.GetComponent<InputField>();
        Input& input = IOSystem::getInput();

        std::string& text = textInput.GetComponent<UIText>().text;
        for(int i = 0; i < input.text.size(); i++) {
            int c = input.text[i];
            switch (c) {
                case 8: { //backspace
                    if(!text.empty()) text.pop_back();
                    break;
                }

                case 9: { //tab
                    if (field.charFilter && !field.charFilter((char)c)) break;
                    if (text.size() + 3 >= field.maxLength) break;
                    if(field.onChar) field.onChar(c);
                    else text.append("    ");
                    break;
                }

                case 13: { //enter
                    if (field.charFilter && !field.charFilter((char)c)) break;
                    if (text.size() >= field.maxLength) break;
                    if(field.onSubmit) field.onSubmit();
                    else text.push_back('\n');
                    break;
                }

                default: {
                    if (field.charFilter && !field.charFilter((char)c)) break;
                    if (text.size() >= field.maxLength) break;
                    if(field.onChar) field.onChar(c);
                    else text.push_back((char)c);
                    break;
                }
            }
        }
        if(!input.text.empty()) textInput.GetComponent<UIText>().buildMesh();
    }
}



bool Slider::calculate() {
    UIElement* ui = TryGetUIElement(object.getID());
    if(!ui) return false;
    
    Vector2 size = ui->getComputedSize();
    Vector2 center = ui->getOffset();
    
    int blobID = object.getChild(0).getID();
    UIElement* blob = TryGetUIElement(blobID);
    if(!blob) return false;

    float x = center.x + (value - 0.5f) * size.x;
    blob->setOffset(Vector2(x, blob->getOffset().y));
    return true;
}



// else if(c == 13) { // enter
//     char command[32];
//     char ip[64];
//     int port;
//     // ClientServerSystem::getInstance().connect("127.0.0.1", 7777);
//     if (sscanf(textViews[0].text.c_str(), "%31s %63s %d", command, ip, &port) == 3) {
//         // if (strcmp(command, "connect") == 0) ClientServerSystem::getInstance().connect(ip, port);
//     }
//     textViews[0].text.clear();
//     continue;
// }
// if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_P)) ClientServerSystem::getInstance().connect("127.0.0.1", 7777);
// if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_O)) ClientServerSystem::getInstance().host(7777);