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

inline bool calculateAxisSize(int objID, char axis, char layoutDirection, Vector2 padding, char spacing, float& out) {
    const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
    int childrenCount = childrenID.size();
    if(childrenCount == 0) {
        return false;
    }

    int uiElementsCounter = 0;
    float maxAxisSize = 0;
    float newAxisSize = 0;
    for(auto childID : childrenID) {
        UIElement* ui = TryGetUIElement(childID);
        if(ui->direction == Direction::Absolute || ECS::HasComponent<UIPopup>(childID)) continue;

        ++uiElementsCounter;
        float childAxisSize = ui->getComputedSize()[axis];
        if(layoutDirection == axis) newAxisSize += childAxisSize;
        else newAxisSize = std::max(newAxisSize, childAxisSize);
    }
    if(axis != layoutDirection) {
        uiElementsCounter = 1;
    }
    newAxisSize += padding[0] + padding[1] + spacing * (uiElementsCounter - 1);
    out = newAxisSize;
    return true;
}

inline void calculateAxisGrow(int objID, char axis, Vector2 uiSize, Vector2 padding, char spacing, char layoutDirection) {
    float parentSize = uiSize[axis] - padding[0] - padding[1];
    float remainSize = parentSize;
    float weightSum = 0;
    int counter = 0;
    bool mainAxis = axis == layoutDirection;
    const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
    if(mainAxis) {
        for(auto childID: childrenID) {
            UIElement* ui = TryGetUIElement(childID);
            if(!ui) continue;
            if (ECS::HasComponent<UIPopup>(childID)) continue;

            counter++;
            if(ui->isFill(axis)) weightSum += ui->weight;
            if(!ui->isFill(axis)) remainSize -= ui->getComputedSize()[axis];
        }
        if(counter > 0) remainSize -= spacing * (counter - 1);
    }

    
    for(auto childID: childrenID) {
        UIElement* ui = TryGetUIElement(childID);
        if(!ui) continue;
        if(!ui->isFill(axis)) continue;

        Vector2 size = ui->getComputedSize();
        if (ECS::HasComponent<UIPopup>(childID)) {
            size[axis] = parentSize;
        }
        else {
            if (mainAxis) size[axis] = remainSize * ui->weight / weightSum;
            else size[axis] = remainSize;
        }
        ui->setComputedSize(size);
    }
}

inline void calculateMainAxis(const std::vector<int>& childrenID, char axis, Vector2 parentSize, Vector2 padding, float spacing, bool overflow, UIAlignFlags alignAxisFlags) {
    float parentAxisSize = parentSize[axis];
    float rowSize = 0;
    int count = 0;
    for (auto childID : childrenID) {
        UIElement* ui = TryGetUIElement(childID);
        if(!ui) continue;
        if(ECS::HasComponent<UIPopup>(childID)) continue;

        Vector2 childSize = ui->getComputedSize();
        count++;
        rowSize += childSize[axis];
    }
    rowSize += (count - 1) * spacing;

    float rowOffset = 0;
    if(alignAxisFlags == UIAlignFlags::Start) rowOffset = padding[0];
    else if(alignAxisFlags == UIAlignFlags::Center) rowOffset = padding[0] + (parentAxisSize - rowSize) * 0.5f - padding[1];
    else if(alignAxisFlags == UIAlignFlags::End) rowOffset += (parentAxisSize - rowSize - padding[1]);
    else if (alignAxisFlags == UIAlignFlags::Justify) {
        rowOffset = padding[0];
        if (count > 1) spacing = (parentAxisSize - rowSize - padding[1] - padding[0]) / (count - 1);
    }

    for (auto childID : childrenID) {
        UIElement* ui = TryGetUIElement(childID);
        if(!ui) continue;
        if(ECS::HasComponent<UIPopup>(childID)) continue;

        Vector2 childSize = ui->getComputedSize();
        ui->setOffset(rowOffset, axis);
        rowOffset += childSize[axis] + spacing;
    }
}

inline void calculateCrossAxis(const std::vector<int>& childrenID, char axis, Vector2 parentSize, Vector2 padding, float spacing, bool overflow, UIAlignFlags alignAxisFlags) {
    float parentAxisSize = parentSize[axis];
    float rowSize = 0;
    int count = 0;
    for (auto childID : childrenID) {
        UIElement* ui = TryGetUIElement(childID);
        if(!ui) continue;
        if(ECS::HasComponent<UIPopup>(childID)) continue;

        Vector2 childSize = ui->getComputedSize();
        float offset = 0.0f;
        switch (alignAxisFlags) {
            case UIAlignFlags::Center:
                offset = padding[0] + (parentAxisSize - padding[0] - padding[1] - childSize[axis]) * 0.5f;
                break;

            case UIAlignFlags::End:
                offset = parentAxisSize - padding[1] - childSize[axis];
                break;

            default: offset = padding[0]; break;
        }
        ui->setOffset(offset, axis);
    }
}

