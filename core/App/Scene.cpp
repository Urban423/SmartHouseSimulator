#include "Scene.h"
#include "PerlinNoise.h"
#include "ScreenLogic.h"
#include "IOSystem.h"
#include "RenderManager.h"
#include "InputEventSystem.h"
#include "NavMeshSystem.h"
#include "prefabs.h"
#include "TextureManager.h"
#include "BotLogic.h"
#include "Random.h"
#include "MaterialManager.h"
#include "HouseGenerator.h"
#include "ClientServerSystem.h"
#include "Image.h"

#include <math.h>
#include <cstdio>
#include <cmath>

void createTrees()
{
	float startX = -9;
	float endX = 9;
	float startY = -7;
	float endY = 7;
	float delta = 0.25f;
	float skip_delta = 1.3f;
	short treeMat = MaterialManager::CreateMaterial(Material(0, 10, 0, 0xff));
	short bushMat = MaterialManager::CreateMaterial(Material(0, 11, 0, 0xff));
	for (float i = startY; i < endY; i += delta)
	{
		for (float j = startX; j < endX; j += delta)
		{
			if ((j * j + i * i) < skip_delta * skip_delta)
			{
				continue;
			}
			Object Tree = ECS::createObject();
			float randAngle = noise(0, 6.28);
			Tree.transform.scale = Vector3(0.2f, 0.2f, 0.2f);
			Tree.transform.position = Vector3(j, i, 0) + noise(0, delta / 1.5f) * Vector3(sin(randAngle), cos(randAngle), 0);
			if (rand() & 0x1)
			{
				Tree.AddComponent<RenderView>().materals[0] = treeMat;
			}
			else
			{
				Tree.AddComponent<RenderView>().materals[0] = bushMat;
			}
		}
	}
}

