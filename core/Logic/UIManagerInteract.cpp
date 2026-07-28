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
    CursorType cursor = CursorType::Arrow;


    if(clickDown) {
        focusedInputField = Object();
        if(cursorImage.valid()) cursorImage.GetComponent<Active>().enabled = false;
    }

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

        if (ECS::HasComponent<InputField>(objID)) {
            InputField& inputField = ECS::GetComponent<InputField>(objID);
            if (inside) cursor = CursorType::Text;
            
            if(inside && clickDown) {
                if(!cursorImage.valid()) {
                    cursorImage = ECS::createObject();
                    cursorImage.AddComponent<Active>();
                    cursorImage.AddComponent<UIPopup>();
                    cursorImage.AddComponent<UIImage>().setSize(3, 32);
                }

                focusedInputField = inputField.object;

                Object textInput = focusedInputField.getChild(0);
                if(textInput.valid() && textInput.HasComponent<UIText>()) {
                    UIText& uiText = textInput.GetComponent<UIText>();
                    cursorIndex = uiText.getCursorIndex(mousePos);
                    Vector2 cursorPos = uiText.getCursorPos(cursorIndex);

                    cursorBlinkTimer = 0;
                    cursorImage.GetComponent<Active>().enabled = true;
                    cursorImage.GetComponent<UIImage>().setDepth(uiText.depth + 1);
                    cursorImage.GetComponent<UIImage>().setOffset(cursorPos);
                    cursorImage.setParent(focusedInputField);
                }
            }
        }
    }
    IOSystem::getPlatform().setCursor(cursor);



    //inputFieldUpdate
    if(focusedInputField.valid() && focusedInputField.HasComponent<InputField>()) {
        Object textInput = focusedInputField.getChild(0);
        if(!textInput.valid() || !textInput.HasComponent<UIText>()) return;

        InputField& field = focusedInputField.GetComponent<InputField>();

        Input& input = IOSystem::getInput();
        cursorBlinkTimer += Time::realDeltaTime;
        if (cursorBlinkTimer >= cursorBlinkRate) {
            cursorBlinkTimer -= cursorBlinkRate;
            Active& cursorActive = cursorImage.GetComponent<Active>();
            cursorActive.enabled = !cursorActive.enabled;
        }

        std::string& text = textInput.GetComponent<UIText>().text;
        for (char c : input.text) {
            switch (c) {
                case 8: { //backspace
                    if (cursorIndex > 0 && text.size() > 0) {
                        text.erase(cursorIndex - 1, 1);
                        cursorIndex--;
                    }
                    break;
                }

                case 9: { //tab
                    if (field.charFilter && !field.charFilter((char)c)) break;
                    if (text.size() + 4 >= field.maxLength) break;
                    if(field.onChar) field.onChar(c);
                    else {
                        text.insert(cursorIndex, "    ");
                        cursorIndex += 4;
                    }
                    break;
                }

                case 13: { //enter
                    if (field.charFilter && !field.charFilter((char)c)) break;
                    if (text.size() >= field.maxLength) break;
                    if(field.onSubmit) field.onSubmit();
                    else {
                        text.insert(text.begin() + cursorIndex, '\n');
                        cursorIndex++;
                    }
                    break;
                }

                default: {
                    if (field.charFilter && !field.charFilter((char)c)) break;
                    if (text.size() >= field.maxLength) break;
                    if(field.onChar) field.onChar(c);
                    else {
                        text.insert(cursorIndex, 1, (char)c);
                        cursorIndex++;
                    }
                    break;
                }
            }
        }

        bool rebuild = !input.text.empty();
        for(int c : input.keyEvents) {
            if (c == KeyCode_LeftArrow) {
                if (cursorIndex > 0) {
                    rebuild = true;
                    cursorIndex--;
                }
            }

            if (c == KeyCode_RightArrow) {
                if (cursorIndex < text.size()) {
                    rebuild = true;
                    cursorIndex++;
                }
            }

            if (c == KeyCode_Delete) {
                if (cursorIndex < text.size()){
                    rebuild = true;
                    text.erase(cursorIndex, 1);
                }
            }
        }

        if(rebuild)  {
            UIText& uiText = textInput.GetComponent<UIText>();
            uiText.buildMesh();

            UIElement* focusedInputFieldUI = TryGetUIElement(focusedInputField.getID());
            if(focusedInputFieldUI) {
                Vector2 focusedInputFieldSize = focusedInputFieldUI->getComputedSize();
                Rebuild(focusedInputField, focusedInputFieldSize.x, focusedInputFieldSize.y, 1.0f);
            }

            if(uiText.text.size() > 0) {
                cursorBlinkTimer = 0;
                cursorImage.GetComponent<Active>().enabled = true;
                Vector2 cursorPos = uiText.getCursorPos(cursorIndex);
                cursorImage.GetComponent<UIImage>().setOffset(cursorPos);
            }
            else {
                cursorImage.GetComponent<UIImage>().setOffset(uiText.getOffset2());
            }
        }
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