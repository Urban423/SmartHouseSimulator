#include "UIManager.h"

UIElement* TryGetUIElement(int objectID) {
    if (ECS::HasComponent<UIImage>(objectID)) return &ECS::GetComponent<UIImage>(objectID);
    if (ECS::HasComponent<UIText>(objectID)) return &ECS::GetComponent<UIText>(objectID);
    if (ECS::HasComponent<UIBox>(objectID)) return &ECS::GetComponent<UIBox>(objectID);

    return nullptr;
}



Vector2 GetAnchorOffset(Anchor anchor, Vector2 viewPortSize) {
    float width = viewPortSize.x / 2;
    float height = viewPortSize.y / 2;
    switch (anchor) {
        case Anchor::TopLeft:     return {-width,   height};
        case Anchor::Top:         return {0.0f,     height};
        case Anchor::TopRight:    return {width,    height};

        case Anchor::Left:        return {-width,   0.0f};
        case Anchor::Center:      return {0,        0};
        case Anchor::Right:       return {width,    0.0f};

        case Anchor::BottomLeft:  return {-width,   -height};
        case Anchor::Bottom:      return {0.0f,     -height};
        case Anchor::BottomRight: return {width,    -height};
    }
    return {0, 0};
}

inline bool calculateAxisSize(int objID, char axis, char layoutDirection, short padding, float& out) {
    const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
    int childrenCount = childrenID.size();
    if(childrenCount == 0) return false;


    int uiElementsCounter = 0;
    float maxAxisSize = 0;
    float newAxisSize = 0;
    for(int i = 0; i < childrenCount; ++i) {
        UIElement* ui = TryGetUIElement(childrenID[i]);
        if(!ui) continue;

        ++uiElementsCounter;
        float childAxisSize = ui->getComputedSize()[axis];
        if(layoutDirection == axis) newAxisSize += childAxisSize;
        else newAxisSize = std::max(newAxisSize, childAxisSize);
    }
    if(axis != layoutDirection) {
        uiElementsCounter = 1;
    }
    newAxisSize += (uiElementsCounter + 1) * padding;
    out = newAxisSize;
    return true;
}

template<typename It>
inline void placeRow(It begin, It end, Vector2 startOffset, float rowSize, Vector2 parentPos, Vector2 parentSize, short padding, char axis, char depth, Align align) {
    Vector2 localOffset = startOffset;

    float available = parentSize[axis] - padding;
    switch (align) {
        case(Align::End): {
            localOffset[axis] += (available - rowSize);
            break;
        }
        case(Align::Center): {
            localOffset[axis] += (available - rowSize) * 0.5f;
            break;
        }

        case(Align::Start): break;
        default: break;
    }
    for (; begin != end; ++begin) {
        UIElement* ui = TryGetUIElement(*begin);
        if (!ui) continue;

        Vector2 childSize = ui->getComputedSize();

        ui->setOffset(
            Vector2(localOffset.x, -localOffset.y)
            + parentPos
            + GetAnchorOffset(Anchor::TopLeft, parentSize)
            - GetAnchorOffset(Anchor::TopLeft, childSize),
            depth);

        localOffset[axis] += childSize[axis] + padding;
    }
}

