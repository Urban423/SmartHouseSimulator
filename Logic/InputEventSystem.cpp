#include "InputEventSystem.h"
#include "ECS.h"
#include "Transform.h"
#include "CameraControlSystem.h"
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

bool checkCollision(Vector3 center, Vector3 scale, Vector3 pos) {
	if((pos.x < center.x - scale.x) || (pos.x > center.x + scale.x)) { return false; }
	if((pos.y < center.y - scale.y) || (pos.y > center.y + scale.y)) { return false; }
	return true;
}

void InputEventSystem::handleMouseClickDown(float x, float y) {
	auto[renderViews, size] = ECS::GetComponents<RenderView>(0);
	for(int i = size - 1; i >= 0; i--) {
		if(checkCollision(renderViews[i].object.transform.position, renderViews[i].object.transform.scale, Vector3(x, y, 0) )) {
			if(renderViews[i].object.HasComponent<Button>()) {
				Button& button = renderViews[i].object.GetComponent<Button>();
				button.pressed = true;
				if(button.onMouseDown) { button.onMouseDown(); }
			}
			break;
		}
	}
}

void InputEventSystem::handleMouseClickUp(float x, float y) {
		auto[renderViews, size] = ECS::GetComponents<RenderView>(0);
		for(int i = size - 1; i >= 0; i--) {
		if(checkCollision(renderViews[i].object.transform.position, renderViews[i].object.transform.scale, Vector3(x, y, 0) )) {
			if(renderViews[i].object.HasComponent<Button>()) {
				Button& button = renderViews[i].object.GetComponent<Button>();
				button.pressed = false;
				if(button.onMouseUp) { button.onMouseUp(); }
			}
			break;
		}
	}
}

void InputEventSystem::handleMouseWheel(char delta) {
	if(delta != 120 && delta != -120) { return; }
	auto[cameraControlSystems, size] = ECS::GetComponents<CameraControlSystem>(0);
	for(int i = 0 ; i < size; i++) {
		cameraControlSystems[i].ChangeDist(delta);
	}
}

void InputEventSystem::handleKeyDown(int key) {
	wprintf(L"%c", key);
}