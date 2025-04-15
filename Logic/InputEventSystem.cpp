#include "InputEventSystem.h"
#include "ECS.h"
#include "Transform.h"
#include <stdio.h>

void InputEventSystem::setSize(int width, int height) {
	std::pair<Camera*, int> s1 = ECS::GetComponents<Camera>(0);
	Camera* camera = s1.first;
	for(int cameraIndex = 0; cameraIndex < s1.second; cameraIndex++) {
		Camera& camData = camera[cameraIndex];
		camData.left = 0;
		camData.right = width;
		camData.top = 0;
		camData.bottom = height;
		camData.projection.setIdentity();
		camData.projection.setOrthoLH((float)width * camData.focalLength, (float)height * camData.focalLength, -4, 4);
		//camData.projection.setPerspectiveFovLH(1.17f, size.x / size.y, 0.01f, 1111);
	}
	//renderManager->Render();
}

void InputEventSystem::handleMouseClickDown(float x, float y) {
	printf("mouse pos: %f %f\n", x, y);
}

void InputEventSystem::handleMouseClickUp(float x, float y) {
	printf("mouse pos: %f %f\n", x, y);
}

void InputEventSystem::handleMouseWheel(char delta) {
	printf("moues wheel: %d\n", delta);
}

void InputEventSystem::handleKeyDown(int key) {
	wprintf(L"%c", key);
}