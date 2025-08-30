#pragma once
#include "Vector2.h"
#include "../IOstructures.h"

class keyBoard
{
public:
	//main
	void create();
	void update();

	//cursor
	void setCursorPos(int x, int y);
	void setCursorPosWithoutMoving();
	
	
	inline unsigned char* 	getInputState() 	{ return keyBoarState; }
	inline unsigned char* 	getOldInputState() 	{ return oldkeyBoarState; }
	inline Vector2 			deltaCursorPos() 	{ return (*windowInput.poses) - (*windowInput.oldPoses); }
	inline Vector2* 		getPos() 			{ return windowInput.poses; }
	inline Vector2* 		getOldPos() 		{ return windowInput.oldPoses; }
	inline Vector2			getCursorPos() 		{ return *windowInput.poses; }
public:
	WindowInputs windowInput;
	
	unsigned char keyBoarState[256];
	unsigned char oldkeyBoarState[256];
};