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
        if(!ui || ui->direction == Direction::Absolute) continue;

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

template<typename It>
inline void placeRow(It begin, It end, Vector2 startOffset, float rowSize, Vector2 parentPos, Vector2 parentSize, short padding, char axis, char depth, UIAlignFlags alignXFlags, UIAlignFlags alignYFlags) {
    Vector2 localOffset = startOffset;
    float spacing = padding;
    float available = parentSize[axis] - padding;
    float rowHeight = parentSize[!axis] - 2 * padding;

    UIAlignFlags mainFlags  = (axis == 0) ? alignXFlags : alignYFlags;
    UIAlignFlags crossFlags = (axis == 0) ? alignYFlags : alignXFlags;
    if(mainFlags == UIAlignFlags::Center) localOffset[axis] += (available - rowSize) * 0.5f;
    else if(mainFlags == UIAlignFlags::End) localOffset[axis] += (available - rowSize);
    else if (mainFlags == UIAlignFlags::Justify) {
        size_t count = std::distance(begin, end);
        if (count > 1) spacing = (parentSize[axis] - rowSize) / (count - 1);
    }

    for (; begin != end; ++begin) {
        UIElement* ui = TryGetUIElement(*begin);
        if (!ui) continue;

        Vector2 childSize = ui->getComputedSize();

        float crossOffset = startOffset[!axis];
        if (crossFlags == UIAlignFlags::Center) crossOffset += (rowHeight - childSize[!axis]) * 0.5f;
        else if (crossFlags == UIAlignFlags::End) crossOffset += rowHeight - childSize[!axis];

        Vector2 pos = localOffset;
        pos[!axis] = crossOffset;
        ui->setOffset(
            Vector2(pos.x, -pos.y)
            + parentPos
            + GetAnchorOffset(Anchor::TopLeft, parentSize)
            - GetAnchorOffset(Anchor::TopLeft, childSize),
            depth);

        localOffset[axis] += childSize[axis] + spacing;
    }
}

inline void calculateAxisPos(int objID, char axis, Vector2 parentPos, Vector2 parentSize, short padding, char depth, bool overflow, bool isAbsolute, UIAlignFlags alignXFlags, UIAlignFlags alignYFlags) {
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
                placeRow(rowBegin, it, rowStartOffset, localOffset[axis], parentPos, parentSize, padding, axis, depth, alignXFlags, alignYFlags);

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
    if(!isAbsolute) placeRow(rowBegin, childrenID.end(), rowStartOffset, localOffset[axis], parentPos, parentSize, padding, axis, depth, alignXFlags, alignYFlags);
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
        if (ui->HasFlag(ui->widthFlags, UISizeFlags::Wrap)) {
            float width;
            if(calculateAxisSize(objID, 0, direction, padding, width)) {
                ui->setComputedWidth(width);
            }
        }
        if (ui->HasFlag(ui->heightFlags, UISizeFlags::Wrap)) {
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
        calculateAxisPos(objID, direction, parentPos, parentSize, padding, depth, ui->overflow, ui->direction == Direction::Absolute, ui->alignX, ui->alignY);
    }

    Span<Slider> uiSliders = ECS::GetComponents<Slider>();
    for(auto& slider : uiSliders) {
        slider.calculate();
    }
}