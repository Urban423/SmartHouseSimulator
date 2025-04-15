#include "AndroidInputManager.h"


AndroidInputManager:: AndroidInputManager() {}
//AndroidInputManager::~AndroidInputManager() {}

void AndroidInputManager::create()
{
	
}

void AndroidInputManager::update() 
{
	
}

unsigned char* AndroidInputManager::getInputState()
{
	return keys;
}

unsigned char* AndroidInputManager::getOldInputState()
{
	return oldKeys;
}

Vector2 AndroidInputManager::getCursorPos()
{
	return Vector2(0, 0);
}


void AndroidInputManager::showCursor(bool state)
{
	
}

void  AndroidInputManager::setCursorPos(int x, int y)
{
	
}

Vector2 AndroidInputManager::deltaCursorPos() 
{
	return Vector2(0, 0);
}

Vector2* AndroidInputManager::getPos()
{
	return windowInput.poses;
}

Vector2* AndroidInputManager::getOldPos()
{
	return windowInput.oldPoses;
}