#include "ScreenLogic.h"
#include "GraphicsEngine.h"
#include "ECS.h"
#include "MaterialManager.h"
#include "IOSystem.h"
#include "TextureManager.h"
#include <algorithm>

Vector2 getMouseScene() {
	Vector2 mousePos = IOSystem::getInput().pointerPosition;
	Rect winRect = IOSystem::getWindow().getInnerSize();
	Vector2 sceneMousePos = {(mousePos.x - winRect.left) / (winRect.right - winRect.left), 1 - (mousePos.y - winRect.top) / (winRect.bottom - winRect.top)};
	sceneMousePos *= 2;
	sceneMousePos.x -= 1;
	sceneMousePos.y -= 1;
	// printf("%f %f\n", (mousePos.x - winRect.left) / (winRect.right - winRect.left), 1 - (mousePos.y - winRect.top) / (winRect.bottom - winRect.top));
	return sceneMousePos;
}

void createObjectCopy(Vector3 newPos, Vector3 newScale, Object original)
{
	float light = 0.4;
	Object newCamera = ECS::createObject();
	newCamera.AddComponent<Camera>().color = Color(light, light, light);
	newCamera.GetComponent<Camera>().renderLayout = 2;
	newCamera.GetComponent<Camera>().frameBufferIndex = TextureManager::CreateFrameBuffer();

	Material copyMat = MaterialManager::Get(original.GetComponent<RenderView>().materals[0]);
	copyMat.texture_index = newCamera.GetComponent<Camera>().frameBufferIndex;

	Object copy = ECS::createObject();
	copy.transform = {newPos, Quaternion(0, 0, 0, 1), newScale};
	copy.AddComponent<ScreenBlock>();
	copy.AddComponent<RenderView>().layout = original.GetComponent<RenderView>().layout;
	copy.GetComponent<RenderView>().materals[0] = MaterialManager::CreateMaterial(copyMat);
}

bool areNeighbors(int &axis, Vector3 posA, Vector3 scaleA, Vector3 posB, Vector3 scaleB)
{
	Vector2 box1_min = Vector2(posA.x - scaleA.x, posA.y - scaleA.y);
	Vector2 box1_max = Vector2(posA.x + scaleA.x, posA.y + scaleA.y);
	Vector2 box2_min = Vector2(posB.x - scaleB.x, posB.y - scaleB.y);
	Vector2 box2_max = Vector2(posB.x + scaleB.x, posB.y + scaleB.y);

	float distX = std::min(box1_max.x, box2_max.x) - std::max(box1_min.x, box2_min.x);
	float distY = std::min(box1_max.y, box2_max.y) - std::max(box1_min.y, box2_min.y);
	axis = 0;
	if (distX > distY)
	{
		axis = 1;
		std::swap(distX, distY);
	}
	return std::abs(distX) < EPSILON_RECTS;
}

rectsCollisionData getCollisionPoints(Vector3 posA, Vector3 scaleA, Vector3 posB, Vector3 scaleB)
{
	Vector2 box1_min = Vector2(posA.x - scaleA.x, posA.y - scaleA.y);
	Vector2 box1_max = Vector2(posA.x + scaleA.x, posA.y + scaleA.y);
	Vector2 box2_min = Vector2(posB.x - scaleB.x, posB.y - scaleB.y);
	Vector2 box2_max = Vector2(posB.x + scaleB.x, posB.y + scaleB.y);

	float distX = std::min(box1_max.x, box2_max.x) - std::max(box1_min.x, box2_min.x);
	float distY = std::min(box1_max.y, box2_max.y) - std::max(box1_min.y, box2_min.y);
	int axis = 0;
	if (distX > distY)
	{
		std::swap(distX, distY);
		axis = 1;
	}

	float points[] = {box1_min[!axis], box1_max[!axis], box2_min[!axis], box2_max[!axis]};
	float pointsAxis[] = {box1_min[axis], box1_max[axis], box2_min[axis], box2_max[axis]};
	std::sort(std::begin(points), std::end(points));
	std::sort(std::begin(pointsAxis), std::end(pointsAxis));
	return {{points[0], points[1], points[2], points[3]}, {pointsAxis[0], pointsAxis[1], pointsAxis[2], pointsAxis[3]}, axis, (points[0] == box2_min[!axis]) | ((points[3] == box2_max[!axis]) << 1)};
}

