#pragma once
#include "Transform.h"
#include "Physic.h"
#include "ECS.h"
#include "tests.h"

class Scene {
public:
	void Start();
	void Update();
private:
	Object controlablePanels;
};