#include "InputEventSystem.h"
#include "ECS.h"
#include "Transform.h"
#include "CameraControlSystem.h"
#include "ScreenLogic.h"
#include <stdio.h>

void InputEventSystem::setSize(int width, int height)
{
	Span<ScreenLogic> screenLogics = ECS::GetComponents<ScreenLogic>();
	for (int cameraIndex = 0; cameraIndex < screenLogics.size(); cameraIndex++)
	{
		screenLogics[cameraIndex].resizeWindows(width, height);
	}
}


void InputEventSystem::handleMouseClickDown(float x, float y)
{
	
}

void InputEventSystem::handleMouseClickUp(float x, float y)
{
}

void InputEventSystem::handleMouseWheel(char delta)
{
	if (delta != 120 && delta != -120) return;
	// Input::mouseScrollDelta = delta;
}

void InputEventSystem::handleKeyDown(int key)
{
	wprintf(L"%c", key);
}

void InputEventSystem::Update()
{
	// Input::mouseScrollDelta = 0;
}