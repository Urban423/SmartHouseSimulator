#pragma once
#include <umath.h>
#include "Transform.h"

class Input {
public:
	static char mouseScrollDelta;
};

class InputEventSystem: public Component, public InputEventSystemI {
public:
	InputEventSystem(Object obj, bool enabled) { object = obj; enabled = enabled;}
	void Update();
	void setSize(int width, int height);
	void handleMouseClickDown(float x, float y);
	void handleMouseClickUp(float x, float y);
	void handleMouseWheel(char delta);
	void handleKeyDown(int key);
};