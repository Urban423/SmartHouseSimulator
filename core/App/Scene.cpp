#include "Scene.h"
#include "IOSystem.h"
#include "RenderManager.h"
#include "NavMeshSystem.h"
#include "prefabs.h"
#include "TextureManager.h"
#include "BotLogic.h"
#include "Random.h"
#include "MaterialManager.h"
#include "HouseGenerator.h"
#include "ClientServerSystem.h"
#include "Settings.h"
#include "Image.h"

void createMainSimulation() {
	Object cube = ECS::createObject();
	cube.transform.scale = 0.1f;
	cube.AddComponent<RenderView>().mesh_index = MESH_Cube;
	cube.GetComponent<RenderView>().materals[0] = PrefabSystem::getInstance().FloortexMaterial;

	for(int i = 0; i < 12; i++){
		Object entity = PrefabSystem::getInstance().createBall();;
		entity.transform.position = Vector3(22 + i * 2, 15, 4);
		entity.transform.scale = 2;
	}

	for(int i = 0; i < 5; i++) {
		PrefabSystem::getInstance().createBox().transform.position = Vector3(22 + i * 2, 14, -10);
	}

	for(int i = 0; i < 5; i++) {
		PrefabSystem::getInstance().createCylinder().transform.position = Vector3(22 + i * 2, 15, 22);
	}

	for(int i = 0; i < 5; i++) {
		PrefabSystem::getInstance().createCapsule().transform.position = Vector3(22 + i * 3, 15, 12);
	}

	OverlayManager::Create();
	Object player = PrefabSystem::getInstance().createPlayer(true);
	Object mainCamera = player.getChild(0);

	Object terrain = PrefabSystem::getInstance().createTerrain();
	terrain.transform.scale = 4;
	terrain.transform.scale.y = 1;
	terrain.transform.position -= Vector3(50, 0, 50);

	Object text3d = ECS::createObject();
	text3d.transform.scale = 0.2f;
	text3d.transform.position = Vector3(-11, 3.0f, 100);
	text3d.AddComponent<TextView>();
	text3d.GetComponent<TextView>().text = "Sanya is \na super\nmega loser!!!!";
	text3d.GetComponent<TextView>().buildMesh();


	short botMat = MaterialManager::CreateMaterial(Material(SHADER_standartShader, TEX_Ghost1, 0, 0xff));
	short maskMat = MaterialManager::CreateMaterial(Material(SHADER_standartShader, TEX_Ghost, 0, 0xff));
	for (int i = 0; i < 3; i++) {
		Object Bot = ECS::createObject();
		Bot.transform.scale = 1;
		Bot.AddComponent<RenderView>().mesh_index = MESH_Cube;
		Bot.GetComponent<RenderView>().materals[0] = botMat;
		Bot.AddComponent<NavMeshAgent>().speed = 0.01;
		Bot.AddComponent<BotLogic>();

		Object mask = ECS::createObject();
		mask.transform.scale = 1;
		// mask.transform.position = 12;
		mask.AddComponent<RenderView>().mesh_index = 1;
		mask.GetComponent<RenderView>().materals[0] = maskMat;
		mask.setParent(Bot);
	}


	// Object generatable = ECS::createObject();
	// generatable.AddComponent<HouseGenerator>();
	// // generatable.AddComponent<RenderView>(1).mesh_index = generatedMesh.id;
	// // generatable.GetComponent<RenderView>().shader_indexes[0] = 3;
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
	// generatable.AddComponent<RenderView>().materals[0] = MaterialManager::CreateMaterial(Material(0, image.convertToTexture(), 0, 0xff));
}


void Scene::Start() {
	PrefabSystem::getInstance().createMaterials();
	createMainSimulation();
	printf("%d %d %d %d\n", sizeof(RenderView), sizeof(Object), sizeof(Component), sizeof(Transform));
	
	Object panel = ECS::createObject();
	panel.transform.scale = 2;
	panel.AddComponent<RenderView>().layout = 1;
	panel.AddComponent<Camera>().renderLayout = 1;
	panel.GetComponent<RenderView>().materals[0] = PrefabSystem::getInstance().mainMaterial;

	NavMeshSystem::getPtr()->Start();
}

void Scene::Update() {
	if(IOSystem::getWindowResizeFinished()) {
		Rect window = IOSystem::getWindow().getInnerSize();
		Rect renderSize = window * SettingsSystem::GetSettings().resolution;

		int width = renderSize.width();
		int height = renderSize.height();
		Span<Camera> cameras = ECS::GetComponents<Camera>();
		for(auto& cam : cameras) {
			TextureManager::ResizeFrameBuffer(cam.frameBufferIndex, width, height);
		}

		constexpr float REFERENCE_WIDTH  = 1920.0f;
		constexpr float REFERENCE_HEIGHT = 1080.0f;
		float uiScale = std::min(window.width() / REFERENCE_WIDTH, window.height() / REFERENCE_HEIGHT );
		Span<UILayout> uiLayouts = ECS::GetComponents<UILayout>();
		for(auto & layout: uiLayouts) {
			UISystem::getInstance().Rebuild(layout.object, window.width(), window.height(), uiScale);
		}
	}
	InputComponentUpdate();

	NavMeshSystem::getPtr()->Update();

	Span<CameraControlSystem> cameraControlSystems = ECS::GetComponents<CameraControlSystem>();
	for (int i = 0; i < cameraControlSystems.size(); i++) {
		cameraControlSystems[i].Update();
	}

	UISystem::getInstance().Update();
}


void Scene::FixedUpdate() {
	PhysicSystem::getInstance()->calculatePhysic();

	Span<BotLogic> botLogics = ECS::GetComponents<BotLogic>();
	for (int i = 0; i < botLogics.size(); i++) {
		botLogics[i].Update();
	}

	ClientServerSystem::getInstance().FixedUpdate();

	Span<CameraControlSystem> cameraControlSystems = ECS::GetComponents<CameraControlSystem>();
	for (int i = 0; i < cameraControlSystems.size(); i++) {
		cameraControlSystems[i].FixedUpdate();
	}
}