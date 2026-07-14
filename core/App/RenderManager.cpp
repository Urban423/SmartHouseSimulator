#include "RenderManager.h"
#include "IOSystem.h"
#include "TextureManager.h"
#include "ECS.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "PerlinNoise.h"
#include "UIManager.h"

RenderManager *RenderManager::renderManager = new RenderManager();

void RenderManager::init() {
	Color white(255);
	TextureStruct whiteTexture;
	whiteTexture.width = 1;
	whiteTexture.height = 1;
	whiteTexture.pixels = (int*)&white;
	TextureManager::CreateTexture(whiteTexture);

	// create texture
	const char *textureFiles[] = {
		"Textures/Tile.png",			  	// 1
		"Textures/Tile1.png",		  		// 2
		"Textures/Floortex.png",		  	// 3
		"Textures/Grass.png",		  		// 4
		"Textures/Ghost.png",		  		// 5
		"Textures/Ghost1.png",		  		// 6
		"Textures/LightBulb.png",	  		// 7
		"Textures/LightBulbOff.png",	  	// 8
		"Textures/MotionSensor.png",	  	// 9
		"Textures/Tree.png",			  	// 10
		"Textures/Bush.png",			  	// 11
		"Textures/Sofa.png",			  	// 12
		"Textures/urok_17_opengl.png", 		// 13
		"Textures/floorWooden1.png",	  	// 14
	};
	int textureNumber = sizeof(textureFiles) / sizeof(const char *);
	std::vector<TextureStruct> textures;
	textures.reserve(textureNumber);
	for (int i = 0; i < textureNumber; i++) {
		TextureStruct temp;
		if(!IOSystem::readImage(temp, textureFiles[i])) continue;
		TextureManager::CreateTexture(temp);
		textures.push_back(temp);
	}

	// create shader
	const char *shadersFiles[] = {
		"Shaders/shader.vsh",
		"Shaders/shader.fsh",

		"Shaders/shader2.vsh",
		"Shaders/shader2.fsh",

		"Shaders/shader3.vsh",
		"Shaders/shader3.fsh",

		"Shaders/shader4.vsh",
		"Shaders/shader4.fsh",

		"Shaders/shader5.vsh",
		"Shaders/shader5.fsh",

		"Shaders/textShader.vsh",
		"Shaders/textShader.fsh",

		"Shaders/uiShader.vsh",
		"Shaders/uiShader.fsh",
	};
	for (int i = 0; i < sizeof(shadersFiles) / sizeof(const char *); i += 2)
	{
		shaders.push_back(GraphicsEngine::createShaderProgram({openCFile(shadersFiles[i]).getPtr(), openCFile(shadersFiles[i + 1]).getPtr()}));
	}

	// create shape points

	Vertex planePoints[4] = {
		{{-0.5f, -0.5f, 0.0f}, {0, 0, 1}, {0, 0}, {0, 0}},
		{{ 0.5f, -0.5f, 0.0f}, {0, 0, 1}, {1, 0}, {1, 0}},
		{{-0.5f,  0.5f, 0.0f}, {0, 0, 1}, {0, 1}, {0, 1}},
		{{ 0.5f,  0.5f, 0.0f}, {0, 0, 1}, {1, 1}, {1, 1}},
	};
	int planeVertexCount = 4;

	int planeInedxes[6] = {0, 2, 1, 1, 2, 3};
	unsigned int planeIndexCount = 6;
	int planeMaterialCount = 1;

	Mesh mesh((int*)planeInedxes, planeIndexCount, planePoints, planeVertexCount, &planeIndexCount, planeMaterialCount);

	const char *files[] = {
		"Models/cube.fbx",
		"Models/sphere.fbx",
	};
	for (int i = 0; i < sizeof(files) / sizeof(const char *); i++) {
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
		projection.setOrthoLH(0, (float)width * camera.focalLength, (float)height * camera.focalLength, 0, -4, 4);
	GraphicsEngine::setViewPort(0, 0, width, height);
	GraphicsEngine::clear(camera.color);
	GraphicsEngine::clearColorDepthBuffer();

	Span<RenderView> renderViews = ECS::GetComponents<RenderView>();
	for (auto& renderView : renderViews) {
		if (renderView.layout != renderViewIndex) continue;
		int objectID = renderView.object.getID();
		if (!ECS::isActive(objectID)) continue;

		// set shape
		unsigned int number_of_mats = MeshManager::setMeshById(renderView.mesh_index);

		// render object
		int number_of_materials = Math::Min(number_of_mats, (unsigned int)renderView.materals.size());
		int offset = 0;
		for (int j = 0; j < number_of_materials; j++) {
			Material material = MaterialManager::Get(renderView.materals[j]);
			int shader_index = material.shader_indexes;
			int texture_index = material.texture_index;
			int texture_index1 = material.texture_index1;
			int mesh_index = renderView.mesh_index;
			Shader *shader_ptr = shaders[shader_index];
			// set material
			GraphicsEngine::setShaderProgram(shader_ptr);
			GraphicsEngine::setTime(shader_ptr, Time::time);
			GraphicsEngine::setProjectionMatrix(shader_ptr, projection);
			GraphicsEngine::setCameraViewMatrix(shader_ptr, camView);

			GraphicsEngine::setTexture(TextureManager::GetTextureByID(texture_index), shader_ptr);
			GraphicsEngine::setVector4(shader_ptr, material.color.ToVector4());
			GraphicsEngine::setMatrix(shader_ptr, worlds[objectID]);

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
	for (auto& textView : textViews) {
		if (textView.layout != renderViewIndex) continue;
		int objectID = textView.object.getID();
		if (!ECS::isActive(objectID)) continue;

		// set shape
		int mesh_index = textView.getId();

		// set material
		unsigned int number_of_mats = MeshManager::setMeshById(mesh_index);
		GraphicsEngine::setMatrix(shaders[text_shader_index], worlds[objectID]);
		int number_of_triangles = MeshManager::getNumberOfPolygonsByMaterialID(mesh_index, 0);
		GraphicsEngine::drawTriangles(number_of_triangles, nullptr);
	}

	//ui
	// std::tie(width, height) = UILayout::getInstance().GetLayoutSize();
	
	
	constexpr float scaleUI = 0.01f;
	constexpr bool ui3d = true;
	int planeIndex = 0;
	int uiShaderIndex = 6;
	if(ui3d) {
		unsigned int number_of_mats = MeshManager::setMeshById(planeIndex);
		int planeTriangles = MeshManager::getNumberOfPolygonsByMaterialID(planeIndex, 0);
		Span<UIImage> uiImages = ECS::GetComponents<UIImage>();
		GraphicsEngine::setShaderProgram(shaders[uiShaderIndex]);
		GraphicsEngine::setProjectionMatrix(shaders[uiShaderIndex], projection);
		GraphicsEngine::setCameraViewMatrix(shaders[uiShaderIndex], camView);
		for (auto& uiImage : uiImages) {
			if (uiImage.layout != renderViewIndex) continue;
			int objectID = uiImage.object.getID();
			if (!ECS::isActive(objectID)) continue;

			GraphicsEngine::setTexture(TextureManager::GetTextureByID(uiImage.texture), shaders[uiShaderIndex]);
			GraphicsEngine::setVector4(shaders[uiShaderIndex], uiImage.color.ToVector4());

			Vector3 offset = uiImage.getOffset() * scaleUI;
			offset.z *= -1;
			offset.y += 6;
			worlds[objectID].setIdentity();
			worlds[objectID].setScale(uiImage.getComputedSize() * scaleUI);
			worlds[objectID].setTranslation(offset);

			GraphicsEngine::setMatrix(shaders[uiShaderIndex], worlds[objectID]);
			GraphicsEngine::drawTriangles(planeTriangles, nullptr);
		}

		Span<UIText> uiTextes = ECS::GetComponents<UIText>();
		GraphicsEngine::setShaderProgram(shaders[text_shader_index]);
		GraphicsEngine::setTexture(TextureManager::GetTextureByID(atlas_index), shaders[text_shader_index]);
		if(ui3d) {
			for (auto& uiText : uiTextes) {
				if (uiText.layout != renderViewIndex) continue;
				int objectID = uiText.object.getID();
				if (!ECS::isActive(objectID)) continue;

				Vector3 offset = uiText.getOffset() * scaleUI;
				offset.z *= -1;
				offset.y += 6;
				worlds[objectID].setIdentity();
				worlds[objectID].setScale(scaleUI);
				worlds[objectID].setTranslation(offset);

				GraphicsEngine::setVector4(shaders[text_shader_index], uiText.color.ToVector4());
				GraphicsEngine::setMatrix(shaders[text_shader_index], worlds[objectID]);
				
				int uiTextMesh = uiText.getId();
				unsigned int number_of_mats = MeshManager::setMeshById(uiTextMesh);
				int number_of_triangles = MeshManager::getNumberOfPolygonsByMaterialID(uiTextMesh, 0);
				GraphicsEngine::drawTriangles(number_of_triangles, nullptr);
			}
		}
	}

	

	Span<UIImage> uiImages = ECS::GetComponents<UIImage>();
	Vector2 uiSize = uiImages[0].getComputedSize();
	projection.setIdentity();
	projection.setOrthoLH(0.0f, uiSize.x, 0.0f, uiSize.y, -100.0f, 100.0f);
	camView.setIdentity();

	unsigned int number_of_mats = MeshManager::setMeshById(planeIndex);
	int planeTriangles = MeshManager::getNumberOfPolygonsByMaterialID(planeIndex, 0);
	GraphicsEngine::setShaderProgram(shaders[uiShaderIndex]);
	GraphicsEngine::setProjectionMatrix(shaders[uiShaderIndex], projection);
	GraphicsEngine::setCameraViewMatrix(shaders[uiShaderIndex], camView);
	for (auto& uiImage : uiImages) {
		if (uiImage.layout != renderViewIndex) continue;
		int objectID = uiImage.object.getID();
		if (!ECS::isActive(objectID)) continue;

		GraphicsEngine::setTexture(TextureManager::GetTextureByID(uiImage.texture), shaders[uiShaderIndex]);
		GraphicsEngine::setVector4(shaders[uiShaderIndex], uiImage.color.ToVector4());

		Matrix4x4 world;
		world.setIdentity();
		world.setScale(uiImage.getComputedSize());
		world.setTranslation(uiImage.getOffset());

		GraphicsEngine::setMatrix(shaders[uiShaderIndex], world);
		GraphicsEngine::drawTriangles(planeTriangles, nullptr);
	}

	//ui text
	Span<UIText> uiTextes = ECS::GetComponents<UIText>();
	GraphicsEngine::setShaderProgram(shaders[text_shader_index]);
	GraphicsEngine::setTexture(TextureManager::GetTextureByID(atlas_index), shaders[text_shader_index]);
	GraphicsEngine::setProjectionMatrix(shaders[text_shader_index], projection);
	GraphicsEngine::setCameraViewMatrix(shaders[text_shader_index], camView);
	for (auto& uiText : uiTextes) {
		if (uiText.layout != renderViewIndex) continue;
		int objectID = uiText.object.getID();
		if (!ECS::isActive(objectID)) continue;

		worlds[objectID].setIdentity();
		worlds[objectID].setTranslation(uiText.getOffset());

		GraphicsEngine::setVector4(shaders[text_shader_index], uiText.color.ToVector4());
		GraphicsEngine::setMatrix(shaders[text_shader_index], worlds[objectID]);
		
		int uiTextMesh = uiText.getId();
		unsigned int number_of_mats = MeshManager::setMeshById(uiTextMesh);
		int number_of_triangles = MeshManager::getNumberOfPolygonsByMaterialID(uiTextMesh, 0);
		GraphicsEngine::drawTriangles(number_of_triangles, nullptr);
	}
}

// #include <chrono>
// float delta = 0;
// int n = 0;
void RenderManager::Render() {
	ECS::UpdateActiveStates();
	// calcualte matrxies
	auto [hierarchy, hierarchySize] = ECS::GetHierarchy().rebuild();
	auto [parents, parentsSize] = ECS::GetHierarchy().getParents();
	for (int i = 0; i < hierarchySize; i++) {
		int objectID = hierarchy[i];
		int parent = parents[objectID];
		if (worlds.size() <= objectID) { worlds.resize(objectID + 1); }
		
		if (!ECS::isActive(objectID)) continue;

		Transform transform = ECS::GetComponent<Transform>(objectID);
		Matrix4x4 S, R, T;
		S.setIdentity();
		S.setScale(transform.scale);

		R.setIdentity();
		R.setRotation(transform.rotation);

		T.setIdentity();
		T.setTranslation(transform.position);

		Matrix4x4 local = T * R * S;
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