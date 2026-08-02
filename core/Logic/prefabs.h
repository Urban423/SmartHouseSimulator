#pragma once
#include "Transform.h"
#include "ECS.h"
#include "Physic.h"
#include "CameraControlSystem.h"
#include "TextureManager.h"
#include "UIManager.h"
#include "MaterialManager.h"
#include "ResourceManager.h"
#include "PerlinNoise.h"

class PrefabSystem {
public:
    short boxMaterial;
    short FloortexMaterial;
	short mainMaterial; 
    short perlinMat;
    int frameBufferMain;
public:
    static PrefabSystem& getInstance() { static PrefabSystem prefabSystem; return prefabSystem; }

    inline void createMaterials() {
        Rect winSize = IOSystem::getWindow().getInnerSize();
        float winScale = SettingsSystem::GetSettings().resolution;
        frameBufferMain     = TextureManager::CreateFrameBuffer(winSize.width() * winScale, winSize.height() * winScale);
        perlinMat           = MaterialManager::CreateMaterial(Material(SHADER_terrainShader, TEX_Grass, 0, 0xff));

        mainMaterial        = MaterialManager::CreateMaterial(Material(SHADER_mainPanel, frameBufferMain, 0, 0.1f));
        boxMaterial         = MaterialManager::CreateMaterial(Material(SHADER_standartShader, TEX_box, 0, 0xff));
        FloortexMaterial    = MaterialManager::CreateMaterial(Material(SHADER_standartShader, TEX_Floortex, 0, 0xff));
    }

    inline Object createPlayer(bool isClient) {
        Object player = ECS::createObject();
        player.transform.position = Vector3(0, 0, 20);

        player.AddComponent<RenderView>().mesh_index = 4;
        player.GetComponent<RenderView>().materals[0] = FloortexMaterial;
        player.AddComponent<Rigidbody>().isKinematic;
        player.GetComponent<Rigidbody>().angularLock = AxisLock::X | AxisLock::Y | AxisLock::Z;
        // player.GetComponent<Rigidbody>().linearLock = AxisLock::X | AxisLock::Y | AxisLock::Z; 
        player.AddComponent<CapsuleCollider>().mask = 1;
        player.AddComponent<CameraControlSystem>(); 
        player.AddComponent<InputComponent>(); 
        player.AddComponent<NetworkIdentity>();

        if (isClient) { createCamera(player); }

        return player;
    }
    
    void createCamera(Object player) {
        Object cam = ECS::createObject();

        cam.transform.position = Vector3(0, 0.1f, 0);
        cam.transform.rotation = Quaternion::FromEuler(13, 0, 0);
        cam.AddComponent<Camera>().color = Color(0.39f, 0.72f, 1.0f);
        cam.GetComponent<Camera>().frameBufferIndex = frameBufferMain;
        cam.GetComponent<Camera>().perpective = true;

        cam.setParent(player);
    }

    Object createBox() {
        Object box = ECS::createObject();
		box.AddComponent<CubeCollider>().size = 0.6;
		box.AddComponent<Rigidbody>();
		box.AddComponent<RenderView>().mesh_index = MESH_Box;
		box.GetComponent<RenderView>().materals[0] = boxMaterial;
		box.GetComponent<RenderView>().materals[1] = boxMaterial;
		box.AddComponent<NetworkIdentity>();
        return box;
    }

    Object createBall() {
        Object ball = ECS::createObject();
		ball.AddComponent<SphereCollider>();
		ball.AddComponent<Rigidbody>();
		ball.AddComponent<RenderView>().mesh_index = MESH_Sphere;
		ball.GetComponent<RenderView>().materals[0] = FloortexMaterial;
		ball.AddComponent<NetworkIdentity>();
        return ball;
    }

    Object createCapsule() {
        Object capsule = ECS::createObject();
		capsule.AddComponent<CapsuleCollider>();
		capsule.AddComponent<Rigidbody>();
		capsule.AddComponent<RenderView>().mesh_index = MESH_Capsule;
		capsule.GetComponent<RenderView>().materals[0] = FloortexMaterial;
		capsule.AddComponent<NetworkIdentity>();
        return capsule;
    }

    Object createCylinder() {
        Object cylinder = ECS::createObject();
		cylinder.AddComponent<CubeCollider>();
		cylinder.AddComponent<RenderView>().mesh_index = MESH_Cylinder;
		cylinder.GetComponent<RenderView>().materals[0] = FloortexMaterial;
        return cylinder;
    }

    Object createTerrain(float scale) {
        int perlinMeshID = MeshManager::addMesh(PerlinNoiseMesh(100, 100, 2.0f, 3, 0.9f, scale, 12312));
        Object perlinMesh = ECS::createObject();
        perlinMesh.AddComponent<RenderView>().mesh_index = perlinMeshID;
        perlinMesh.GetComponent<RenderView>().materals[0] = perlinMat;
        perlinMesh.AddComponent<TerrainCollider>().width = 100;
        perlinMesh.GetComponent<TerrainCollider>().height = 100;
        for(int i = 0; i < 100 * 100; i++) {
            perlinMesh.GetComponent<TerrainCollider>().heightMap.push_back(MeshManager::getMeshByID(perlinMeshID).vertices[i].pos.y);
        }
        return perlinMesh;
    }

    Object createWidgetLabel(const char* label, Object widget, bool horizontal = false, Color textColor = 0);
    Object createInputField(const char* startText, size_t maxLength, std::function<bool(char)> charFilter);
    Object createButton(const char* label, std::function<void()> onClickDown);
    Object createSlider(float startStatement, std::function<void(float)> onChangeEnd);
    Object createFixedSlider(float startStatement, int fixedPositions, float minValue, float maxValue, std::function<void(float)> onDrag);
    Object createCheckbox(bool startStatement, std::function<void(bool)> onValueChanged);
    Object createRadioGroup(const std::vector<std::string>& values, int startValue, std::function<void(int)> onValueSelect);
};