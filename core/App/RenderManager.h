#pragma once
#include "GraphicsEngine.h"
#include "Transform.h"
#include "Scene.h"
#include <vector>

class RenderManager {
public:
	inline static void onCreate() { renderManager->init(); };
	inline static void onUpdate() { renderManager->Render(); };
private:
	void init();
	void Render();
	Matrix4x4 calculateCameraView(const Transform camera);
	void renderCamera(Camera& camera, int renderViewIndex);
private:
	static RenderManager* renderManager;
	std::vector<Shader*> shaders;
	std::vector<Matrix4x4> worlds;
};

inline Matrix4x4 RenderManager::calculateCameraView(const Transform camera)
{
    Quaternion invRot = Quaternion::Inverse(camera.rotation);

	Matrix4x4 view;
    view.setIdentity();
    view.setRotation(invRot);

    Matrix4x4 trans;
    trans.setIdentity();
    trans.setTranslation(-camera.position);

    view *= trans;
    return view;
}