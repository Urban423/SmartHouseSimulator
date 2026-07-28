#pragma once
#include "GraphicsEngine.h"
#include "Transform.h"
#include "Scene.h"

class RenderManager {
public:
	inline static void onCreate() { renderManager->init(); };
	inline static void onUpdate() { renderManager->Render(); };
private:
	void init();
	void Render();
	void renderCamera(Camera& camera, int renderViewIndex);
	void renderUI(int renderViewIndex);
private:
	static RenderManager* renderManager;
	std::vector<Shader*> shaders;
	std::vector<Matrix4x4> worlds;
};