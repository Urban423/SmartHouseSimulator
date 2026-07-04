#pragma once
#include "Physic.h"
#include "IOSystem.h"
#include "Transform.h"
#include "ClientServerSystem.h"

class InputComponent: public Component {
public:
	Input input;
	bool isLocalPaused;

	void Update() {
		if (input.pausePressed) isLocalPaused = !isLocalPaused;
		
		// Time::timeScale = paused ? 0 : 1.0f;
        IOSystem::getPlatform().showCursor(isLocalPaused);
        IOSystem::lockMouse(!isLocalPaused);


		Span<TextView> textViews = ECS::GetComponents<TextView>();
		for(int i = 0; i < input.text.size(); i++) {
			int c = input.text[i];
			if(c == 8) { //backspace
				if(!textViews[0].text.empty()) {
					textViews[0].text.pop_back();
				}
				continue;
			}
			if(c == 9) { // tab
				textViews[0].text.append("    ");
				continue;
			}
			if(IOSystem::getKeyBoard().GetKey(KeyCode_Left_Shift) && c == 13) {
				textViews[0].text.push_back('\n');
				continue;
			}
			else if(c == 13) { // enter
				if(textViews[0].text == "host") {
					// ClientServerSystem::getInstance().host(7777);
				}

				char command[32];
				char ip[64];
				int port;
				// ClientServerSystem::getInstance().connect("127.0.0.1", 7777);
				if (sscanf(textViews[0].text.c_str(), "%31s %63s %d", command, ip, &port) == 3)
				{
					if (strcmp(command, "connect") == 0)
					{
						ClientServerSystem::getInstance().connect(ip, port);
					}
				}
				else
				{
					printf("error\n");
				}
				textViews[0].text.clear();
				continue;
			}
			if(c >= 32) {
				textViews[0].text.push_back((char)c);
			}
		}
		if(!input.text.empty())
		{
			textViews[0].buildMesh();
		}

		// if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_P)) {
		// 	printf("connect");
		// 	ClientServerSystem::getInstance().connect("127.0.0.1", 7777);
		// }
		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_O)) {
			printf("host");
			// ClientServerSystem::getInstance().host(7777);
		}
	}
};

class CameraControlSystem: public Component {
public:
	Object obj;
	float walkSpeed = 3;
	float runSpeed = 6;
	float jumpPower = 32;
	float sensitivity = 121;
	float minDist = 0.00000003f;
	float maxDist = 0.08f;
	float pitch = 0.0f;
	bool sitting = false;

	void sit(bool value) {
		sitting = value;
		object.GetComponent<SphereCollider>().radius = 0.2f * sitting + (1 - sitting) * 1.0f;
	}

	bool checkPause(Input& input) {
		if(!object.HasComponent<InputComponent>()) return true;
		InputComponent& ic = object.GetComponent<InputComponent>();
		input = ic.input;

		if(ic.isLocalPaused) return true;
		return false;
	}

	void FixedUpdate() {
		Input input;
		if(checkPause(input)) return;

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
		Input input;
		if(checkPause(input)) return;
		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_S) && IOSystem::getKeyBoard().GetKey(KeyCode_Left_CTRL)) {
			printf("save\n");
		}

		//camera
		Object camera = object.getChild(0);	
		Vector2 cursor = input.pointerDelta;
		float sens = sensitivity * Time::deltaTime;
		pitch += -cursor.y * sens;
		pitch = clamp(-89.9f, 89.9f, pitch);
		camera.transform.rotation = Quaternion::FromEuler(pitch, 0, 0);
		object.transform.rotation *= Quaternion::FromEuler(0, -cursor.x * sens, 0);

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