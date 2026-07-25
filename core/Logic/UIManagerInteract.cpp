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


    Span<Interactable> interactables = ECS::GetComponents<Interactable>();
    for (auto& interactable : interactables) {
        bool inside = updateInteractable(interactable, mousePos);
        if (inside) cursor = CursorType::Hand;

        int objID = interactable.object.getID();
        int parentID = ECS::GetHierarchy().getParent(objID);



        if (ECS::HasComponent<Slider>(objID) || parentID != -1 && ECS::HasComponent<Slider>(parentID)) {
            Slider* slider = nullptr;
            if(ECS::HasComponent<Slider>(objID)) {
                slider = &ECS::GetComponent<Slider>(objID);
            } else slider = &ECS::GetComponent<Slider>(parentID);
            
            if (inside && clickDown) slider->dragging = true;

            if(slider->dragging) {
                if (click) {
                    UIElement* ui = TryGetUIElement(slider->object.getID());
                    Vector2 size = ui->getComputedSize();
                    Vector2 center = ui->getOffset();
                    // float t = (mousePos.x - (center.x - size.x * 0.5f)) / size.x;

                    UIElement* blob = TryGetUIElement(slider->object.getChild(0).getID());
                    Vector2 blobSize = blob->getComputedSize();
                    float left  = center.x - size.x * 0.5f + blobSize.x * 0.5f;
                    float right = center.x + size.x * 0.5f - blobSize.x * 0.5f;
                    float t = (mousePos.x - left) / (right - left);

                    t = clamp(0.0f, 1.0f, t);
                    slider->value = slider->minValue + t * (slider->maxValue - slider->minValue);
                    if (slider->fixedPositions > 1) {
                        float step = (slider->maxValue - slider->minValue) / (slider->fixedPositions - 1);
                        slider->value = slider->minValue + std::round((slider->value - slider->minValue) / step) * step;
                    }
                    if (slider->onDrag) slider->onDrag(slider->value);
                    slider->calculate();
                }
                else {
                    slider->dragging = false;
                    if (slider->onDragEnd) slider->onDragEnd(slider->value);
                }
            }
        }

        if (ECS::HasComponent<Toggle>(objID)) {
            Toggle& toggle = ECS::GetComponent<Toggle>(objID);
            if (interactable.pressed && clickDown) {
                toggle.value = !toggle.value;
                if (toggle.onValueChanged) toggle.onValueChanged(toggle.value);
                Object child = toggle.object.getChild(0); 
                if(child.valid() && child.HasComponent<Active>()) { 
                    child.GetComponent<Active>().enabled = toggle.value; 
                }
            }
        }

        if(interactable.pressed && clickDown && parentID != -1 && ECS::HasComponent<RadioGroup>(parentID)) {
            RadioGroup& group = ECS::GetComponent<RadioGroup>(parentID);
            auto& children = ECS::GetHierarchy().getChildren(parentID);
            int selectedIndex = 0;
            for(auto child: children) {
                if(child == objID) {
                    group.selected = selectedIndex;
                    if(group.onValueSelect) group.onValueSelect(selectedIndex);
                }
                if(ECS::HasComponent<Toggle>(child)) {
                    ECS::GetComponent<Toggle>(child).value = child == objID;
                }
                selectedIndex++;
            }
        }

        if (ECS::HasComponent<SelectableStyle>(objID)) {
            SelectableStyle& style = ECS::GetComponent<SelectableStyle>(objID);
            Color color = style.normal;
            if (ECS::HasComponent<Toggle>(objID)) {
                Toggle& toggle = ECS::GetComponent<Toggle>(objID);
                if (toggle.value) color = style.selected;
                else if (interactable.pressed) color = style.pressed;
                else if (interactable.hovered) color = style.hover;
            }
            else {
                if (interactable.pressed) color = style.pressed;
                else if (interactable.hovered) color = style.hover;
            }
            ECS::GetComponent<UIImage>(objID).color = color;
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

    float t = (value - minValue) / (maxValue - minValue);
    // float x = center.x + (t - 0.5f) * size.x;

    Vector2 blobSize = blob->getComputedSize();
    float left  = center.x - size.x * 0.5f + blobSize.x * 0.5f;
    float right = center.x + size.x * 0.5f - blobSize.x * 0.5f;
    float x = left + (right - left) * t;
    blob->setOffset({x, blob->getOffset().y});
    return true;
}