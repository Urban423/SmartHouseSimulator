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
		Camera& camData = object.GetComponent<Camera>();
		if(GetKey(KeyCode_LeftMouseButton))  {
			cursor = mousePositionDelta();
			object.transform.position.x += -cursor.x * camData.focalLength * speed;
			object.transform.position.y +=  cursor.y * camData.focalLength * speed;
			object.transform.position.x = clamp(-3, 3, object.transform.position.x);
			object.transform.position.y = clamp(-3, 3, object.transform.position.y);
		}
		if(Input::mouseScrollDelta) {
			float zoomFactor = 1.1f;
			if (Input::mouseScrollDelta > 0)
				camData.focalLength /= zoomFactor;
			else if (Input::mouseScrollDelta < 0)
				camData.focalLength *= zoomFactor;
			
			if(camData.focalLength < minDist) {
				camData.focalLength = minDist;
			}
			if(camData.focalLength > maxDist) {
				camData.focalLength = maxDist;
			}
		}
	}	
};