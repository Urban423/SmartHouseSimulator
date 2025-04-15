#include "Scene.h"
#include "IOSP.h"
#include "PerlinNoise.h"
#include "ScreenLogic.h"
#include "IOSystem.h"
#include "RenderManager.h"
#include "InputEventSystem.h"
#include "Lamp.h"

#include <math.h>
#include <cstdio>
#include <cmath>

void Scene::Start() {
	// transforms[i] = { Vector3(0, 0, 0), Quaternion(0, 0, 0, 1), Vector3(1, 1, 1) };
	// collisions[i].offset = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand())).normalized() * 0.1f;
	// collisions[i].radius = static_cast<float>(rand() % 1000) / 20000 + 0.01;
	// transforms[i].scale = Vector3(collisions[i].radius, collisions[i].radius, collisions[i].radius);
	// aabb[i] = {Vector3(-collisions[i].radius, -collisions[i].radius, -collisions[i].radius), Vector3(collisions[i].radius, collisions[i].radius, collisions[i].radius), i};
	// collisions[i].radius /= 2;
	
	
	
	controlablePanels = ECS::createObject();
	controlablePanels.AddComponent<Camera>();
	controlablePanels.AddComponent<RenderView>(0).enabled = false;
	controlablePanels.GetComponent<RenderView>( ).shader_index = 3;
	
	
	Object casontrolablePanels 	= ECS::createObject();
	casontrolablePanels.AddComponent<RenderView>(0);
	
	controlablePanels.AddComponent<ScreenLogic>().controlPanelRender = controlablePanels;
	controlablePanels.GetComponent<ScreenLogic>().split({ 0.75, 0, 0}, {2, 1, 1});
	controlablePanels.GetComponent<ScreenLogic>().split({0, -0.75, 0}, {1, 2, 1});
	
	
	Object button = ECS::createObject();
	//button.AddComponent<Camera>().RenderViewDataIndex = 1;
	button.AddComponent<InputEventSystem>();
	button.AddComponent<RenderView>(1).shader_index = 1;
	button.AddComponent<InputEventSystem>(0);
	
	
	Object lamp = ECS::createObject();
	lamp.AddComponent<RenderView>(1).shader_index = 1;
	lamp.AddComponent<LampComponent>().light = button;
	lamp.GetComponent<LampComponent>().switcher = true;
}


void Scene::Update() {
	controlablePanels.GetComponent<ScreenLogic>().update();
	//for(int i = 0; i < screenLogic.size(); i++) { screenLogic[i].update(); }
}
