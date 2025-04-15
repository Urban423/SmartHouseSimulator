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
	void calculateCameraView(const Transform& camera, Camera& cameraData);
private:
	static RenderManager* renderManager;
	
	//shapes
	std::vector<VertexArrayObject*> vertexes;
	std::vector<IndexArrayObject*>  indicies;
	std::vector<Shader*>   shaders;
	std::vector<Texture*>  textures;
};