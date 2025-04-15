#pragma once
#include "IOSP.h"
#include "Transform.h"

struct FirstPersonController
{
	Transform* transform;
	Transform* camera;
	float speed = 0.1f;
	float yaw;
	float pitch;
};

inline void UpdateFPSO(FirstPersonController* player, int size)
{
	for(int i = 0; i < size; i++)
	{
		Vector2 cursor(0, 0);
		if(GetKey(KeyCode_LeftMouseButton))  {
			cursor = mousePositionDelta();
			player[i].yaw 	= -cursor.y * 0.003f;
			player[i].pitch = -cursor.x * 0.003f;
			player[i].camera->rotation    *= Quaternion(0, player[i].pitch, 0, 1);
			player[i].camera->rotation    *= Quaternion( player[i].yaw, 0,  0, 1);
			player[i].transform->rotation.normalize();
			player[i].camera->rotation.normalize();
		}
		
		Vector3 forward = player[i].transform->rotation * Vector3(0, 0, 1);
		Vector3 up      = player[i].transform->rotation * Vector3(0, 1, 0);
		Vector3 right 	= player[i].transform->rotation * Vector3(1, 0, 0);
		if(GetKey(KeyCode_W)) {
			player[i].transform->position.z -= player[i].speed;
		}
		else if(GetKey(KeyCode_S)) {
			player[i].transform->position.z += player[i].speed;
		}
		
		if(GetKey(KeyCode_A)) {
			player[i].transform->position.x -=  player[i].speed;
		}
		else if(GetKey(KeyCode_D)) {
			player[i].transform->position.x +=  player[i].speed;
		}
		
		if(GetKey(KeyCode_Space)) {
			player[i].transform->position += up * player[i].speed;
		}
		else if(GetKey(KeyCode_Left_CTRL)) {
			player[i].transform->position -= up * player[i].speed;
		}
	}
}