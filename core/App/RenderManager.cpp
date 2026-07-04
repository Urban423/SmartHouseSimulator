#include "RenderManager.h"
#include "IOSystem.h"
#include "TextureManager.h"
#include "ECS.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "PerlinNoise.h"

#include <cstdio>

RenderManager *RenderManager::renderManager = new RenderManager();

void RenderManager::init()
{
	// create texture
	const char *textureFiles[] = {
		"floorWooden1.bmp",	  // 0
		"Tile.bmp",			  // 1
		"Tile1.bmp",		  // 2
		"Floortex.bmp",		  // 3
		"Grass.bmp",		  // 4
		"Ghost.bmp",		  // 5
		"Ghost1.bmp",		  // 6
		"LightBulb.bmp",	  // 7
		"LightBulbOff.bmp",	  // 8
		"MotionSensor.bmp",	  // 9
		"Tree.bmp",			  // 10
		"Bush.bmp",			  // 11
		"Sofa.bmp",			  // 12
		"urok_17_opengl.bmp", // 13
	};
	for (int i = 0; i < sizeof(textureFiles) / sizeof(const char *); i++)
	{
		TextureStruct assa = IOSystem::readBMP(textureFiles[i]);
		TextureManager::CreateTexture(assa);
	}

	// create shader
	const char *shadersFiles[] = {
		"shader.vsh",
		"shader.fsh",

		"shader2.vsh",
		"shader2.fsh",

		"shader3.vsh",
		"shader3.fsh",

		"shader4.vsh",
		"shader4.fsh",

		"shader5.vsh",
		"shader5.fsh",

		"textShader.vsh",
		"textShader.fsh",
	};
	for (int i = 0; i < sizeof(shadersFiles) / sizeof(const char *); i += 2)
	{
		shaders.push_back(GraphicsEngine::createShaderProgram({openCFile(shadersFiles[i]).getPtr(), openCFile(shadersFiles[i + 1]).getPtr()}));
	}

	// create shape points
	const char *files[] = {
		"Plane.fbx",
		"cube.fbx",
		"sphere.fbx",
	};
	for (int i = 0; i < sizeof(files) / sizeof(const char *); i++)
	{
		std::vector<Mesh> meshes = IOSystem::readFBX(files[i]);
	}
}