inline void calculateAxisPos(int objID, char axis, Vector2 parentPos, Vector2 parentSize, short padding, char depth, bool overflow, bool isAbsolute, Align align) {
    const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
    
    Vector2 localOffset(padding, padding);
    Vector2 rowStartOffset;
    rowStartOffset[axis] = padding;
    auto rowBegin = childrenID.begin();
    float rowHeight = 0;
    for (auto it = childrenID.begin(); it != childrenID.end(); ++it) {
        UIElement* ui = TryGetUIElement(*it);
        if(!ui) continue;

        if(isAbsolute) ui->setOffset(parentPos + GetAnchorOffset(ui->anchor, parentSize) - GetAnchorOffset(ui->pivot, ui->getComputedSize()), depth);
        else {
            Vector2 childSize = ui->getComputedSize();
            if (!overflow && localOffset[axis] + childSize[axis] + padding > parentSize[axis]) {
                rowStartOffset[!axis] = localOffset[!axis];
                placeRow(rowBegin, it, rowStartOffset, localOffset[axis], parentPos, parentSize, padding, axis, depth, align);

                rowBegin = it;
                localOffset[axis] = padding;
                localOffset[!axis] += rowHeight + padding;
                rowHeight = 0;
            }
            localOffset[axis] += childSize[axis] + padding;
            rowHeight = std::max(rowHeight, childSize[!axis]);
        }
    }

    rowStartOffset[!axis] = localOffset[!axis];
    if(!isAbsolute) placeRow(rowBegin, childrenID.end(), rowStartOffset, localOffset[axis], parentPos, parentSize, padding, axis, depth, align);
}

inline void calculateAxisGrow(int objID, char axis, Vector2 uiSize, short padding, char layoutDirection) {
    float remainSize = uiSize[axis] - padding;
    char weightSum = 0;
    bool mainAxis = axis == layoutDirection;
    const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
    if(mainAxis) {
        for(auto childID: childrenID) {
            UIElement* ui = TryGetUIElement(childID);
            if(!ui) continue;
            
            if(ui->isFill(axis)) weightSum += ui->weight;
            remainSize -= ui->getComputedSize()[axis];
            remainSize -= padding;
        }
    }
    if(remainSize <= 0) return;
    
    for(auto childID: childrenID) {
        UIElement* ui = TryGetUIElement(childID);
        if(!ui) continue;
        
        if(ui->isFill(axis)) {
            Vector2 size = ui->getComputedSize();
            if (mainAxis) size[axis] += remainSize * ui->weight / weightSum;
            else size[axis] = remainSize - padding;
            ui->setComputedSize(size);
        }
    }
}


