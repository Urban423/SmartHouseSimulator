#include "ScreenLogic.h"
#include "GraphicsEngine.h"
#include <IOSP.h>
#include <algorithm>
#include "ECS.h"

//ScreenLogic::ScreenLogic(): rects(std::vector<Rect>(1, {0, 0, 1, 1})) {}

bool areNeighbors(int& axis, Vector3 posA, Vector3 scaleA, Vector3 posB, Vector3 scaleB) {
	Vector2 box1_min = Vector2(posA.x - scaleA.x, posA.y - scaleA.y);
	Vector2 box1_max = Vector2(posA.x + scaleA.x, posA.y + scaleA.y);
	Vector2 box2_min = Vector2(posB.x - scaleB.x, posB.y - scaleB.y);
	Vector2 box2_max = Vector2(posB.x + scaleB.x, posB.y + scaleB.y);
	
	float distX = std::min(box1_max.x, box2_max.x) - std::max(box1_min.x, box2_min.x);
	float distY = std::min(box1_max.y, box2_max.y) - std::max(box1_min.y, box2_min.y);
	axis = 0;
	if(distX > distY) {
		axis = 1;
		std::swap(distX, distY);
	}
	return std::abs(distX) < EPSILON_RECTS;
}

rectsCollisionData getCollisionPoints(Vector3 posA, Vector3 scaleA, Vector3 posB, Vector3 scaleB) {
	Vector2 box1_min = Vector2(posA.x - scaleA.x, posA.y - scaleA.y);
	Vector2 box1_max = Vector2(posA.x + scaleA.x, posA.y + scaleA.y);
	Vector2 box2_min = Vector2(posB.x - scaleB.x, posB.y - scaleB.y);
	Vector2 box2_max = Vector2(posB.x + scaleB.x, posB.y + scaleB.y);
	
	float distX = std::min(box1_max.x, box2_max.x) - std::max(box1_min.x, box2_min.x);
	float distY = std::min(box1_max.y, box2_max.y) - std::max(box1_min.y, box2_min.y);
	int axis = 0;
	if(distX > distY) {
		std::swap(distX, distY);
		axis = 1;
	}
	
	float points[] 		= { box1_min[!axis], box1_max[!axis],  box2_min[!axis], box2_max[!axis]};
	float pointsAxis[] 	= { box1_min[axis], box1_max[axis],  box2_min[axis], box2_max[axis]};
	std::sort(std::begin(points), std::end(points));
	std::sort(std::begin(pointsAxis), std::end(pointsAxis));
	return { { points[0], points[1], points[2], points[3]}, { pointsAxis[0], pointsAxis[1], pointsAxis[2], pointsAxis[3]}, axis, (points[0] == box2_min[!axis]) | ((points[3] == box2_max[!axis]) << 1) };
}

