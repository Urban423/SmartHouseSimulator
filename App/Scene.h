#pragma once
#include "Transform.h"
#include "Physic.h"
#include "ECS.h"

class Scene {
public:
	void Start();
	void Update();
private:
	Object controlablePanels;
};