Object createMainSimulation()
{
	// Object generatable = ECS::createObject();
	// generatable.AddComponent<HouseGenerator>();
	// // generatable.AddComponent<RenderView>(1).mesh_index = generatedMesh.id;
	// // generatable.GetComponent<RenderView>().shader_indexes[0] = 3;

	short testmat = MaterialManager::CreateMaterial(Material(0, 3, 0, 0xff));
	Object cube = ECS::createObject();
	cube.transform.scale = 0.1f;
	cube.AddComponent<RenderView>().mesh_index = 1;
	cube.GetComponent<RenderView>().materals[0] = testmat;

	for(int i = 0; i < -2; i++){
		Object sphere = ECS::createObject();
		sphere.transform.position = Vector3(i * 2, 15, 4);
		sphere.AddComponent<SphereCollider>().radius = 1.0f;
		sphere.AddComponent<Rigidbody>().mass ;
		sphere.AddComponent<RenderView>().mesh_index = 2;
		sphere.GetComponent<RenderView>().materals[0] = testmat;
		sphere.AddComponent<NetworkIdentity>();
	}

	for(int i = 0; i < 5; i++){
		Object physCube = ECS::createObject();
		physCube.transform.rotation = Quaternion::FromEuler(0, 0, 0);
		physCube.transform.scale = 1;
		physCube.transform.position = Vector3(-22 , 14+ i * 3, -10);
		physCube.AddComponent<CubeCollider>().size = 2;
		physCube.AddComponent<Rigidbody>();
		physCube.AddComponent<RenderView>().mesh_index = 1;
		physCube.GetComponent<RenderView>().materals[0] = testmat;
		physCube.AddComponent<NetworkIdentity>();
	}

	Object player = PrefabSystem::getInstance().createPlayer(true);
	Object mainCamera = player.getChild(0);

	Mesh perlin = PerlinNoiseMesh(100, 100, 2.0f, 3, 0.9f, 0, 12312);
	short perlinMat = MaterialManager::CreateMaterial(Material(3, 4, 0, 0xff));
	Object perlinMesh = ECS::createObject();
	perlinMesh.transform.scale = 4;
	perlinMesh.transform.scale.y = 1;
	perlinMesh.transform.position -= Vector3(50, 0, 50);
	perlinMesh.AddComponent<RenderView>().mesh_index = perlin.id;
	perlinMesh.GetComponent<RenderView>().materals[0] = perlinMat;
	perlinMesh.AddComponent<TerrainCollider>().width = 100;
	perlinMesh.GetComponent<TerrainCollider>().height = 100;
	for(int i = 0; i < 100 * 100; i++) {
		perlinMesh.GetComponent<TerrainCollider>().heightMap.push_back(perlin.vertex[i].pos.y);
	}


	int ImageWidth = 1024;
	int ImageHeight = 1024;
	Image image(ImageWidth, ImageHeight);
	int nw = 1;
	int nh = 1;
	Layer *layers = new Layer[nw * nh];
	int layerWidth = ImageWidth / nw;
	int layerHeight = ImageHeight / nh;
	for (int i = 0; i < nw * nh; i++)
	{
		layers[i].init(layerWidth * (i % nw), layerHeight * (i / nw), layerWidth, layerHeight);
		srand(i);
		// generatable.GetComponent<HouseGenerator>().GenerateHouse(layers[i]);
		image.addLayer(&layers[i]);
	}
	return mainCamera;
	// generatable.AddComponent<RenderView>().materals[0] = MaterialManager::CreateMaterial(Material(0, image.convertToTexture(), 0, 0xff));

	/*
	Object server = ECS::createObject();
	server.AddComponent<Server>();

	Object lamp = server.GetComponent<Server>().addLamp();
	Object motionSensor = server.GetComponent<Server>().addMotionSensor();
	motionSensor.transform.position = Vector3(-0.3, 0, 0);

	Object HouseWalls = ECS::createObject();
	HouseWalls.transform.scale = Vector3(0.4f, 0.4f, 0.4f);
	HouseWalls.AddComponent<RenderView>(1).mesh_index = 1;
	HouseWalls.GetComponent<RenderView>().color = Color(0, 0, 0, 0);
	HouseWalls.GetComponent<RenderView>().texture_indexes[0] = 3;


	Object sofa = ECS::createObject();
	sofa.transform.scale 	= Vector3(0.15f, 0.075f, 0.06f);
	sofa.transform.position = Vector3(-0.5f, 0.45f, 0);
	sofa.AddComponent<RenderView>(1).texture_indexes[0] = 12;

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

	//createTrees();

	Object Grass = ECS::createObject();
	Grass.transform.scale = Vector3(10, 10, 10);
	Grass.AddComponent<RenderView>(1).mesh_index = 0;
	Grass.GetComponent<RenderView>().texture_indexes[0] = 4;
	Grass.GetComponent<RenderView>().shader_indexes[0] = 3;
	*/
}

void createConsole()
{
	Object consoleText = ECS::createObject();
	consoleText.transform.scale = 0.2f;
	consoleText.transform.position = Vector2(-0.6f, 0.5f);
	consoleText.AddComponent<TextView>().enabled;
	consoleText.GetComponent<TextView>().layout = 2;
	consoleText.GetComponent<TextView>().text = "Sanya is \na super\nmega loser!!!!";
	consoleText.GetComponent<TextView>().buildMesh();

	short botMat = MaterialManager::CreateMaterial(Material(0, 5, 0, 0xff));
	short maskMat = MaterialManager::CreateMaterial(Material(0, 4, 0, 0xff));
	for (int i = 0; i < -3; i++) {
		Object Bot = ECS::createObject();
		Bot.transform.scale = 0.04f;
		Bot.AddComponent<RenderView>().layout = 2;
		Bot.GetComponent<RenderView>().materals[0] = botMat;
		Bot.AddComponent<NavMeshAgent>().speed = 0.01;
		Bot.AddComponent<BotLogic>();

		Object mask = ECS::createObject();
		mask.transform.scale = 5.04f;
		// mask.transform.position = 12;
		mask.AddComponent<RenderView>().mesh_index = 1;
		mask.GetComponent<RenderView>().layout = 2;
		mask.GetComponent<RenderView>().materals[0] = maskMat;
		mask.setParent(Bot);
	}
}

