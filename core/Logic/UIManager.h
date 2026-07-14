#pragma once
#include "Transform.h"
#include "ECS.h"
#include "DirtyValue.h"

enum class Anchor : uint8_t {
    TopLeft,
    Top,
    TopRight,

    Left,
    Center,
    Right,

    BottomLeft,
    Bottom,
    BottomRight
};

enum class Direction : uint8_t {
    Horizontal,
    Vertical,
    Absolute
};

enum class Align : uint8_t {
    Start,
    Center,
    End,
    Stretch
};

enum class UISizeFlags : uint8_t {
    SizeFixed = 0,
    SizeFill  = 1 << 0,
    SizeWrap  = 1 << 1
};

class UISystem;
struct UIElement {
protected:
    UISizeFlags widthFlags = UISizeFlags::SizeFixed;
    UISizeFlags heightFlags = UISizeFlags::SizeFixed;
    float width = 200;
    float height = 200;

    float computedWidth = 200;
    float computedHeight = 200;

    Vector2 offset;
public:
    static constexpr float Fill = -1.0f;
    static constexpr float Wrap = -2.0f;

    short padding = 0;
	char layout = 0;
    Anchor anchor = Anchor::Center;
    Anchor pivot = Anchor::Center;
    Align align = Align::Start;
    Direction direction = Direction::Horizontal;
    bool overflow = true;
    char depth = 0;
    char weight = 1;


    void setWidth(float value)  {
        widthFlags = UISizeFlags::SizeFixed;
        if (value < 0) {
            int mode = static_cast<int>(-value);
            widthFlags = static_cast<UISizeFlags>(mode);
            width = 0;
        }
        else width = value;
        computedWidth = width;
    }
    void setHeight(float value)  {
        heightFlags = UISizeFlags::SizeFixed;
        if (value < 0) {
            int mode = static_cast<int>(-value);
            heightFlags = static_cast<UISizeFlags>(mode);
            height = 0;
        }
        else height = value;
        computedHeight = height;
    }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    bool isWidthFill() const { return (char)widthFlags & (char)UISizeFlags::SizeFill; }
    bool isWidthWrap() const { return (char)widthFlags & (char)UISizeFlags::SizeWrap; }
    bool isHeightFill() const {  return (char)heightFlags & (char)UISizeFlags::SizeFill; }
    bool isHeightWrap() const { return (char)heightFlags & (char)UISizeFlags::SizeWrap; }
    inline bool isFill(char axis) { return axis == 0 ? isWidthFill() : isHeightFill(); }

    inline void setSize(float newWidth, float newHeight) { setWidth(newWidth); setHeight(newHeight); }
    inline void setComputedWidth(float value) { computedWidth = value; };
    inline void setComputedHeight(float value) { computedHeight = value; };
    inline void setComputedSize(Vector2 newSize) { computedWidth = newSize.x; computedHeight = newSize.y; }
    inline Vector2 getComputedSize() { return { computedWidth, computedHeight }; }

    inline void setOffset(Vector2 newOffset) { offset = newOffset; } 
    inline void setOffset(Vector2 newOffset, char newDepth) { offset = newOffset; depth = newDepth; } 
    inline Vector3 getOffset() { return { offset.x, offset.y, -static_cast<float>(depth) }; }
    inline bool contain(Vector2 point) {  
        Vector2 half = { computedWidth * 0.5f, computedHeight * 0.5f };
        return point.x >= offset.x - half.x &&
               point.x <= offset.x + half.x &&
               point.y >= offset.y - half.y &&
               point.y <= offset.y + half.y;
    }
};

struct UIBox : public Component, public UIElement {};

struct UIImage : public Component, public UIElement {
    int texture = 0;
    Color color;
};

struct UIText : public Component, public UIElement {
private:
	Mesh mesh;
	bool dirty = true;
public:
    float fontSize = 32;
    std::string text;
	Color color = 0;

	inline void buildMesh() { 
        Vector2 newSize = mesh.calculateAndRebuildTextMesh(text, fontSize, 0, 0);
        setSize(newSize.x, newSize.y);
    }
	inline int getId() { return mesh.id; }
};

struct InputField : public Component {
    size_t cursor = 0;
    size_t maxLength = SIZE_MAX;

    std::function<void(char)> onChar = nullptr;
    std::function<bool(char)> charFilter = nullptr;
    std::function<void()> onSubmit = nullptr;
};

struct Slider : public Component {
    float value = 0.5f;
    bool dragging = false;
    std::function<void()> onChangeEnd = nullptr;

    bool calculate();
};

struct Checkbox : public Component {
    bool statement = true;

    std::function<void()> onClickDown = nullptr;
};

struct Button : public Component {
public:
    Color onEnterColor = Color(255, 255, 0);
    Color onPressedColor = Color(100, 100, 100);
    Color onExitColor = Color(255, 255, 255);
    bool hovered = false;
    bool isPressed = false;

    std::function<void()> onClickDown = nullptr;
};


class UISystem {
public:
    void Rebuild(Object& root);
    void Update();

    static UISystem& getInstance() {
        static UISystem uiSystem;
        return uiSystem;
    }
private:
    Object focusedInputField;
};