void UISystem::Rebuild(Object& root) {
    std::vector<int> bottomToTop;
    std::stack<int> dfs;
    dfs.push(root.getID());

    while(!dfs.empty()) {
        int objID = dfs.top();
        dfs.pop();
        bottomToTop.push_back(objID);
        const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
        for(int i = 0; i < childrenID.size(); ++i) dfs.push(childrenID[i]);
    }


    for(int i = bottomToTop.size() - 1; i >= 0; --i) {
        int objID = bottomToTop[i];
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;

        char direction = ui->direction == Direction::Vertical;
        short padding = ui->padding;
        if (ui->isWidthWrap()) {
            float width;
            if(calculateAxisSize(objID, 0, direction, padding, width)) {
                ui->setComputedWidth(width);
            }
        }
        if (ui->isHeightWrap()) {
            float height;
            if(calculateAxisSize(objID, 1, direction, padding, height)) {
                ui->setComputedHeight(height);
            }
        }
    }

    for(int i = 0; i < bottomToTop.size(); ++i) {
        int objID = bottomToTop[i];
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;
        
        char direction = ui->direction == Direction::Vertical;
        short padding = ui->padding;
        Vector2 uiSize = ui->getComputedSize();
        calculateAxisGrow(objID, 0, uiSize, padding, direction);
        calculateAxisGrow(objID, 1, uiSize, padding, direction);
    }

    for(int i = 0; i < bottomToTop.size(); ++i) {
        int objID = bottomToTop[i];
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;

        char direction = ui->direction == Direction::Vertical;
        short padding = ui->padding;
        char depth = ui->depth + 1;
        Vector2 parentSize = ui->getComputedSize();
        Vector2 parentPos = ui->getOffset();
        calculateAxisPos(objID, direction, parentPos, parentSize, padding, depth, ui->overflow, ui->direction == Direction::Absolute, ui->align);
    }

    Span<Slider> uiSliders = ECS::GetComponents<Slider>();
    for(auto& slider : uiSliders) {
        slider.calculate();
    }
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

    if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton)) focusedInputField = Object();

    CursorType cursor = CursorType::Arrow;
    Span<Button> uiButtons = ECS::GetComponents<Button>();
    for(auto& button : uiButtons) {
        int objID = button.object.getID();
        if (!ECS::isActive(objID)) {
            button.isPressed = false;
            button.hovered = false;
            continue;
        }
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;
        bool inside = ui->contain(mousePos);
        // printf("%f %f(%f)\t", ui->getOffset().x, ui->getComputedSize().x, mousePos.x);  printf(" %f %f(%f)\n", ui->getOffset().y, ui->getComputedSize().y, mousePos.y); break;
    
        if(inside && !button.isPressed && IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton)) {
            button.isPressed = true;
            button.onClickDown();
        }
        else if(button.isPressed && IOSystem::getKeyBoard().GetKeyUp(KeyCode_LeftMouseButton)) { 
            button.isPressed = false;
        }
        button.hovered = inside;
        if(inside) cursor = CursorType::Hand;

        if(ECS::HasComponent<UIImage>(objID)) {
            Color color = button.onExitColor;
            if(button.hovered) color = button.onEnterColor;
            if(button.isPressed) color = button.onPressedColor;
            ECS::GetComponent<UIImage>(objID).color = color;
        }
        // if(inside) return;
    }

    Span<Checkbox> checkboxs = ECS::GetComponents<Checkbox>();
    for(auto& checkbox : checkboxs) {
        int objID = checkbox.object.getID();
        if (!ECS::isActive(objID)) {
            continue;
        }
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;
        bool inside = ui->contain(mousePos);

        if(inside) cursor = CursorType::Hand;
        if(inside && IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton)) {
            checkbox.statement = !checkbox.statement;
            if(checkbox.onClickDown) checkbox.onClickDown();

            Object child = checkbox.object.getChild(0);
            if(child.valid() && child.HasComponent<Active>()) {
                child.GetComponent<Active>().enabled = checkbox.statement;
            }
        }

        // if(inside) return;
    }


    Span<Slider> uiSliders = ECS::GetComponents<Slider>();
    for(auto& slider : uiSliders) {
        int objID = slider.object.getID();
        if (!ECS::isActive(objID)) {
            slider.dragging = false; 
            continue;
        }
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;
        bool insideTrack = ui->contain(mousePos);
        Object blobObject = slider.object.getChild(0);
        UIElement* blobUI = TryGetUIElement(blobObject.getID());
        bool inside = insideTrack || blobUI && blobUI->contain(mousePos);

        if(inside) cursor = CursorType::Hand;
        if (inside && IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton)) slider.dragging = true;

        if (slider.dragging) {
            if (IOSystem::getKeyBoard().GetKey(KeyCode_LeftMouseButton)) {
                Vector2 size = ui->getComputedSize();
                Vector2 center = ui->getOffset();
                float left = center.x - size.x * 0.5f;
                float t = (mousePos.x - (center.x - size.x * 0.5f)) / size.x;

                slider.value = clamp(0.0f, 1.0f, t);
                slider.calculate();
            }
            else {
                slider.dragging = false;
                if (slider.onChangeEnd) slider.onChangeEnd();
            }
        }
        // if(inside) return;
    }


    if(!focusedInputField.valid()) {
        Span<InputField> inputFields = ECS::GetComponents<InputField>();
        for(auto& inputField : inputFields) {
            int objID = inputField.object.getID();
            if(!ECS::isActive(objID)) continue;
            UIElement* ui = TryGetUIElement(objID);
            if(!ui) continue;
            bool inside = ui->contain(mousePos);

            if(inside) cursor = CursorType::Text;
            if(inside && IOSystem::getKeyBoard().GetKeyDown(KeyCode_LeftMouseButton)) {
               focusedInputField = inputField.object;
            }

            // if(inside) return;
        }
    }

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
    
    IOSystem::getPlatform().setCursor(cursor);
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