void ScreenLogic::resizeWindows(int width, int height)
{
	auto [renderViews, size] = ECS::GetComponents<RenderView>();
	if (size == 0) return;
	for (int i = 1; i < size; i++) {
		TextureManager::ResizeFrameBuffer(
			MaterialManager::Get(renderViews[i].materals[0]).texture_index,
			renderViews[i].object.transform.scale.x * width,
			renderViews[i].object.transform.scale.y * height
		);
	}
}

void ScreenLogic::mouseOnFrameUpdate()
{
	if (!IOSystem::getInput().pointerHold) return;
	Vector2 MousePos = getMouseScene();
	rectangle = findArea(MousePos.x, MousePos.y);

	float rectPosX = rectangle.transform.position.x;
	float rectPosY = rectangle.transform.position.y;
	float rectSizeX = rectangle.transform.scale.x;
	float rectSizeY = rectangle.transform.scale.y;
	float dx = MousePos.x - rectPosX;
	float dy = MousePos.y - rectPosY;
	corner.x = (dx < 0) ? rectPosX - rectSizeX : rectPosX + rectSizeX;
	corner.y = (dy < 0) ? rectPosY - rectSizeY : rectPosY + rectSizeY;
	if (Vector2::DistanceSquare(MousePos, corner) < checkBoxSize)
	{
		mouseState = MouseOnBoxes;
		return;
	};

	Vector2 closestSideCenter = corner;
	float deltaX = std::abs(MousePos.x - corner.x);
	float deltaY = std::abs(MousePos.y - corner.y);
	axis = 0;
	if (deltaX > deltaY)
	{
		axis = 1;
		closestSideCenter.x = rectangle.transform.position.x;
	}
	else
	{
		closestSideCenter.y = rectangle.transform.position.y;
	}
	if ((deltaX < 3 * checkBoxSize || deltaY < 3 * checkBoxSize) && closestSideCenter[axis] != rect[0][axis] && closestSideCenter[axis] != rect[3][axis])
	{
		mouseState = MouseOnSplit;
		Vector4 tempClamp = fillArea(closestSideCenter, !axis);
		leftClamp = tempClamp.x;
		rightClamp = tempClamp.y;
		splitLine.transform.scale[axis] = offsetSize;
		splitLine.transform.scale[!axis] = (tempClamp.w - tempClamp.z) / 2;
		splitLine.transform.position[!axis] = (tempClamp.w + tempClamp.z) / 2;
		MaterialManager::Get(splitLine.GetComponent<RenderView>().materals[0]).color = Color(0.7, axis ? 0 : 0.5, axis ? 0.5 : 0);
	}
}

void ScreenLogic::mouseOnBoxesUpdate(Object &newRectangle, bool &newZone)
{
	Vector2 MousePos = getMouseScene();
	splitLine.transform.scale = Vector3(offsetSize, offsetSize, offsetSize);
	splitLine.GetComponent<RenderView>().enabled = false;
	newRectangle = findArea(MousePos.x, MousePos.y);
	if (newRectangle == -1)
	{
		return;
	}
	else if (newRectangle != rectangle)
	{
		splitLine.transform.position = newRectangle.transform.position;
		splitLine.transform.scale = newRectangle.transform.scale;
		newZone = areNeighbors(axis, rectangle.transform.position, rectangle.transform.scale, newRectangle.transform.position, newRectangle.transform.scale);
		splitLine.GetComponent<RenderView>().enabled = newZone;
		MaterialManager::Get(splitLine.GetComponent<RenderView>().materals[0]).color = Color(0.7, 0.0, 0.0);
	}
	else
	{
		axis = 0;
		float deltaX = std::abs(MousePos.x - corner.x);
		float deltaY = std::abs(MousePos.y - corner.y);
		if (deltaY < deltaX) axis = 1;
		float delta = MousePos[!axis] - newRectangle.transform.position[!axis];
		float closestSide = (delta < 0) ? newRectangle.transform.position[!axis] - newRectangle.transform.scale[!axis] : newRectangle.transform.position[!axis] + newRectangle.transform.scale[!axis];

		if (std::abs(MousePos[!axis] - closestSide) < minSize) {
			if (IOSystem::getInput().pointerPressed) {
				mouseState = MouseOnFrame;
			}
			return;
		}

		splitLine.transform.position = newRectangle.transform.position;
		splitLine.transform.position[!axis] = MousePos[!axis];
		splitLine.transform.scale[axis] = newRectangle.transform.scale[axis];
		splitLine.GetComponent<RenderView>().enabled = true;
		MaterialManager::Get(splitLine.GetComponent<RenderView>().materals[0]).color = Color(0.0, !axis ? 0.5 : 0, !axis ? 0 : 0.5);
	}
}

