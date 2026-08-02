#pragma once
#include "Rect.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "IOstructures.h"
#include "FBXfile.h"
#include "umath.h"
#include "Mesh.h"
#include "KeyCodes.h"
#include "TTFAtlas.h"
#include <math.h>
#include <vector>
#include <mutex>
#include <thread>

enum class CursorType {
    UNIDENTIFIED,
    Arrow,
    Hand,
    Text,
    None
};

struct Input {
	std::vector<int> text;
	std::vector<int> keyEvents;
	Vector2 movement = Vector2(0, 0);
	Vector2 pointerPosition = Vector2(0, 0);
	Vector2 pointerDelta = Vector2(0, 0);
	float scroll = 0.0f;
	bool pointerPressed = false;
	bool pointerReleased = false;
	bool pointerHold = false;
	bool jumpPressed = false;
	bool sprint = false;
	bool pausePressed = false;
	bool sitPressed = false;
	bool actionPressed = false;
};

struct NetInput {
	int netID;
	Quaternion rotation;
    Vector2 movement;
    Vector2 pointerPosition;
    Vector2 pointerDelta;
    float scroll;

    bool pointerPressed;
    bool pointerReleased;
    bool pointerHold;

    bool jumpPressed;
    bool sprint;
    bool pausePressed;
    bool sitPressed;
    bool actionPressed;
};



inline NetInput ToNetInput(const Input& input, Quaternion rotation) {
    NetInput net{};

	net.rotation = rotation;
    net.movement = input.movement;
    net.pointerPosition = input.pointerPosition;
    net.pointerDelta = input.pointerDelta;
    net.scroll = input.scroll;

    net.pointerPressed = input.pointerPressed;
    net.pointerReleased = input.pointerReleased;
    net.pointerHold = input.pointerHold;

    net.jumpPressed = input.jumpPressed;
    net.sprint = input.sprint;
    net.pausePressed = input.pausePressed;
    net.sitPressed = input.sitPressed;
    net.actionPressed = input.actionPressed;

    return net;
}

inline Input ToInput(const NetInput& net) {
    Input input{};

    input.movement = net.movement;
    input.pointerPosition = net.pointerPosition;
    input.pointerDelta = net.pointerDelta;
    input.scroll = net.scroll;

    input.pointerPressed = net.pointerPressed;
    input.pointerReleased = net.pointerReleased;
    input.pointerHold = net.pointerHold;

    input.jumpPressed = net.jumpPressed;
    input.sprint = net.sprint;
    input.pausePressed = net.pausePressed;
    input.sitPressed = net.sitPressed;
    input.actionPressed = net.actionPressed;

    return input;
}


class IKeyBoard {
public:
	virtual void getKeyboardState(unsigned char* keyboard) = 0;
public:
	inline bool GetKey(unsigned char key) 		{ return keyBoardState[key] & 0x80; }
	inline bool GetKeyDown(unsigned char key) 	{ return (keyBoardState[key] & 0x80) && (oldKeyBoardState[key] & 0x80) != 0x80; }
	inline bool GetKeyUp(unsigned char key) 	{ return (keyBoardState[key] & 0x80) != 0x80 && (oldKeyBoardState[key] & 0x80); }

	void create() {
		getKeyboardState(keyBoardState);
		memcpy(oldKeyBoardState, keyBoardState, 256);
	}
	void update() {
		std::swap(keyBoardState, oldKeyBoardState);
		getKeyboardState(keyBoardState);
	}
	void updateInput(Input& input) {
		Vector2 movement;
		if(GetKey(KeyCode_W)) {
			movement.y = 1;
		} 
		else if(GetKey(KeyCode_S)) {
			movement.y = -1;
		}
		if(GetKey(KeyCode_A)) {
			movement.x = -1;
		} 
		else if(GetKey(KeyCode_D)) {
			movement.x = 1;
		}
		input.movement = movement.normalized();
		if(GetKeyDown(KeyCode_Escape) || GetKeyDown((KeyCode_Windows))){
			input.pausePressed = true;
		}
		if(GetKey(KeyCode_Left_Shift)) {
			input.sprint = true;
		}
		if(GetKeyDown(KeyCode_E)) {
			input.actionPressed = true;
		}
		if(GetKey(KeyCode_Space)) {
			input.jumpPressed = true;
		}
		if(GetKey(KeyCode_Left_CTRL)) {
			input.sitPressed = true;
		}
		if(GetKeyDown(KeyCode_LeftMouseButton)) {
			input.pointerPressed = true;
		} else if(GetKeyUp(KeyCode_LeftMouseButton)) {
			input.pointerReleased = true;
		}
		if(GetKey(KeyCode_LeftMouseButton)) {
			input.pointerHold = true;
		}
	}
private:
	unsigned char keyBoardState[256];
	unsigned char oldKeyBoardState[256];
};

class IWindow {
public:
	virtual void create(const char* windowName, int width, int height, bool fullscreen, bool vsync) = 0;
	virtual void close() = 0;

