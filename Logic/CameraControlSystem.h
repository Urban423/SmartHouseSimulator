#pragma once
#include "IOSP.h"
#include "Transform.h"

class CameraControlSystem: public Component {
public:
	float speed = 1;
	float minDist = 0.0003f;
	float maxDist = 0.007f;

	void UpdateFPSO()
	{
		Vector2 cursor(0, 0);
		if(GetKey(KeyCode_LeftMouseButton))  {
			Camera& camData = object.GetComponent<Camera>();
			cursor = mousePositionDelta();
			object.transform.position.x += -cursor.x * camData.focalLength * speed;
			object.transform.position.y +=  cursor.y * camData.focalLength * speed;
			object.transform.position.x = clamp(-3, 3, object.transform.position.x);
			object.transform.position.y = clamp(-3, 3, object.transform.position.y);
		}
	}
	
	void ChangeDist(char delta) {
		Camera& camData = object.GetComponent<Camera>();
		float zoomFactor = 1.1f;
		if (delta > 0)
			camData.focalLength /= zoomFactor;
		else if (delta < 0)
			camData.focalLength *= zoomFactor;
		
		if(camData.focalLength < minDist) {
			camData.focalLength = minDist;
		}
		if(camData.focalLength > maxDist) {
			camData.focalLength = maxDist;
		}
		camData.projection.setIdentity();
		camData.projection.setOrthoLH((float)camData.right * camData.focalLength, (float)camData.bottom * camData.focalLength, -4, 4);
	};		
};