void UISystem::Rebuild(Object& root, int newWidth, int newHeight, float uiScale) {
    std::vector<int> bottomToTop;
    std::stack<int> dfs;
    dfs.push(root.getID());

    UIElement* rootUI = TryGetUIElement(root.getID());
    rootUI->setMinSize(newWidth, newHeight);
    rootUI->setComputedSize({newWidth, newHeight});

    while(!dfs.empty()) {
        int objID = dfs.top();
        dfs.pop();
        bottomToTop.push_back(objID);
        const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
        for(int i = 0; i < childrenID.size(); ++i) dfs.push(childrenID[i]);
    }


    for(int i = bottomToTop.size() - 1; i > 0; --i) {
        int objID = bottomToTop[i];
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;

        ui->setComputedWidth(ui->getWidth());
        ui->setComputedHeight(ui->getHeight());
        char direction = ui->direction == Direction::Vertical;
        Vector4 padding = ui->padding;
        float spacing = ui->spacing;
        if(ui->scalable) {
            padding *= uiScale;
            spacing *= uiScale;
        }
        if (ui->HasFlag(ui->widthFlags, UISizeFlags::Wrap)) {
            float width;
            if(calculateAxisSize(objID, 0, direction, {padding[0], padding[1]}, spacing, width)) {
                ui->setComputedWidth(width);
            } 
        }
        if (ui->HasFlag(ui->heightFlags, UISizeFlags::Wrap)) {
            float height;
            if(calculateAxisSize(objID, 1, direction, {padding[2], padding[3]}, spacing, height)) {
                ui->setComputedHeight(height);
            } 
        }
    }

    for(int i = 0; i < bottomToTop.size(); ++i) {
        int objID = bottomToTop[i];
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;
        
        char direction = ui->direction == Direction::Vertical;
        Vector4 padding = ui->padding;
        float spacing = ui->spacing;
        if(ui->scalable) {
            padding *= uiScale;
            spacing *= uiScale;
        }
        Vector2 uiSize = ui->getComputedSize();
        calculateAxisGrow(objID, 0, uiSize, {padding[0], padding[1]}, spacing, direction);
        calculateAxisGrow(objID, 1, uiSize, {padding[2], padding[3]}, spacing, direction);
    }

    for(int i = 0; i < bottomToTop.size(); ++i) {
        int objID = bottomToTop[i];
        UIElement* ui = TryGetUIElement(objID);
        if(!ui) continue;

        const std::vector<int>& childrenID = ECS::GetHierarchy().getChildren(objID);
        char mainDirection = ui->direction == Direction::Vertical;
        char crossDirection = 1 - mainDirection;
        Vector4 padding = ui->padding;
        float spacing = ui->spacing;
        if(ui->scalable) {
            padding *= uiScale;
            spacing *= uiScale;
        }
        char depth = ui->depth + 1;
        Vector2 parentSize = ui->getComputedSize();
        Vector2 parentPos = ui->getOffset();
        UIAlignFlags flags[2] = { ui->alignX, ui->alignY };
        calculateMainAxis(childrenID,   mainDirection, parentSize, {padding[mainDirection * 2],  padding[mainDirection * 2 + 1]},  spacing, ui->overflow, flags[mainDirection] );
        calculateCrossAxis(childrenID, crossDirection, parentSize, {padding[crossDirection * 2], padding[crossDirection * 2 + 1]}, spacing, ui->overflow, flags[crossDirection]);

        for(auto childID : childrenID) {
            UIElement* childUI = TryGetUIElement(childID);
            if(!childUI) continue;


            Vector2 childSize = childUI->getComputedSize();
            Vector2 offset = childUI->getOffset();
            if(ECS::HasComponent<UIPopup>(childID) || ui->direction == Direction::Absolute) {
                offset = parentPos + GetAnchorOffset(childUI->anchor, parentSize) - GetAnchorOffset(childUI->pivot, childSize);
            }
            else {
                offset.y *= -1;
                offset += parentPos + GetAnchorOffset(Anchor::TopLeft, parentSize) - GetAnchorOffset(Anchor::TopLeft, childSize);
            }
            childUI->setOffset(offset);
            childUI->setDepth(depth++);
        }
    }

    Span<Slider> uiSliders = ECS::GetComponents<Slider>();
    for(auto& slider : uiSliders) {
        slider.calculate();
    }

    if(focusedInputField.valid() && focusedInputField.HasComponent<InputField>()) {
        Object textInput = focusedInputField.getChild(0);
        if(!textInput.valid() || !textInput.HasComponent<UIText>()) return;
        
        UIText& uiText = textInput.GetComponent<UIText>();
        Vector2 cursorPos = uiText.getCursorPos(cursorIndex);
        cursorImage.GetComponent<UIImage>().setOffset(cursorPos);
    }
}