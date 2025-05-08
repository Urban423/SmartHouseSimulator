#include "Scene.h"
#include "IOSP.h"
#include "PerlinNoise.h"
#include "ScreenLogic.h"
#include "IOSystem.h"
#include "RenderManager.h"
#include "InputEventSystem.h"
#include "NavMeshSystem.h"
#include "CameraControlSystem.h"
#include "BotLogic.h"
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
	controlablePanels.AddComponent<CameraControlSystem>();
	controlablePanels.AddComponent<RenderView>(0).enabled = false;
	controlablePanels.GetComponent<RenderView>();
	controlablePanels.AddComponent<InputEventSystem>();
	
	
	Object casontrolablePanels 	= ECS::createObject();
	//casontrolablePanels.AddComponent<RenderView>(0).enabled = false;
	
	controlablePanels.AddComponent<ScreenLogic>().controlPanelRender = controlablePanels;
	//controlablePanels.GetComponent<ScreenLogic>().split({ 0.75, 0, 0}, {2, 1, 1});
	//controlablePanels.GetComponent<ScreenLogic>().split({0, -0.75, 0}, {1, 2, 1});
	
	
	Object button = ECS::createObject();
	//button.AddComponent<Camera>().RenderViewDataIndex = 1;
	//button.AddComponent<RenderView>(0);
	button.AddComponent<InputEventSystem>(0);
	Grab& grabObj = button.AddComponent<Grab>(0);
	button.AddComponent<Button>(0).onMouseDown = std::bind(&Grab::grab, grabObj);
	
	
	Object HouseWalls = ECS::createObject();
	HouseWalls.transform.scale = Vector3(0.4f, 0.4f, 0.4f);
	HouseWalls.AddComponent<RenderView>(0).mesh_index = 1;
	HouseWalls.GetComponent<RenderView>().color = Color(0, 0, 0, 0);
	HouseWalls.GetComponent<RenderView>().texture_indexes[0] = 3;
	
	Object Bot = ECS::createObject();
	Bot.transform.scale = Vector3(0.04f, 0.04f, 0.04f);
	Bot.AddComponent<RenderView>(0).texture_indexes[0] = 5;
	Bot.AddComponent<NavMeshAgent>(0).speed = 0.3;
	Bot.AddComponent<BotLogic>(0);
	
	
	Object House = ECS::createObject();
	House.transform.scale = HouseWalls.transform.scale;
	House.AddComponent<RenderView>(0).mesh_index = 2;
	House.GetComponent<RenderView>().shader_indexes.resize(6, 0);
	House.GetComponent<RenderView>().shader_indexes[0] = 0;
	House.GetComponent<RenderView>().texture_indexes.resize(6, 0);
	House.GetComponent<RenderView>().texture_indexes[0] = 0;
	House.GetComponent<RenderView>().texture_indexes[1] = 1;
	House.GetComponent<RenderView>().texture_indexes[2] = 2;
	House.GetComponent<RenderView>().texture_indexes[3] = 3;
	House.GetComponent<RenderView>().texture_indexes[5] = 4;
	
	Object Grass = ECS::createObject();
	Grass.transform.scale = Vector3(10, 10, 10);
	Grass.AddComponent<RenderView>(0).mesh_index = 0;
	Grass.GetComponent<RenderView>().texture_indexes[0] = 4;
	Grass.GetComponent<RenderView>().shader_indexes[0] = 3;
	
	NavMeshSystem::getPtr()->Start();
}


void Scene::Update() {
	//controlablePanels.GetComponent<ScreenLogic>().update();
	NavMeshSystem::getPtr()->Update();
	
	controlablePanels.GetComponent<CameraControlSystem>().UpdateFPSO();
	auto[botLogic, size] = ECS::GetComponents<BotLogic>(0);
	for(int i = 0; i < size; i++) { botLogic[i].Update(); }
}
