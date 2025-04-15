#include "KeyBoard.h"
#include <Windows.h>

void keyBoard::create()
{
	windowInput.number 		= 1;
	windowInput.states 		= new unsigned char(0);
	windowInput.poses  		= new Vector2(0, 0);
	windowInput.oldPoses  	= new Vector2(0, 0);
	
	
	if(GetKeyboardState(keyBoarState) == 0) { }
	
	unsigned char* ptrA = keyBoarState;
	unsigned char* ptrB = oldkeyBoarState;
	for(unsigned int i = 0; i < 256; i++)
	{
		(*ptrB) = (*ptrA);
		ptrA++;
		ptrB++;
	}
}

void keyBoard::update()
{
	std::swap(keyBoarState, oldkeyBoarState);
	if(GetKeyboardState(keyBoarState) == 0) { }
	
	POINT p;
	GetCursorPos(&p);
	*(windowInput.states)  	= *(keyBoarState + 1) & 0x80;
	*(windowInput.oldPoses) = *(windowInput.poses);
	*(windowInput.poses)    = Vector2(p.x, p.y);
}




void keyBoard::setCursorPos(int x, int y)
{
	*(windowInput.poses) = Vector2(x, y);
	SetCursorPos(x, y);
}

void keyBoard::setCursorPosWithoutMoving()
{
	POINT p;
	GetCursorPos(&p);
	*(windowInput.poses)    = Vector2(p.x, p.y);
}