#pragma once
#include <umath.h>
#include "Transform.h"

class InputEventSystem: public Component, public InputEventSystemI {
public:
	InputEventSystem(Object obj, bool enabled) { object = obj; enabled = enabled;}
	void setSize(int width, int height);
	void handleMouseClickDown(float x, float y);
	void handleMouseClickUp(float x, float y);
	void handleMouseWheel(char delta);
	void handleKeyDown(int key);
};