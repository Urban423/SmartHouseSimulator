#pragma once
#include "Transform.h"
#include "Physic.h"
#include "ECS.h"
#include "tests.h"

class Scene {
public:
	static void Start();
	static void Update();
	static void FixedUpdate();
};