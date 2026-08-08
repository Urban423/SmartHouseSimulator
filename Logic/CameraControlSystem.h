#pragma once
#include "Physic.h"
#include "IOSystem.h"
#include "Transform.h"
#include "OverlayManager.h"
#include "ClientServerSystem.h"
#include "Voxels.h"

class InputComponent: public Component {
public:
	Input input;

	void Update() {

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
	Object inventory;

	void sit(bool value) {
		sitting = value;
    	// CapsuleCollider& capsule = object.GetComponent<CapsuleCollider>();
		object.transform.scale[1] = 0.5f * sitting + (1 - sitting) * 1.0f;
	}

	void FixedUpdate() {
		// if (OverlayManager::IsPaused()) return;
		Input& input = object.GetComponent<InputComponent>().input;
		CharacterController& characterController = object.GetComponent<CharacterController>();
		Rigidbody& rb = object.GetComponent<Rigidbody>();
		
		Vector3 forward = object.transform.rotation * Vector3(0, 0, -1);
		Vector3 right   = object.transform.rotation * Vector3(1, 0, 0);
		if (characterController.grounded)
		{
			forward = Vector3::ProjectOnPlane(forward, characterController.groundNormal).normalized();
			right   = Vector3::ProjectOnPlane(right, characterController.groundNormal).normalized();
		}
		Vector3 move = forward * input.movement.y + right * input.movement.x;
		bool sitted = false;
		if(input.jumpPressed) {
			rb.AddForce(Vector3(0, jumpPower, 0));
			object.transform.position.y += walkSpeed * Time::deltaTime;
		} 
		if(input.sitPressed) {
			object.transform.position.y -= walkSpeed * Time::deltaTime;
			sitted = true;
		}
		sit(sitted);

		float speed = input.sprint? runSpeed : walkSpeed;
		Vector3 moveDir = move.normalized();
		Vector3 targetVel = moveDir * speed;
		Vector3 planarVel = rb.velocity;
		if (characterController.grounded) {
			planarVel = Vector3::ProjectOnPlane(rb.velocity, characterController.groundNormal);
		}
		else {
			planarVel.y = 0;
		}
		Vector3 verticalVel = rb.velocity - planarVel;
		rb.velocity = targetVel + verticalVel;
	}

	void Update() {
		// if (OverlayManager::IsPaused()) return;
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
		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_RightMouseButton) && inventory.valid()) {
			inventory.transform.position += object.transform.position;
			inventory.GetComponent<Rigidbody>().isKinematic = false;
			inventory.setParent(-1);
			inventory = Object();
			return;
		}
		
		if(input.actionPressed) {
			RayHit hit;
			Vector3 source = object.transform.position + camera.transform.position;
			Vector3 direction = object.transform.rotation * camera.transform.rotation * Vector3(0, 0, -1);
			if (PhysicSystem::RayCast(source, direction, 100.0f, hit, 0)) {
				if(hit.collider->object.HasComponent<SolutionObstacle>()) {
					if(hit.collider->object.GetComponent<SolutionObstacle>().isObstacle) {
						if(inventory.valid() && inventory.GetComponent<SolutionObstacle>().mask & hit.collider->object.GetComponent<SolutionObstacle>().mask) {
							hit.collider->object.transform.position -= 100;
						}
					}
					else if(!inventory.valid()) {
						inventory = hit.collider->object;
						inventory.transform.position = Vector3(0, -0.5f, -2);
						inventory.GetComponent<Rigidbody>().isKinematic = true;
						inventory.setParent(object);
					}
					return;
				}
			}
		}

