#include "InputEventSystem.h"
#include "ECS.h"
#include "Transform.h"
#include "CameraControlSystem.h"
#include "ScreenLogic.h"
#include <stdio.h>

void InputEventSystem::setSize(int width, int height)
{
	std::pair<ScreenLogic *, int> s2 = ECS::GetComponents<ScreenLogic>();
	for (int cameraIndex = 0; cameraIndex < s2.second; cameraIndex++)
	{
		s2.first[cameraIndex].resizeWindows(width, height);
	}
}

bool checkCollision(Vector3 center, Vector3 scale, Vector3 pos)
{
	if ((pos.x < center.x - scale.x) || (pos.x > center.x + scale.x))
	{
		return false;
	}
	if ((pos.y < center.y - scale.y) || (pos.y > center.y + scale.y))
	{
		return false;
	}
	return true;
}

void InputEventSystem::handleMouseClickDown(float x, float y)
{
	auto [renderViews, size] = ECS::GetComponents<RenderView>();
	for (int i = size - 1; i >= 0; i--)
	{
		if (checkCollision(renderViews[i].object.transform.position, renderViews[i].object.transform.scale, Vector3(x, y, 0)))
		{
			if (renderViews[i].object.HasComponent<Button>())
			{
				Button &button = renderViews[i].object.GetComponent<Button>();
				button.pressed = true;
				if (button.onMouseDown)
				{
					button.onMouseDown();
				}
			}
			break;
		}
	}
}

void InputEventSystem::handleMouseClickUp(float x, float y)
{
	auto [renderViews, size] = ECS::GetComponents<RenderView>();
	for (int i = size - 1; i >= 0; i--)
	{
		if (checkCollision(renderViews[i].object.transform.position, renderViews[i].object.transform.scale, Vector3(x, y, 0)))
		{
			if (renderViews[i].object.HasComponent<Button>())
			{
				Button &button = renderViews[i].object.GetComponent<Button>();
				button.pressed = false;
				if (button.onMouseUp)
				{
					button.onMouseUp();
				}
			}
			break;
		}
	}
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