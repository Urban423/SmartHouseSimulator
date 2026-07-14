#pragma once
#include "Rect.h"
#include "Transform.h"
#include "Vector4.h"
#define EPSILON_RECTS 0.0001
#define EPSILON_RECTS_MOVE 0.025

#define MouseOnFrame 0
#define MouseOnSplit 1
#define MouseOnBoxes 2

struct miniWinData
{
	int axis;
	int pixel;
};

struct rectsCollisionData
{
	Vector4 points;
	Vector4 pointsAxis;
	int axis;
	int spaces;
};

class ScreenLogic : public Component
{
public:
	void update();
	void mouseOnFrameUpdate();
	void mouseOnSplitUpdate();
	void mouseOnBoxesUpdate(Object &newRectangle, bool &newZone);
	void split(Vector3 position, Vector3 scale);
	Object findArea(float x, float y);
	Vector4 fillArea(Vector2 center, int axis);
	void resizeWindows(int width, int height);

public:
	Rect rect = {-1, -1, 1, 1};
	Object splitLine;

	std::vector<int> leftMove, rightMove;
	std::vector<float> leftSide, rightSide;
	float leftClamp;
	float rightClamp;
	Object rectangle;
	Vector2 corner;

	Rect windowSize;
	int mouseState = 0;
	int axis = 0;
	bool relax = true;
	float offsetSize = 0.01f;
	float checkBoxSize = 0.006f;
	float minSize = 0.2;
};