void ScreenLogic::update() {
	if(mouseState == MouseOnFrame) {
		if(GetKeyDown(KeyCode_LeftMouseButton)) {
			Vector2 MousePos = getMouseScene();
			rectangle = findArea(MousePos.x, MousePos.y);
			
			float dx = MousePos.x - rectangle.transform.position.x;
			float dy = MousePos.y - rectangle.transform.position.y;
			angle.x = (dx < 0) ? rectangle.transform.position.x - rectangle.transform.scale.x : rectangle.transform.position.x + rectangle.transform.scale.x;
			angle.y = (dy < 0) ? rectangle.transform.position.y - rectangle.transform.scale.y : rectangle.transform.position.y + rectangle.transform.scale.y;
			if(Vector2::DistanceSquare(MousePos, angle) < checkBoxSize) { mouseState = MouseOnBoxes; return;};
			
			
			
			
			Vector2 closestSideCenter = angle;
			float deltaX = std::abs(MousePos.x - angle.x);
			float deltaY = std::abs(MousePos.y - angle.y);
			axis = 0;
			if (deltaX > deltaY) {
				axis = 1;
				 closestSideCenter.x = rectangle.transform.position.x;
			} else {
				closestSideCenter.y = rectangle.transform.position.y;
			}
			if((deltaX < 3 * checkBoxSize || deltaY < 3 * checkBoxSize) && closestSideCenter[axis] != rect[0][axis] && closestSideCenter[axis] != rect[3][axis]) { 
				mouseState =  MouseOnSplit;
				Vector4 tempClamp = fillArea(closestSideCenter, !axis);
				leftClamp = tempClamp.x;
				rightClamp = tempClamp.y;
				controlPanelRender.transform.scale[axis] 				= offsetSize;
				controlPanelRender.transform.scale[!axis] 				= (tempClamp.w - tempClamp.z) / 2;
				controlPanelRender.transform.position[!axis] 			= (tempClamp.w + tempClamp.z) / 2;
				controlPanelRender.GetComponent<RenderView>().color.r 	= 0.7f;
				controlPanelRender.GetComponent<RenderView>().color.g 	= axis ? 0: 0.5;
				controlPanelRender.GetComponent<RenderView>().color.b 	= axis ? 0.5: 0;
				return;
			}
		}
		return;
	}
	
	bool new_zone = false;
	Vector2 MousePos = getMouseScene();
	Object newRectangle;
	if(mouseState == MouseOnBoxes) {
		controlPanelRender.transform.scale = Vector3(offsetSize, offsetSize, offsetSize);
		controlPanelRender.GetComponent<RenderView>().enabled = false;
		newRectangle = findArea(MousePos.x, MousePos.y);
		if(newRectangle == -1) { return; }
		else if(newRectangle != rectangle) { 
			controlPanelRender.transform.position = newRectangle.transform.position;
			controlPanelRender.transform.scale 	= newRectangle.transform.scale;
			new_zone = areNeighbors(axis, rectangle.transform.position, rectangle.transform.scale, newRectangle.transform.position, newRectangle.transform.scale);
			controlPanelRender.GetComponent<RenderView>().enabled = new_zone;
			controlPanelRender.GetComponent<RenderView>().color.r = 0.7f;
			controlPanelRender.GetComponent<RenderView>().color.g = 0;
			controlPanelRender.GetComponent<RenderView>().color.b = 0;
		}
		else {
			axis = 0;
			float deltaX = std::abs(MousePos.x - angle.x);
			float deltaY = std::abs(MousePos.y - angle.y);
			if(deltaY < deltaX) { axis = 1; }
			float delta = MousePos[!axis] - newRectangle.transform.position[!axis];
			float closestSide = (delta < 0) ? newRectangle.transform.position[!axis] - newRectangle.transform.scale[!axis] : newRectangle.transform.position[!axis] + newRectangle.transform.scale[!axis];
			
			if(std::abs(MousePos[!axis] - closestSide) < minSize) { if(GetKeyUp(KeyCode_LeftMouseButton)) { mouseState = MouseOnFrame; } return; } 
			
			controlPanelRender.transform.position = newRectangle.transform.position;
			controlPanelRender.transform.position[!axis] = MousePos[!axis];
			controlPanelRender.transform.scale[axis] = newRectangle.transform.scale[axis];
			controlPanelRender.GetComponent<RenderView>().enabled = true;
			controlPanelRender.GetComponent<RenderView>().color.r = 0;
			controlPanelRender.GetComponent<RenderView>().color.g = axis ? 0.5: 0;
			controlPanelRender.GetComponent<RenderView>().color.b = axis ? 0: 0.5;
		}
	} 
	else if(mouseState == MouseOnSplit) {
		controlPanelRender.GetComponent<RenderView>().enabled = true;
		MousePos[axis] = clamp(leftClamp + minSize, rightClamp - minSize, MousePos[axis]);
		controlPanelRender.transform.position[axis] = MousePos[axis];
		std::pair<RenderView*, int> s = ECS::GetComponents<RenderView>(0);
		for(int i = 0; i < leftMove.size(); i++) {
			int objID = leftMove[i];
			s.first[objID].object.transform.position[axis] = (leftSide[i] +  MousePos[axis]) / 2;
			s.first[objID].object.transform.scale[axis] = std::abs(leftSide[i] -  MousePos[axis]) / 2;
		}
		for(int i = 0; i < rightMove.size(); i++) {
			int objID = rightMove[i];
			s.first[objID].object.transform.position[axis] = (rightSide[i] + MousePos[axis]) / 2;
			s.first[objID].object.transform.scale[axis] = std::abs(rightSide[i] -  MousePos[axis]) / 2;
		}
	}
	
	if(GetKeyUp(KeyCode_LeftMouseButton)) {
		if(new_zone) {
			if(newRectangle.transform.position[axis] < rectangle.transform.position[axis]) { std::swap(rectangle, newRectangle); }
			rectsCollisionData collisionData = getCollisionPoints(rectangle.transform.position, rectangle.transform.scale, newRectangle.transform.position, newRectangle.transform.scale);
			float zones[] = {
				collisionData.points[1] - collisionData.points[0],
				collisionData.points[2] - collisionData.points[1],
				collisionData.points[3] - collisionData.points[2]
			};
			int holes 		= (zones[0] > EPSILON_RECTS) | ((zones[2] > EPSILON_RECTS) << 1);
			bool check = (zones[1] >= minSize) && (zones[0] >= minSize || !(holes & 0x1)) && (zones[2] >= minSize || !(holes & 0x2));
			if(check == 0) { controlPanelRender.GetComponent<RenderView>().enabled = false; mouseState = MouseOnFrame; return; }
			
			rectangle.transform.position[axis] 		= (collisionData.pointsAxis[3] + collisionData.pointsAxis[0]) / 2;
			rectangle.transform.position[!axis] 	= (collisionData.points[2] + collisionData.points[1]) / 2;
			rectangle.transform.scale[axis] 		= (collisionData.pointsAxis[3] - collisionData.pointsAxis[0]) / 2;
			rectangle.transform.scale[!axis] 		= zones[1] / 2;
			
			if(holes == 0) {
				ECS::deleteObject(newRectangle);
				controlPanelRender.GetComponent<RenderView>().enabled = false;
				mouseState = MouseOnFrame;
				return;
			}
			else if(holes == 3) {
				int holeIndex2 = (holes & 0x1);
				int zoneIndex2 = 2 * ((collisionData.spaces >> holeIndex2) & 0x1) ;
				holeIndex2 *= 2;
				
				Vector3 newPos, newScale;
				newPos[axis] 		= (collisionData.pointsAxis[zoneIndex2 + 1] + collisionData.pointsAxis[zoneIndex2]) / 2;
				newPos[!axis] 		= (collisionData.points[holeIndex2 + 1] + collisionData.points[holeIndex2]) / 2;
				newScale[axis] 		= (collisionData.pointsAxis[zoneIndex2 + 1] - collisionData.pointsAxis[zoneIndex2]) / 2;
				newScale[!axis] 	= zones[holeIndex2] / 2;
				
				Object copy = ECS::createObject();
				copy.transform = {newPos, Quaternion(0, 0, 0, 1), newScale};
				copy.AddComponent<RenderView>(0);// = rectangle.GetComponent<RenderView>();
			}
			
			int holeIndex1 = !(holes & 0x1);
			int zoneIndex1 = 2 * ((collisionData.spaces >> holeIndex1) & 0x1) ;
			holeIndex1 *= 2;
			
			newRectangle.transform.position[axis] 	= (collisionData.pointsAxis[zoneIndex1 + 1] + collisionData.pointsAxis[zoneIndex1]) / 2;
			newRectangle.transform.position[!axis] 	= (collisionData.points[holeIndex1 + 1] + collisionData.points[holeIndex1]) / 2;
			newRectangle.transform.scale[axis] 		= (collisionData.pointsAxis[zoneIndex1 + 1] - collisionData.pointsAxis[zoneIndex1]) / 2;
			newRectangle.transform.scale[!axis] 	= zones[holeIndex1] / 2;
		}
		else if(mouseState == MouseOnBoxes && controlPanelRender.GetComponent<RenderView>().enabled) {
			Vector3 pointEdge1 =  rectangle.transform.position - rectangle.transform.scale * Vector3(axis, !axis, 0);
			Vector3 pointEdge2 =  2 * rectangle.transform.position - pointEdge1;
			Vector3 newPos = (controlPanelRender.transform.position + pointEdge2) / 2;
			Vector3 newScale = controlPanelRender.transform.scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge2, newPos) * Vector3(axis, !axis, 0);
			rectangle.transform.position =  (controlPanelRender.transform.position + pointEdge1) / 2;
			rectangle.transform.scale 	=  controlPanelRender.transform.scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge1, rectangle.transform.position) * Vector3(axis, !axis, 0);
			
			Object copy = ECS::createObject();
			copy.transform = {newPos, Quaternion(0, 0, 0, 1), newScale};
			copy.AddComponent<RenderView>(0);// = rectangle.GetComponent<RenderView>();
		}
		controlPanelRender.GetComponent<RenderView>().enabled = false;
		mouseState = MouseOnFrame;
	}
}

