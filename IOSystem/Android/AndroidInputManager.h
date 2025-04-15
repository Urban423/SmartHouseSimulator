#pragma once
#include "Vector2.h"
#include "../IOstructures.h"

class AndroidInputManager
{
public:
	AndroidInputManager();
	//~AndroidInputManager();
	
	//main
	void create();
	void update();
	
	//keyboard
	unsigned char* getInputState();
	unsigned char* getOldInputState();
	
	//cursor
	Vector2* getPos();
	Vector2* getOldPos();
	void showCursor(bool state);
	void setCursorPos(int x, int y);
	Vector2 getCursorPos();
	Vector2 deltaCursorPos();
private:
	WindowInputs windowInput;
	
	unsigned char keys[256] = {0};
	unsigned char oldKeys[256] = {0};
};