void ScreenLogic::mouseOnSplitUpdate()
{
	Vector2 MousePos = getMouseScene();
	splitLine.GetComponent<RenderView>().enabled = true;
	MousePos[axis] = clamp(leftClamp + minSize, rightClamp - minSize, MousePos[axis]);
	splitLine.transform.position[axis] = MousePos[axis];
	MaterialManager::Get(splitLine.GetComponent<RenderView>().materals[0]).color = Color(1.0, axis ? 0.5 : 0, axis ? 0 : 0.5);
	for (int i = 0; i < leftMove.size(); i++)
	{
		int objID = leftMove[i];
		// printf("l: %d\n", objID);
		Object obj = ECS::getObjectByID(objID);
		obj.transform.position[axis] = (leftSide[i] + MousePos[axis]) / 2;
		obj.transform.scale[axis] = std::abs(leftSide[i] - MousePos[axis]) / 2;
	}
	for (int i = 0; i < rightMove.size(); i++)
	{
		int objID = rightMove[i];
		// printf("r: %d\n", objID);
		Object obj = ECS::getObjectByID(objID);
		obj.transform.position[axis] = (rightSide[i] + MousePos[axis]) / 2;
		obj.transform.scale[axis] = std::abs(rightSide[i] - MousePos[axis]) / 2;
	}
	// printf("\n");
	auto [width, height] = IOSystem::getWindowSize();
	resizeWindows(width, height);
}

