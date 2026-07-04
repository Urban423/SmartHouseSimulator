#pragma once
#include "Transform.h"
#include "ECS.h"
#include "Physic.h"
#include "CameraControlSystem.h"
#include "TextureManager.h"
#include "MaterialManager.h"

class PrefabSystem {
private:
    short testmat;
	short mainMaterial; 
    int frameBufferMain;
public:
    static PrefabSystem& getInstance() { static PrefabSystem prefabSystem; return prefabSystem; }

    inline void createMaterials() {
        frameBufferMain = TextureManager::CreateFrameBuffer();
        mainMaterial = MaterialManager::CreateMaterial(Material(1, frameBufferMain, 0, 0.1f));
        testmat = MaterialManager::CreateMaterial(Material(0, 3, 0, 0xff));
    }

    inline int getMainMaterial() { return mainMaterial; }

    inline Object createPlayer(bool isClient) {
        Object player = ECS::createObject();

        player.AddComponent<RenderView>().mesh_index = 2;
        player.GetComponent<RenderView>().materals[0] = testmat;
        player.AddComponent<Rigidbody>().isKinematic;
        player.GetComponent<Rigidbody>().angularLock = AxisLock::X | AxisLock::Y | AxisLock::Z;
        // player.GetComponent<Rigidbody>().linearLock = AxisLock::X | AxisLock::Y | AxisLock::Z; 
        player.AddComponent<SphereCollider>();
        player.AddComponent<CameraControlSystem>(); 
        player.AddComponent<InputComponent>(); 
        player.AddComponent<NetworkIdentity>();

        if (isClient) { createCamera(player); }

        return player;
    }
    
    void createCamera(Object player) {
        Object cam = ECS::createObject();

        cam.transform.position = Vector3(0, 0.1f, 0);
        cam.AddComponent<Camera>().color = Color(0.39f, 0.72f, 1.0f);
        cam.GetComponent<Camera>().frameBufferIndex = frameBufferMain;
        cam.GetComponent<Camera>().perpective = true;

        cam.setParent(player);
    }
};