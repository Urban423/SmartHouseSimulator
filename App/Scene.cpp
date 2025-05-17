#include "Scene.h"
#include "IOSP.h"
#include "PerlinNoise.h"
#include "ScreenLogic.h"
#include "IOSystem.h"
#include "RenderManager.h"
#include "InputEventSystem.h"
#include "NavMeshSystem.h"
#include "CameraControlSystem.h"
#include "TextureManager.h"
#include "BotLogic.h"
#include "Lamp.h"

#include <math.h>
#include <cstdio>
#include <cmath>

unsigned int createMainSimulation() {
	Object MainCamera = ECS::createObject();
	MainCamera.AddComponent<CameraControlSystem>();
	MainCamera.AddComponent<Camera>(1).color.r = 1;
	
	
	Object HouseWalls = ECS::createObject();
	HouseWalls.transform.scale = Vector3(0.4f, 0.4f, 0.4f);
	HouseWalls.AddComponent<RenderView>(1).mesh_index = 1;
	HouseWalls.GetComponent<RenderView>().color = Color(0, 0, 0, 0);
	HouseWalls.GetComponent<RenderView>().texture_indexes[0] = 3;
	
	Object Bot = ECS::createObject();
	Bot.transform.scale = Vector3(0.04f, 0.04f, 0.04f);
	Bot.AddComponent<RenderView>(1).texture_indexes[0] = 5;
	Bot.AddComponent<NavMeshAgent>(0).speed = 0.01;
	Bot.AddComponent<BotLogic>(0);
	
	
	Object House = ECS::createObject();
	House.transform.scale = HouseWalls.transform.scale;
	House.AddComponent<RenderView>(1).mesh_index = 2;
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
	Grass.AddComponent<RenderView>(1).mesh_index = 0;
	Grass.GetComponent<RenderView>().texture_indexes[0] = 4;
	Grass.GetComponent<RenderView>().shader_indexes[0] = 3;
	
	
	return MainCamera.GetComponent<Camera>().frameBufferIndex = TextureManager::CreateFrameBuffer();
}

void Scene::Start() {
	// transforms[i] = { Vector3(0, 0, 0), Quaternion(0, 0, 0, 1), Vector3(1, 1, 1) };
	// collisions[i].offset = Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand())).normalized() * 0.1f;
	// collisions[i].radius = static_cast<float>(rand() % 1000) / 20000 + 0.01;
	// transforms[i].scale = Vector3(collisions[i].radius, collisions[i].radius, collisions[i].radius);
	// aabb[i] = {Vector3(-collisions[i].radius, -collisions[i].radius, -collisions[i].radius), Vector3(collisions[i].radius, collisions[i].radius, collisions[i].radius), i};
	// collisions[i].radius /= 2;
	
	
	
	controlablePanels = ECS::createObject();
	controlablePanels.transform.scale = Vector3(0.1f, 0.1f, 0.1f);
	controlablePanels.AddComponent<Camera>();
	controlablePanels.AddComponent<RenderView>(0).enabled = false;
	controlablePanels.GetComponent<RenderView>().shader_indexes[0] = 2;
	controlablePanels.AddComponent<InputEventSystem>();
	
	
	
	Object panel 	= ECS::createObject();
	panel.AddComponent<RenderView>(0).shader_indexes[0] = 1;
	controlablePanels.AddComponent<ScreenLogic>().controlPanelRender = controlablePanels;
	//controlablePanels.GetComponent<ScreenLogic>().split({ 0.75, 0, 0}, {2, 1, 1});
	//controlablePanels.GetComponent<ScreenLogic>().split({0, -0.75, 0}, {1, 2, 1});
	
	// Object button = ECS::createObject();
	// button.AddComponent<Camera>().RenderViewDataIndex = 1;
	// //button.AddComponent<RenderView>(0);
	// button.AddComponent<InputEventSystem>(0);
	// Grab& grabObj = button.AddComponent<Grab>(0);
	// button.AddComponent<Button>(0).onMouseDown = std::bind(&Grab::grab, grabObj);
	panel.GetComponent<RenderView>().texture_indexes[0] = createMainSimulation();
	NavMeshSystem::getPtr()->Start();
}


void Scene::Update() {
	controlablePanels.GetComponent<ScreenLogic>().update();
	NavMeshSystem::getPtr()->Update();
	
	auto[cameraControlSystem, cameraControlSystemSize] = ECS::GetComponents<CameraControlSystem>(0);
	for(int i = 0; i < cameraControlSystemSize; i++) { cameraControlSystem[i].UpdateFPSO(); }
	auto[botLogic, BotLogicSize] = ECS::GetComponents<BotLogic>(0);
	for(int i = 0; i < BotLogicSize; i++) { botLogic[i].Update(); }
	
	auto[inputEventSystem, InputEventSystemLogicSize] = ECS::GetComponents<InputEventSystem>(0);
	for(int i = 0; i < InputEventSystemLogicSize; i++) { inputEventSystem[i].Update(); }
}