void ScreenLogic::update()
{
	if (mouseState == MouseOnFrame)
	{
		mouseOnFrameUpdate();
		return;
	}

	bool newZone = false;
	Object newRectangle;
	if (mouseState == MouseOnBoxes) {
		mouseOnBoxesUpdate(newRectangle, newZone);
	}
	else if (mouseState == MouseOnSplit) {
		mouseOnSplitUpdate();
	}

	if (IOSystem::getInput().pointerReleased) {
		if (newZone) {
			if (newRectangle.transform.position[axis] < rectangle.transform.position[axis])
			{
				std::swap(rectangle, newRectangle);
				std::swap(rectangle.GetComponent<RenderView>().materals[0], newRectangle.GetComponent<RenderView>().materals[0]);
			}
			rectsCollisionData collisionData = getCollisionPoints(rectangle.transform.position, rectangle.transform.scale, newRectangle.transform.position, newRectangle.transform.scale);
			float zones[] = {
				collisionData.points[1] - collisionData.points[0],
				collisionData.points[2] - collisionData.points[1],
				collisionData.points[3] - collisionData.points[2]};
			int holes = (zones[0] > EPSILON_RECTS) | ((zones[2] > EPSILON_RECTS) << 1);
			bool check = (zones[1] >= minSize) && (zones[0] >= minSize || !(holes & 0x1)) && (zones[2] >= minSize || !(holes & 0x2));
			if (check == 0)
			{
				splitLine.GetComponent<RenderView>().enabled = false;
				mouseState = MouseOnFrame;
				return;
			}

			rectangle.transform.position[axis] = (collisionData.pointsAxis[3] + collisionData.pointsAxis[0]) / 2;
			rectangle.transform.position[!axis] = (collisionData.points[2] + collisionData.points[1]) / 2;
			rectangle.transform.scale[axis] = (collisionData.pointsAxis[3] - collisionData.pointsAxis[0]) / 2;
			rectangle.transform.scale[!axis] = zones[1] / 2;

			if (holes == 0)
			{
				ECS::deleteObject(newRectangle);
				splitLine.GetComponent<RenderView>().enabled = false;
				mouseState = MouseOnFrame;
				auto [width, height] = IOSystem::getWindowSize();
				resizeWindows(width, height);
				return;
			}
			else if (holes == 3)
			{
				int holeIndex2 = (holes & 0x1);
				int zoneIndex2 = 2 * ((collisionData.spaces >> holeIndex2) & 0x1);
				holeIndex2 *= 2;

				Vector3 newPos, newScale;
				newPos[axis] = (collisionData.pointsAxis[zoneIndex2 + 1] + collisionData.pointsAxis[zoneIndex2]) / 2;
				newPos[!axis] = (collisionData.points[holeIndex2 + 1] + collisionData.points[holeIndex2]) / 2;
				newScale[axis] = (collisionData.pointsAxis[zoneIndex2 + 1] - collisionData.pointsAxis[zoneIndex2]) / 2;
				newScale[!axis] = zones[holeIndex2] / 2;

				createObjectCopy(newPos, newScale, rectangle);
			}

			int holeIndex1 = !(holes & 0x1);
			int zoneIndex1 = 2 * ((collisionData.spaces >> holeIndex1) & 0x1);
			holeIndex1 *= 2;

			newRectangle.transform.position[axis] = (collisionData.pointsAxis[zoneIndex1 + 1] + collisionData.pointsAxis[zoneIndex1]) / 2;
			newRectangle.transform.position[!axis] = (collisionData.points[holeIndex1 + 1] + collisionData.points[holeIndex1]) / 2;
			newRectangle.transform.scale[axis] = (collisionData.pointsAxis[zoneIndex1 + 1] - collisionData.pointsAxis[zoneIndex1]) / 2;
			newRectangle.transform.scale[!axis] = zones[holeIndex1] / 2;
		}
		else if (mouseState == MouseOnBoxes && splitLine.GetComponent<RenderView>().enabled)
		{
			Vector3 pointEdge1 = rectangle.transform.position - rectangle.transform.scale * Vector3(axis, !axis, 0);
			Vector3 pointEdge2 = 2 * rectangle.transform.position - pointEdge1;
			Vector3 newPos = (splitLine.transform.position + pointEdge2) / 2;
			Vector3 newScale = splitLine.transform.scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge2, newPos) * Vector3(axis, !axis, 0);
			rectangle.transform.position = (splitLine.transform.position + pointEdge1) / 2;
			rectangle.transform.scale = splitLine.transform.scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge1, rectangle.transform.position) * Vector3(axis, !axis, 0);

			createObjectCopy(newPos, newScale, rectangle);
		}
		splitLine.GetComponent<RenderView>().enabled = false;
		mouseState = MouseOnFrame;
		auto [width, height] = IOSystem::getWindowSize();
		resizeWindows(width, height);
	}
}

Object ScreenLogic::findArea(float x, float y)
{
	auto [screenBlocks, size] = ECS::GetComponents<ScreenBlock>();
	if (size == 0)
		return Object();
	for (int i = 0; i < size; i++)
	{
		float posX = screenBlocks[i].object.transform.position.x;
		float posY = screenBlocks[i].object.transform.position.y;
		float width = screenBlocks[i].object.transform.scale.x;
		float height = screenBlocks[i].object.transform.scale.y;
		if (
			(posX - width <= x && x <= posX + width) &&
			(posY - height <= y && y <= posY + height))
		{
			return screenBlocks[i].object;
		}
	}
	return Object();
}

