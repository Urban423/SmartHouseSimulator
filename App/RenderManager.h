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
	Matrix4x4 calculateCameraView(const Transform& camera);
	void renderCamera(Camera& camera, int renderViewIndex);
private:
	static RenderManager* renderManager;
	
	//shapes
	std::vector<VertexArrayObject*> vertexes;
	std::vector<IndexArrayObject*>  indicies;
	std::vector<Shader*>   shaders;
};