void Scene::Start()
{
	PrefabSystem::getInstance().createMaterials();
	createMainSimulation();
	createConsole();
	printf("%d %d %d %d\n", sizeof(RenderView), sizeof(Object), sizeof(Component), sizeof(Transform));

	short uiBlock = MaterialManager::CreateMaterial(Material(1, 3, 0, 0));

	// Object renderLayerPanel = ECS::createObject();
	// renderLayerPanel.transform.scale = Vector2(0.3, 0.2);
	// renderLayerPanel.AddComponent<RenderView>().layout = 1;
	// renderLayerPanel.GetComponent<RenderView>().materals[0] = uiBlock;

	// Object consoleText = ECS::createObject();
	// consoleText.transform.scale = 0.3f;
	// consoleText.AddComponent<TextView>().layout = 1;
	// consoleText.GetComponent<TextView>().text = "Console";
	// consoleText.setParent(renderLayerPanel);
	// consoleText.GetComponent<TextView>().buildMesh();

	short splitMat = MaterialManager::CreateMaterial(Material(2, 0, 0, 0));
	Object splitLine = ECS::createObject();
	splitLine.transform.scale = Vector3(0.1f, 0.1f, 0.1f);
	splitLine.AddComponent<Camera>().renderLayout = 1;
	splitLine.AddComponent<RenderView>().enabled = false;
	splitLine.GetComponent<RenderView>().layout = 1;
	splitLine.GetComponent<RenderView>().materals[0] = splitMat;
	splitLine.AddComponent<InputEventSystem>();
	splitLine.AddComponent<ScreenLogic>().splitLine = splitLine;
	// controlablePanels.GetComponent<ScreenLogic>().split({ 0.75, 0, 0}, {2, 1, 1});
	// controlablePanels.GetComponent<ScreenLogic>().split({0, -0.75, 0}, {1, 2, 1});

	Object panel = ECS::createObject();
	panel.AddComponent<ScreenBlock>();
	panel.AddComponent<RenderView>().layout = 1;
	panel.GetComponent<RenderView>().materals[0] = PrefabSystem::getInstance().getMainMaterial();

	// Object button = ECS::createObject();
	// button.AddComponent<Camera>().RenderViewDataIndex = 1;
	// //button.AddComponent<RenderView>(0);
	// button.AddComponent<InputEventSystem>(0);
	// Grab& grabObj = button.AddComponent<Grab>(0);
	// button.AddComponent<Button>(0).onMouseDown = std::bind(&Grab::grab, grabObj);
	NavMeshSystem::getPtr()->Start();
	Span<InputEventSystem> inputEventSystems = ECS::GetComponents<InputEventSystem>();
	for (int i = 0; i < inputEventSystems.size(); i++)
	{
		inputEventSystems[i].Update();
	}
}

void Scene::Update() {
	InputComponentUpdate();

	Span<ScreenLogic> screenLogics = ECS::GetComponents<ScreenLogic>();
	for (int i = 0; i < screenLogics.size(); i++) {
		screenLogics[i].update();
	}

	NavMeshSystem::getPtr()->Update();

	Span<CameraControlSystem> cameraControlSystems = ECS::GetComponents<CameraControlSystem>();
	for (int i = 0; i < cameraControlSystems.size(); i++) {
		cameraControlSystems[i].Update();
	}

	Span<InputEventSystem> inputEventSystems = ECS::GetComponents<InputEventSystem>();
	for (int i = 0; i < inputEventSystems.size(); i++) {
		inputEventSystems[i].Update();
	}
}

void Scene::FixedUpdate() {
	PhysicSystem::getInstance()->calculatePhysic();

	Span<BotLogic> botLogics = ECS::GetComponents<BotLogic>();
	for (int i = 0; i < botLogics.size(); i++)
	{
		botLogics[i].Update();
	}

	ClientServerSystem::getInstance().FixedUpdate();

	Span<CameraControlSystem> cameraControlSystems = ECS::GetComponents<CameraControlSystem>();
	for (int i = 0; i < cameraControlSystems.size(); i++) {
		cameraControlSystems[i].FixedUpdate();
	}
}