void ScreenLogic::split(Vector3 position, Vector3 scale)
{
	Object rectangle = findArea(position.x, position.y);
	axis = scale.y < scale.x;
	position[axis] = rectangle.transform.position[axis];
	scale[axis] = rectangle.transform.scale[axis];

	Vector3 pointEdge1 = rectangle.transform.position - rectangle.transform.scale * Vector3(axis, !axis, 0);
	Vector3 pointEdge2 = 2 * rectangle.transform.position - pointEdge1;
	Vector3 newPos = (position + pointEdge2) / 2;
	Vector3 newScale = scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge2, newPos) * Vector3(axis, !axis, 0);
	rectangle.transform.position = (position + pointEdge1) / 2;
	rectangle.transform.scale = scale * Vector3(!axis, axis, 0) + Vector3::Distance(pointEdge1, rectangle.transform.position) * Vector3(axis, !axis, 0);

	createObjectCopy(newPos, newScale, rectangle);
	auto [width, height] = IOSystem::getWindowSize();
	resizeWindows(width, height);
}

bool checkAxis(const Transform &transform, const Vector2 &center, int axis)
{
	float left = transform.position[axis] - transform.scale[axis];
	float right = transform.position[axis] + transform.scale[axis];
	return std::abs(center[axis] - left) < EPSILON_RECTS_MOVE || std::abs(center[axis] - right) < EPSILON_RECTS_MOVE;
}

