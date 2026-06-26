#pragma once
#include "Physic.h"
#include "IOSystem.h"
#include "Transform.h"
#include "NetworkManager.h"

class CameraControlSystem: public Component {
public:
	Object obj;
	float walkSpeed = 3;
	float runSpeed = 6;
	float jumpPower = 3;
	float sensitivity = 11;
	float minDist = 0.00000003f;
	float maxDist = 0.08f;
	bool pause = false;
	float pitch = 0.0f;
	bool sitting = false;

	void sit(bool value) {
		sitting = value;
		Object player = object.getParent();
		player.GetComponent<SphereCollider>().radius = 0.2f * sitting + (1 - sitting) * 1.0f;
	}

	void UpdateFPSO() {
		Input& input = IOSystem::getInput();
		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_S) && IOSystem::getKeyBoard().GetKey(KeyCode_Left_CTRL)) {
			printf("save\n");
		}
		
		//pause
		if(input.pausePressed) pause = !pause;
		IOSystem::getPlatform().showCursor(pause);
		Time::timeScale = pause ? 0 : 1.0f;
		IOSystem::lockMouse(!pause);
		if(pause) return;



		//camera
		Object player = object.getParent();	
		Rigidbody& rb = player.GetComponent<Rigidbody>();
		Vector2 cursor = input.pointerDelta;
		// Camera& camData = object.GetComponent<Camera>();
		float sens = sensitivity * Time::deltaTime;
		pitch += -cursor.y * sens;
		pitch = clamp(-89.9f, 89.9f, pitch);
		object.transform.rotation = Quaternion::FromEuler(pitch, 0, 0);
		player.transform.rotation *= Quaternion::FromEuler(0, -cursor.x * sens, 0);


		//movement
		Vector3 forward = player.transform.rotation * Vector3(0, 0, -1);
		Vector3 right   = player.transform.rotation * Vector3(1, 0, 0);
		Vector3 move = forward * input.movement.y + right * input.movement.x;
		if(input.jumpPressed) {
			rb.velocity += Vector3(0, jumpPower, 0);
			player.transform.position.y += walkSpeed * Time::deltaTime;
			sit(false);
		} 
		if(input.sitPressed) {
			// sit(!sitting);
			player.transform.position.y -= walkSpeed * Time::deltaTime;
		}
		float speed = input.sprint? runSpeed : walkSpeed;
		Vector3 currentVel = Vector3(rb.velocity.x, 0, rb.velocity.z);
		Vector3 targetVel = move.normalized() * speed;
		Vector3 error = targetVel - currentVel;
		rb.AddForce(error * 20);
		// player.transform.position += targetVel * 0.1f;


		//interact
		if(input.actionPressed) {
			RayHit hit;
			if (PhysicSystem::RayCast(object.transform.position, object.transform.rotation * Vector3(0, 0, -1), 100.0f, hit)) {
				obj.transform.position = hit.point;
				printf("%d\n", hit.collider->object.HasComponent<Rigidbody>());
			}
		}

		auto [textViews, textViewsCount] = ECS::GetComponents<TextView>();
		for(int i = 0; i < input.text.size(); i++) {
			int c = input.text[i];
			// printf("c: %c %d\n", c, c);
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
					NetworkManager::getInstance().host(7777);
				}
				else if(textViews[0].text == "client") {
					NetworkManager::getInstance().connect("127.0.0.1", 7777);
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


		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_P)) {
			printf("connect");
			NetworkManager::getInstance().connect("127.0.0.1", 7777);
		}
		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_O)) {
			printf("host");
			NetworkManager::getInstance().host(7777);
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
	}	
};