void RenderManager::renderCamera(Camera &camera, int renderViewIndex)
{
	TextureManager::SetRenderTarget(camera.frameBufferIndex);
	// Matrix4x4 camView = calculateCameraView(camera.object.transform);
	Matrix4x4 camView = worlds[camera.object.getID()];
	camView.inverse();
	Matrix4x4 projection;
	projection.setIdentity();
	int width, height;
	if (camera.frameBufferIndex == -1) {
		std::tie(width, height) = IOSystem::getWindowSize();
	}
	else {
		std::tie(width, height) = TextureManager::GetTextureSize(camera.frameBufferIndex);
	}
	if(camera.perpective)
		projection.setPerspectiveFovLH(3.14f / 4, (float)width / height, 0.01f, 1000);
	else
		projection.setOrthoLH((float)width * camera.focalLength, (float)height * camera.focalLength, -4, 4);
	GraphicsEngine::setViewPort(0, 0, width, height);
	GraphicsEngine::clear(camera.color);
	GraphicsEngine::clearColorDepthBuffer();

	Span<RenderView> renderViews = ECS::GetComponents<RenderView>();
	for (int i = renderViews.size() - 1; i > -1; i--)
	{
		if (renderViews[i].layout != renderViewIndex) continue;
		if (renderViews[i].enabled == false) continue;

		// set shape
		unsigned int number_of_mats = MeshManager::setMeshById(renderViews[i].mesh_index);

		// render object
		int number_of_materials = Math::Min(number_of_mats, (unsigned int)renderViews[i].materals.size());
		int offset = 0;
		for (int j = 0; j < number_of_materials; j++)
		{
			Material material = MaterialManager::Get(renderViews[i].materals[j]);
			int shader_index = material.shader_indexes;
			int texture_index = material.texture_index;
			int texture_index1 = material.texture_index1;
			int mesh_index = renderViews[i].mesh_index;
			Shader *shader_ptr = shaders[shader_index];
			// set material
			GraphicsEngine::setShaderProgram(shader_ptr);
			GraphicsEngine::setTime(shader_ptr, Time::time);
			GraphicsEngine::setProjectionMatrix(shader_ptr, projection);
			GraphicsEngine::setCameraViewMatrix(shader_ptr, camView);

			GraphicsEngine::setTexture(TextureManager::GetTextureByID(texture_index), shader_ptr);
			GraphicsEngine::setVector4(shader_ptr, material.color.ToVector4());
			GraphicsEngine::setMatrix(shader_ptr, worlds[renderViews[i].object.getID()]);

			int number_of_triangles = MeshManager::getNumberOfPolygonsByMaterialID(mesh_index, j);
			GraphicsEngine::drawTriangles(number_of_triangles, (void *)(offset * sizeof(int)));
			offset += number_of_triangles;
		}
	}

	Span<TextView> textViews = ECS::GetComponents<TextView>();
	int text_shader_index = 5;
	int atlas_index = 13;
	GraphicsEngine::setShaderProgram(shaders[text_shader_index]);
	GraphicsEngine::setProjectionMatrix(shaders[text_shader_index], projection);
	GraphicsEngine::setCameraViewMatrix(shaders[text_shader_index], camView);
	GraphicsEngine::setTexture(TextureManager::GetTextureByID(atlas_index), shaders[text_shader_index]);
	for (int i = textViews.size() - 1; i > -1; i--)
	{
		if (textViews[i].layout != renderViewIndex)
			continue;
		if (textViews[i].enabled == false)
			continue;

		// set shape
		int mesh_index = textViews[i].getId();

		// set material
		unsigned int number_of_mats = MeshManager::setMeshById(mesh_index);
		GraphicsEngine::setMatrix(shaders[text_shader_index], worlds[textViews[i].object.getID()]);
		int number_of_triangles = MeshManager::getNumberOfPolygonsByMaterialID(mesh_index, 0);
		GraphicsEngine::drawTriangles(number_of_triangles, (void *)(0 * sizeof(int)));
	}

	//2d ui

}

// #include <chrono>
// float delta = 0;
// int n = 0;
void RenderManager::Render()
{
	// calcualte matrxies
	auto [hierarchy, hierarchySize] = ECS::GetHierarchy().rebuild();
	auto [parents, parentsSize] = ECS::GetHierarchy().getParents();
	for (int i = 0; i < hierarchySize; i++)
	{
		int objectID = hierarchy[i];
		if (worlds.size() <= objectID)
		{
			worlds.resize(objectID + 1);
		}
		Transform transform = ECS::GetComponent<Transform>(objectID);
		Matrix4x4 S, R, T;
		S.setIdentity();
		S.setScale(transform.scale);

		R.setIdentity();
		R.setRotation(transform.rotation);

		T.setIdentity();
		T.setTranslation(transform.position);

		Matrix4x4 local = T * R * S;
		int parent = parents[objectID];
		if (parent != -1) {
			worlds[objectID] = worlds[parent] * local;
		}
		else {
			worlds[objectID] = local;
		}
	}

	GraphicsEngine::disable3D();
	Span<Camera> cameras = ECS::GetComponents<Camera>();
	// auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < cameras.size(); i++)
	{
		renderCamera(cameras[i], cameras[i].renderLayout);
	}

	// auto end = std::chrono::high_resolution_clock::now();
	// float ms = std::chrono::duration<float, std::milli>(end - start).count();
	// delta += ms;
	// n += 1;
	// printf("Render time: %.3f ms\n", delta / n);
}