Vector4 ScreenLogic::fillArea(Vector2 center, int axis)
{
	int otherAxis = !axis;
	float leftClampI = rect[0][otherAxis];
	float rightClampI = rect[3][otherAxis];
	std::vector<int> leftFiltered, rightFiltered;
	int centerLeft = 0;
	int centerRight = 0;
	float minDistLeft = std::numeric_limits<float>::max();
	float minDistRight = std::numeric_limits<float>::max();
	auto [screenBlocks, size] = ECS::GetComponents<ScreenBlock>();
	for (int i = 0; i < size; i++)
	{
		if (!checkAxis(screenBlocks[i].object.transform, center, otherAxis))
			continue;
		float dist = std::abs(center[axis] - screenBlocks[i].object.transform.position[axis]);
		if (screenBlocks[i].object.transform.position[otherAxis] < center[otherAxis])
		{
			leftFiltered.push_back(i);
			if (dist < minDistLeft)
			{
				centerLeft = i;
				minDistLeft = dist;
			}
		}
		else
		{
			rightFiltered.push_back(i);
			if (dist < minDistRight)
			{
				centerRight = i;
				minDistRight = dist;
			}
		}
	}

	std::sort(leftFiltered.begin(), leftFiltered.end(), [&](int a, int b)
			  { return screenBlocks[a].object.transform.position[axis] < screenBlocks[b].object.transform.position[axis]; });

	std::sort(rightFiltered.begin(), rightFiltered.end(), [&](int a, int b)
			  { return screenBlocks[a].object.transform.position[axis] < screenBlocks[b].object.transform.position[axis]; });

	int newCenterLeft = 0;
	int newCenterRight = 0;
	for (int i = 0; i < leftFiltered.size(); i++)
	{
		if (centerLeft == leftFiltered[i])
		{
			newCenterLeft = i;
			break;
		}
	}
	for (int i = 0; i < rightFiltered.size(); i++)
	{
		if (centerRight == rightFiltered[i])
		{
			newCenterRight = i;
			break;
		}
	}
	float minI = 1;
	float maxI = -1;

	leftMove.clear();
	leftSide.clear();
	if (leftFiltered.size() != 0)
	{
		leftMove.push_back(screenBlocks[leftFiltered[newCenterLeft]].object.getID());
		leftClampI = screenBlocks[leftFiltered[newCenterLeft]].object.transform.position[otherAxis] - screenBlocks[leftFiltered[newCenterLeft]].object.transform.scale[otherAxis];
		leftSide.push_back(leftClampI);
		float minVal = screenBlocks[centerLeft].object.transform.position[axis] - screenBlocks[centerLeft].object.transform.scale[axis];
		float maxVal = screenBlocks[centerLeft].object.transform.position[axis] + screenBlocks[centerLeft].object.transform.scale[axis];
		for (int i = newCenterLeft - 1; i > -1; i--)
		{
			Object obj = screenBlocks[leftFiltered[i]].object;
			int objID = obj.getID();
			float rectMaxVal = obj.transform.position[axis] + obj.transform.scale[axis];
			if (std::abs(minVal - rectMaxVal) > EPSILON_RECTS)
			{
				break;
			}
			minVal = obj.transform.position[axis] - obj.transform.scale[axis];
			leftMove.push_back(objID);
			float leftSideS = obj.transform.position[otherAxis] - obj.transform.scale[otherAxis];
			if (leftSideS > leftClampI)
			{
				leftClampI = leftSideS;
			}
			leftSide.push_back(leftSideS);
		}
		for (int i = newCenterLeft + 1; i < leftFiltered.size(); i++)
		{
			Object obj = screenBlocks[leftFiltered[i]].object;
			int objID = obj.getID();
			float rectMinVal = obj.transform.position[axis] - obj.transform.scale[axis];
			if (std::abs(maxVal - rectMinVal) > EPSILON_RECTS)
			{
				break;
			}
			maxVal = obj.transform.position[axis] + obj.transform.scale[axis];
			leftMove.push_back(objID);
			float leftSideS = obj.transform.position[otherAxis] - obj.transform.scale[otherAxis];
			if (leftSideS > leftClampI)
			{
				leftClampI = leftSideS;
			}
			leftSide.push_back(leftSideS);
		}
		if (minI > minVal)
		{
			minI = minVal;
		}
		if (maxI < maxVal)
		{
			maxI = maxVal;
		}
	}

	rightMove.clear();
	rightSide.clear();
	if (rightFiltered.size() != 0)
	{
		rightMove.push_back(screenBlocks[rightFiltered[newCenterRight]].object.getID());
		rightClampI = screenBlocks[rightFiltered[newCenterRight]].object.transform.position[otherAxis] + screenBlocks[rightFiltered[newCenterRight]].object.transform.scale[otherAxis];
		rightSide.push_back(rightClampI);
		float minVal = screenBlocks[centerRight].object.transform.position[axis] - screenBlocks[centerRight].object.transform.scale[axis];
		float maxVal = screenBlocks[centerRight].object.transform.position[axis] + screenBlocks[centerRight].object.transform.scale[axis];
		for (int i = newCenterRight - 1; i > -1; i--)
		{
			Object obj = screenBlocks[rightFiltered[i]].object;
			int objID = obj.getID();
			float rectMaxVal = obj.transform.position[axis] + obj.transform.scale[axis];
			if (std::abs(minVal - rectMaxVal) > EPSILON_RECTS)
			{
				break;
			}
			minVal = obj.transform.position[axis] - obj.transform.scale[axis];
			rightMove.push_back(objID);
			float rightSideS = obj.transform.position[otherAxis] + obj.transform.scale[otherAxis];
			if (rightSideS < rightClampI)
			{
				rightClampI = rightSideS;
			}
			rightSide.push_back(rightSideS);
		}
		for (int i = newCenterRight + 1; i < rightFiltered.size(); i++)
		{
			Object obj = screenBlocks[rightFiltered[i]].object;
			int objID = obj.getID();
			float rectMinVal = obj.transform.position[axis] - obj.transform.scale[axis];
			if (std::abs(maxVal - rectMinVal) > EPSILON_RECTS)
			{
				break;
			}
			maxVal = obj.transform.position[axis] + obj.transform.scale[axis];
			rightMove.push_back(objID);
			float rightSideS = obj.transform.position[otherAxis] + obj.transform.scale[otherAxis];
			if (rightSideS < rightClampI)
			{
				rightClampI = rightSideS;
			}
			rightSide.push_back(rightSideS);
		}
		if (minI > minVal)
		{
			minI = minVal;
		}
		if (maxI < maxVal)
		{
			maxI = maxVal;
		}
	}

	if (leftClampI > rightClampI)
	{
		std::swap(leftClampI, rightClampI);
	}

	return Vector4(leftClampI, rightClampI, minI, maxI);
}