Object ScreenLogic::findArea(float x, float y) {
	std::pair<RenderView*, int> s = ECS::GetComponents<RenderView>(0);
	if(s.second == 0) { return Object(); };
	for(int i = 1; i < s.second; i++) {
		 if(
			(s.first[i].object.transform.position.x - s.first[i].object.transform.scale.x <= x && x <= s.first[i].object.transform.position.x + s.first[i].object.transform.scale.x) &&
			(s.first[i].object.transform.position.y - s.first[i].object.transform.scale.y <= y && y <= s.first[i].object.transform.position.y + s.first[i].object.transform.scale.y)
			) { return s.first[i].object; }
	}
	return Object();
}

void ScreenLogic::split(Vector3 position, Vector3 scale) {
	Object rectangle = findArea(position.x, position.y);
	axis = scale.y < scale.x;
	position[axis] 	= rectangle.transform.position[axis];
	scale[axis] 	= rectangle.transform.scale[axis];
	
	Vector3 pointEdge1 =  rectangle.transform.position - rectangle.transform.scale * Vector3(axis, !axis, 0);
	Vector3 pointEdge2 =  2 * rectangle.transform.position - pointEdge1;
	Vector3 newPos = (position + pointEdge2) / 2;
	Vector3 newScale = scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge2, newPos) * Vector3(axis, !axis, 0);
	rectangle.transform.position =  (position + pointEdge1) / 2;
	rectangle.transform.scale 	=  scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge1, rectangle.transform.position) * Vector3(axis, !axis, 0);
	
	Object copy = ECS::createObject();
	copy.transform = {newPos, Quaternion(0, 0, 0, 1), newScale};
	copy.AddComponent<RenderView>(0);// = rectangle.GetComponent<RenderView>();
}