	virtual Rect getInnerSize() = 0;
	virtual std::pair<int, int> getScreenSize() = 0;
	virtual void setFullscreen(const bool state) = 0;
	virtual void setSize(int width, int height) = 0;
	virtual void setPosition(int x, int y)  = 0;
	
    virtual Vector2 screenToClient(Vector2 pos) = 0;
	virtual bool focus() = 0;
	virtual void swapBuffers() = 0;
	virtual void setVSync(const bool vsync) = 0;
	virtual bool getVSync() = 0;
	virtual bool isRunning() = 0;
	virtual void onDestroy() = 0;
};


class IPlatform {
public:
	virtual void create() = 0;
	virtual void update() = 0;
	virtual double getTime() = 0;
    virtual Vector2 getCursorPosition() = 0;
	virtual Vector2 getDPI() = 0;
	virtual void setCursorPosition(int x, int y) = 0;
    virtual void showCursor(const bool show) = 0;
    virtual void setCursor(const CursorType cursorType) = 0;
	virtual void getSupportedResolutions(std::vector<std::pair<int, int>>& out) = 0;
public:
	void init() {
		dpi = getDPI();
	}
	Vector2 DPI() { return dpi; }
	void addText(int c) {
        textBuffer.push_back(c);
    }
	void getText(std::vector<int>& out) {
        out.clear();
        out.swap(textBuffer);
    }
    void addKeyEvent(int k) {
        keyEventQueue.push_back(k);
    }
	void getKeyEvents(std::vector<int>& out) {
        out.clear();
        out.swap(keyEventQueue);
    }
private:
    std::vector<int> textBuffer;
    std::vector<int> keyEventQueue;
	Vector2 dpi;
};

class IOSystem {
public:
	void create();
	void update();

	static std::vector<Mesh> readFBX(const char* filename);
	static bool readTTF(TTFAtlas& out, const char* filename);
	static bool readBMP(TextureStruct& out, const char* filename);
	static bool readPNG(TextureStruct& out, const char* filename);
	static bool readImage(TextureStruct& out, const char* filename) {
		if(readPNG(out, filename)) return true;
		if(readBMP(out, filename)) return true;
		return false;
	}

	inline static IOSystem& getInstance() {
		static IOSystem iosystem;
		return iosystem;
	}
	inline static Input& getInput() { return getInstance().input; }
	inline static IWindow& addWindow(IWindow* w) { getInstance().windows.push_back(w); return *w; }
    inline static IKeyBoard& addkeyBorad(IKeyBoard* keyboard) { getInstance().keyboards.push_back(keyboard); return *keyboard; }
    inline static IPlatform& addPlatform(IPlatform* p) { getInstance().platform = p; return *p;}
	inline static IKeyBoard& getKeyBoard(int index = 0)  { return *getInstance().keyboards[index]; };
	inline static IWindow& getWindow(int index = 0) { return *getInstance().windows[index]; };
	inline static IPlatform& getPlatform() { return *getInstance().platform; };
	inline static std::vector<std::pair<int, int>>& getSupportedResolutions() { return getInstance().supportedResolutions; }
	inline static void onWindowResizeFinished() { getInstance().resized = true; }
	inline static bool getWindowResizeFinished() { 
		if(getInstance().resized) {
			getInstance().resized = false; 
			return true; 
		}
		return false;
	}
	inline static std::pair<int, int> getWindowSize() { 
		Rect rect = getInstance().windows[0]->getInnerSize();
		return {rect.right - rect.left, rect.bottom - rect.top};
	}
	inline static void lockMouse(bool lock) { getInstance().lockPointers = lock; }

private:
	std::vector<std::pair<int, int>> supportedResolutions;

	std::vector<IKeyBoard*> keyboards;
	std::vector<IWindow*> windows;
	IPlatform* platform;
	Input input;
	bool resized = true;
	bool lockPointers = false;
};



	// inline static Vector2 mousePositionDelta() { return getInstance().platform->getCursorPosition() - getInstance().platform->getOldCursorPos(); }
	// inline static Vector2 getMousePos() { return getInstance().platform->getCursorPosition(); }
	// inline static Rect getWindowInnerSize() { return getInstance().windows[0]->getInnerSize(); }
	// inline static void setMouseCenter() {
	// 	if(!getInstance().windows[0]->focus()) return;
	// 	Rect rect = getInstance().windows[0]->getInnerSize();
	// 	getInstance().platform->setCursorPosition((int)(rect.right - rect.left) / 2,  (int)(rect.bottom - rect.top) / 2);
	// };


// struct WindowSnapshot
// {
//     int width = 0;
//     int height = 0;

//     bool resized = false;

//     bool running = true;
// };
	// static void startWindowThread() {
	// 	getInstance().windowThread = std::thread([]() {
	// 		getInstance().windowLoop();
	// 	});
	// }
	// static void stopWindowThread() {
	// 	if(getInstance().windowThread.joinable()) getInstance().windowThread.join();
	// }
// private:
//     void windowLoop();
    // std::thread windowThread;
	// WindowSnapshot windowSnapshot;
	// std::mutex mutex;
	// std::atomic<bool> running = true;