		// float explosionRadius = 10.0f;
		// float maxForce = 600.0f;
		// if(input.pointerPressed || input.pointerHold) {
		// 	RayHit hit;
		// 	Vector3 source = object.transform.position + camera.transform.position;
		// 	Vector3 direction = object.transform.rotation * camera.transform.rotation * Vector3(0, 0, -1);
		// 	if (PhysicSystem::RayCast(source, direction, 100.0f, hit, 0)) {
		// 		if(!hit.collider->object.HasComponent<Rigidbody>()) return;
		// 		// obj.transform.position = hit.point;
		// 		Rigidbody& body = hit.collider->object.GetComponent<Rigidbody>();
		// 		float distanceMagnitude = direction.sqrMagnitude();
		// 		float strength = maxForce / 70 * (1.0f - distanceMagnitude / explosionRadius);
		// 		float distance = 1.0f / sqrtf(distanceMagnitude);
		// 		direction *= distance;
		// 		body.AddForce(direction * strength);
		// 		if(body.object.HasComponent<Voxels>() && body.object.GetComponent<Voxels>().destructible) {
		// 			Vector3 localSource = Quaternion::Inverse(body.object.transform.rotation) * (source - body.object.transform.position);
		// 			Vector3 localDir 	= Quaternion::Inverse(body.object.transform.rotation) * direction;
		// 			body.object.GetComponent<Voxels>().fireVoxels(localSource, localDir);
		// 		}
		// 	}
		// }


		// if(input.actionPressed) {
		// 	RayHit hit;
		// 	if (PhysicSystem::RayCast(object.transform.position + camera.transform.position, object.transform.rotation * camera.transform.rotation * Vector3(0, 0, -1), 100.0f, hit, 0)) {
		// 		obj.transform.position = hit.point;
		// 		Span<Rigidbody> rigidbodies = ECS::GetComponents<Rigidbody>();
		// 		for(auto& body : rigidbodies) {
		// 			Vector3 direction = body.object.transform.position - hit.point + hit.normal;
		// 			float distanceMagnitude = direction.sqrMagnitude();
		// 			if (distanceMagnitude < explosionRadius * explosionRadius)  {
		// 				float strength = maxForce * (1.0f - distanceMagnitude / ( explosionRadius * explosionRadius ));
		// 				float distance = 1.0f / sqrtf(distanceMagnitude);
		// 				direction *= distance;
		// 				body.AddForce(direction * strength);
		// 				if(body.object.HasComponent<Voxels>() && body.object.GetComponent<Voxels>().destructible) {
		// 					Vector3 worldLocal = hit.point - body.object.transform.position;
		// 					Vector3 localHit = Quaternion::Inverse(body.object.transform.rotation) * worldLocal;
		// 					body.object.GetComponent<Voxels>().explodeVoxels(localHit, explosionRadius);
		// 				}
		// 			}
		// 		}
 		// 		// printf("%d\n", hit.collider->object.HasComponent<Rigidbody>());
		// 	}
		// }

		// if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_RightMouseButton)) {
		// 	RayHit hit;
		// 	if (PhysicSystem::RayCast(object.transform.position + camera.transform.position, object.transform.rotation * camera.transform.rotation * Vector3(0, 0, -1), 100.0f, hit, 0)) {
		// 		obj.transform.position = hit.point;
		// 		Span<Rigidbody> rigidbodies = ECS::GetComponents<Rigidbody>();
		// 		for(auto& body : rigidbodies) {
		// 			Vector3 direction = body.object.transform.position - hit.point + hit.normal;
		// 			float distanceMagnitude = direction.sqrMagnitude();
		// 			if (distanceMagnitude < explosionRadius * explosionRadius)  {
		// 				float strength = maxForce * (1.0f - distanceMagnitude / ( explosionRadius ));
		// 				float distance = 1.0f / sqrtf(distanceMagnitude);
		// 				direction *= distance;
		// 				body.AddForce(direction * strength);
		// 				if(body.object.HasComponent<Voxels>() && body.object.GetComponent<Voxels>().destructible) {
		// 					Vector3 worldLocal = hit.point - body.object.transform.position;
		// 					Vector3 localHit = Quaternion::Inverse(body.object.transform.rotation) * worldLocal;
		// 					body.object.GetComponent<Voxels>().explodeVoxels(localHit, explosionRadius);
		// 				}
		// 			}
		// 		}
		// 	}
		// }

		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_P)) ClientServerSystem::getInstance().connect("127.0.0.1", 7777);
		if(IOSystem::getKeyBoard().GetKeyDown(KeyCode_O)) ClientServerSystem::getInstance().host(7777);
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
	if (!OverlayManager::IsPaused()) {
		ic->input = std::move(IOSystem::getInput());
	}
	else {
		ic->input = std::move(Input());
	}
	if (IOSystem::getInput().pausePressed) OverlayManager::Toggle();
	OverlayManager::Update();
}