bool checkAxis( const Transform& transform, const Vector2& center, int axis) {
	float left = transform.position[axis] - transform.scale[axis];
	float right = transform.position[axis] + transform.scale[axis];
	return std::abs(center[axis] - left) < EPSILON_RECTS_MOVE || std::abs(center[axis] - right) < EPSILON_RECTS_MOVE;
}

Vector4 ScreenLogic::fillArea(Vector2 center, int axis) {
	int otherAxis = !axis;
	float leftClampI = rect[0][otherAxis];
	float rightClampI = rect[3][otherAxis];
	std::vector<int> leftFiltered, rightFiltered;
	int centerLeft = 0;
	int centerRight = 0;
	float minDistLeft = std::numeric_limits<float>::max();
	float minDistRight = std::numeric_limits<float>::max();
	std::pair<RenderView*, int> s = ECS::GetComponents<RenderView>(0);
	 for(int i = 1; i < s.second; i++) {
		if (!checkAxis(s.first[i].object.transform, center, otherAxis)) continue;
		float dist = std::abs(center[axis] - s.first[i].object.transform.position[axis]);
		if (s.first[i].object.transform.position[otherAxis] < center[otherAxis]) {
			leftFiltered.push_back(i);
			if(dist < minDistLeft) {
				centerLeft = i;
				minDistLeft = dist;
			}
		} else {
			rightFiltered.push_back(i);
			if(dist < minDistRight) {
				centerRight = i;
				minDistRight = dist;
			}
		}
	}
	
	std::sort(leftFiltered.begin(), leftFiltered.end(), [&](int a, int b) {
        return s.first[a].object.transform.position[axis] < s.first[b].object.transform.position[axis];
    });

    std::sort(rightFiltered.begin(), rightFiltered.end(), [&](int a, int b) {
        return s.first[a].object.transform.position[axis] < s.first[b].object.transform.position[axis];
    });
	
	
	int newCenterLeft = 0; 
	int newCenterRight = 0;
	for(int i = 0; i < leftFiltered.size(); i++) {
		if(centerLeft == leftFiltered[i]) {
			newCenterLeft = i;
			break;
		}
	}
	for(int i = 0; i < rightFiltered.size(); i++) {
		if(centerRight == rightFiltered[i]) {
			newCenterRight = i;
			break;
		}
	}
	float minI =  1;
	float maxI = -1;
	
	leftMove.clear();
	leftSide.clear();
	if(leftFiltered.size() != 0) {
		leftMove.push_back(leftFiltered[newCenterLeft]);
		leftClampI = s.first[leftFiltered[newCenterLeft]].object.transform.position[otherAxis] -  s.first[leftFiltered[newCenterLeft]].object.transform.scale[otherAxis];
		leftSide.push_back(leftClampI);
		float minVal = s.first[centerLeft].object.transform.position[axis] - s.first[centerLeft].object.transform.scale[axis];
		float maxVal = s.first[centerLeft].object.transform.position[axis] + s.first[centerLeft].object.transform.scale[axis];
		for(int i = newCenterLeft - 1; i > -1; i--) {
			int objID = leftFiltered[i];
			float rectMaxVal = s.first[objID].object.transform.position[axis] + s.first[objID].object.transform.scale[axis];
			if(std::abs(minVal - rectMaxVal) > EPSILON_RECTS) { break; }
			minVal = s.first[objID].object.transform.position[axis] - s.first[objID].object.transform.scale[axis];
			leftMove.push_back(objID);
			float leftSideS = s.first[objID].object.transform.position[otherAxis] - s.first[objID].object.transform.scale[otherAxis];
			if(leftSideS > leftClampI) { leftClampI = leftSideS; }
			leftSide.push_back(leftSideS);
		}
		for(int i = newCenterLeft + 1; i < leftFiltered.size(); i++) {
			int objID = leftFiltered[i];
			float rectMinVal = s.first[objID].object.transform.position[axis] - s.first[objID].object.transform.scale[axis];
			if(std::abs(maxVal - rectMinVal) > EPSILON_RECTS) { break; }
			maxVal = s.first[objID].object.transform.position[axis] + s.first[objID].object.transform.scale[axis];
			leftMove.push_back(objID);
			float leftSideS = s.first[objID].object.transform.position[otherAxis] - s.first[objID].object.transform.scale[otherAxis];
			if(leftSideS > leftClampI) { leftClampI = leftSideS; }
			leftSide.push_back(leftSideS);
		}
		if(minI > minVal) { minI = minVal;}
		if(maxI < maxVal) { maxI = maxVal;}
	}
	
	rightMove.clear();
	rightSide.clear();
	if(rightFiltered.size() != 0) { 
		rightMove.push_back(rightFiltered[newCenterRight]);
		rightClampI = s.first[rightFiltered[newCenterRight]].object.transform.position[otherAxis] + s.first[rightFiltered[newCenterRight]].object.transform.scale[otherAxis];
		rightSide.push_back(rightClampI);
		float minVal = s.first[centerRight].object.transform.position[axis] - s.first[centerRight].object.transform.scale[axis];
		float maxVal = s.first[centerRight].object.transform.position[axis] + s.first[centerRight].object.transform.scale[axis];
		for(int i = newCenterRight - 1; i > -1; i--) {
			int objID = rightFiltered[i];
			float rectMaxVal = s.first[objID].object.transform.position[axis] + s.first[objID].object.transform.scale[axis];
			if(std::abs(minVal - rectMaxVal) > EPSILON_RECTS) { break; }
			minVal = s.first[objID].object.transform.position[axis] - s.first[objID].object.transform.scale[axis];
			rightMove.push_back(objID);
			float rightSideS = s.first[objID].object.transform.position[otherAxis] + s.first[objID].object.transform.scale[otherAxis];
			if(rightSideS < rightClampI) { rightClampI = rightSideS; }
			rightSide.push_back(rightSideS);
		}
		for(int i = newCenterRight + 1; i < rightFiltered.size(); i++) {
			int objID = rightFiltered[i];
			float rectMinVal = s.first[objID].object.transform.position[axis] - s.first[objID].object.transform.scale[axis];
			if(std::abs(maxVal - rectMinVal) > EPSILON_RECTS) { break; }
			maxVal = s.first[objID].object.transform.position[axis] + s.first[objID].object.transform.scale[axis];
			rightMove.push_back(objID);
			float rightSideS = s.first[objID].object.transform.position[otherAxis] + s.first[objID].object.transform.scale[otherAxis];
			if(rightSideS < rightClampI) { rightClampI = rightSideS; }
			rightSide.push_back(rightSideS);
		}
		if(minI > minVal) { minI = minVal;}
		if(maxI < maxVal) { maxI = maxVal;}
	}
	
	if(leftClampI > rightClampI) {std::swap(leftClampI, rightClampI);} 
	
	return Vector4(leftClampI, rightClampI, minI, maxI);
}