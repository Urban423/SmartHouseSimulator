#pragma once
#include "Physic.h"
#include "IOSystem.h"
#include "Transform.h"
#include "PauseManager.h"
#include "ClientServerSystem.h"

class InputComponent: public Component {
public:
	Input input;

	void Update() {
		if (input.pausePressed) PauseManager::Toggle();
		PauseManager::Update();
	}
};

class CameraControlSystem: public Component {
public:
	Object obj;
	float walkSpeed = 3;
	float runSpeed = 6;
	float jumpPower = 32;
	float minDist = 0.00000003f;
	float maxDist = 0.08f;
	float pitch = 13;
	bool sitting = false;

	void sit(bool value) {
		sitting = value;
		object.GetComponent<SphereCollider>().radius = 0.2f * sitting + (1 - sitting) * 1.0f;
	}

	void FixedUpdate() {
		if (PauseManager::IsPaused()) return;
		Input& input = object.GetComponent<InputComponent>().input;

		Rigidbody& rb = object.GetComponent<Rigidbody>();
		
		Vector3 forward = object.transform.rotation * Vector3(0, 0, -1);
		Vector3 right   = object.transform.rotation * Vector3(1, 0, 0);
		Vector3 move = forward * input.movement.y + right * input.movement.x;
		if(input.jumpPressed) {
			rb.AddForce(Vector3(0, jumpPower, 0));
			// player.transform.position.y += walkSpeed * Time::deltaTime;
			sit(false);
		} 
		if(input.sitPressed) {
			// sit(!sitting);
			object.transform.position.y -= walkSpeed * Time::deltaTime;
		}

		float speed = input.sprint? runSpeed : walkSpeed;
		Vector3 currentVel = Vector3(rb.velocity.x, 0, rb.velocity.z);
		Vector3 targetVel = move.normalized() * speed;
		Vector3 error = targetVel - currentVel;
		// printf("%f %f %f\n", error.x, error.y, error.z);
		rb.AddForce(error * 20);
		// player.transform.position += targetVel * 0.1f;

	}

	void Update() {
		if (PauseManager::IsPaused()) return;
		Input& input = object.GetComponent<InputComponent>().input;

		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_S) && IOSystem::getKeyBoard().GetKey(KeyCode_Left_CTRL)) {
			printf("save\n");
		}

		//camera
		Object camera = object.getChild(0);	
		Vector2 cursor = input.pointerDelta;
		Settings& s = SettingsSystem::GetSettings();
		float sensX = s.sensitivityX * 0.1f;
		float sensY = s.sensitivityY * 0.1f;
		pitch += -cursor.y * sensY;
		pitch = clamp(-89.9f, 89.9f, pitch);
		camera.transform.rotation = Quaternion::FromEuler(pitch, 0, 0);
		object.transform.rotation *= Quaternion::FromEuler(0, -cursor.x * sensX, 0);

		//interact
		if(input.actionPressed) {
			RayHit hit;
			if (PhysicSystem::RayCast(object.transform.position, object.transform.rotation * Vector3(0, 0, -1), 100.0f, hit)) {
				obj.transform.position = hit.point;
				printf("%d\n", hit.collider->object.HasComponent<Rigidbody>());
			}
		}
	}
};

inline static void InputComponentUpdate() {
	Object localPlayer = ClientServerSystem::getInstance().getLocalPlayer();
	InputComponent* ic;
	if(localPlayer.valid() && localPlayer.HasComponent<InputComponent>())  {
		ic = &localPlayer.GetComponent<InputComponent>();
	}
	else {
		Span<CameraControlSystem> players = ECS::GetComponents<CameraControlSystem>();
		ic = &players[0].object.GetComponent<InputComponent>();
	}
	ic->input = IOSystem::getInput();
	ic->Update();
}




// if(GetKey(KeyCode_LeftMouseButton))  {
// 	cursor = mousePositionDelta();
// 	object.transform.position.x += -cursor.x * camData.focalLength * speed;
// 	object.transform.position.y +=  cursor.y * camData.focalLength * speed;
// 	object.transform.position.x = clamp(-3, 3, object.transform.position.x);
// 	object.transform.position.y = clamp(-3, 3, object.transform.position.y);
// }
// if(Input::mouseScrollDelta) {
// 	float zoomFactor = 1.45f;
// 	if (Input::mouseScrollDelta > 0)
// 		camData.focalLength /= zoomFactor;
// 	else if (Input::mouseScrollDelta < 0)
// 		camData.focalLength *= zoomFactor;
	
// 	if(camData.focalLength < minDist) {
// 		camData.focalLength = minDist;
// 	}
// 	if(camData.focalLength > maxDist) {
// 		camData.focalLength = maxDist;
// 	}
// }