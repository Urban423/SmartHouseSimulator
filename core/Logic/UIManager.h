#pragma once
#include "Transform.h"
#include "ECS.h"
#include "DirtyValue.h"
#include "MeshManager.h"

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

enum class UISizeFlags : uint8_t {
    Fixed   = 0,
    Fill   = 1 << 0,
    Wrap    = 1 << 1,
};

enum class UIAlignFlags : uint8_t {
    Start,
    Center,
    End,
    Justify 
};

inline UISizeFlags operator|(UISizeFlags a, UISizeFlags b) { return static_cast<UISizeFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
inline UISizeFlags operator&(UISizeFlags a, UISizeFlags b) { return static_cast<UISizeFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }



class UISystem;
struct UIElement {
protected:
    float width = 200;
    float height = 200;

    float computedWidth = width;
    float computedHeight = height;

    Vector2 offset;
public:
    short padding = 0;
	char layout = 0;
    Anchor anchor = Anchor::Center;
    Anchor pivot = Anchor::Center; 
    UISizeFlags widthFlags = UISizeFlags::Wrap | UISizeFlags::Fill;
    UISizeFlags heightFlags = UISizeFlags::Wrap | UISizeFlags::Fill;
    UIAlignFlags alignX = UIAlignFlags::Start;
    UIAlignFlags alignY = UIAlignFlags::Start;
    Direction direction = Direction::Horizontal;
    bool overflow = true;
    char depth = 0;
    char weight = 1;


    float getWidth() const { return width; }
    float getHeight() const { return height; }
    inline bool HasFlag(UISizeFlags value, UISizeFlags flag) { return static_cast<uint8_t>(value) & static_cast<uint8_t>(flag); }
    inline bool isFill(char axis) { return axis == 0 ? HasFlag(widthFlags, UISizeFlags::Fill) : HasFlag(heightFlags, UISizeFlags::Fill); }

    void setWidth(float value) {
        widthFlags = UISizeFlags::Fixed;
        width = value;
        computedWidth = value;
    }
    void setHeight(float value) {
        heightFlags = UISizeFlags::Fixed;
        height = value;
        computedHeight = value;
    }
    void setWidth(UISizeFlags flags) { widthFlags = flags; }
    void setHeight(UISizeFlags flags) { heightFlags = flags; }

    template<typename W, typename H>
    void setSize(W width, H height) {
        setWidth(width);
        setHeight(height);
    }

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

struct UIPopup : public Component {};

struct UIImage : public Component, public UIElement {
    int texture = 0;
    Color color;
};

struct UIText : public Component, public UIElement {
public:
	int meshID = -1;
    float fontSize = 32;
    std::string text;
	Color color = 0;

    inline void copyFrom(const UIText& other) {
        Component oldComponent = static_cast<Component&>(*this);
        *this = other;
        object = oldComponent.object;
    }
	inline void buildMesh() {
        if(meshID == -1) {
            Mesh mesh;
            meshID = MeshManager::addMesh(mesh);
        }
        Vector2 newSize = MeshManager::getMeshByID(meshID).calculateAndRebuildTextMesh(text, fontSize, 0, 0);
        setSize(newSize.x, newSize.y);
    }
	inline int getId() { return meshID; }
};




struct Interactable : public Component {
    bool hovered = false;
    bool pressed = false;

    std::function<void()> onClickDown = nullptr;
    std::function<void()> onClickUp = nullptr;
    std::function<void()> onHoverEnter = nullptr;
    std::function<void()> onHoverExit = nullptr;
};

struct SelectableStyle : public Component {
    Color normal  = Color(255, 255, 255);
    Color hover   = Color(255, 255,   0);
    Color pressed = Color(100, 100, 100);
    Color selected = Color(255, 0, 255);
};

struct Toggle : public Component {
    bool value = false;
    std::function<void(bool)> onValueChanged = nullptr;
};

struct RadioGroup: public Component {
    int selected = 0;
    std::function<void(int)> onValueSelect = nullptr;
};

struct Slider : public Interactable {
    float value = 0.5f;
    bool dragging = false;

    std::function<void(float)> onDragEnd = nullptr;
    bool calculate();
};

struct InputField : public Interactable {
    size_t cursor = 0;
    size_t maxLength = SIZE_MAX;

    std::function<void(char)> onChar = nullptr;
    std::function<bool(char)> charFilter = nullptr;
    std::function<void()> onSubmit = nullptr;
};




UIElement* TryGetUIElement(int objectID);
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