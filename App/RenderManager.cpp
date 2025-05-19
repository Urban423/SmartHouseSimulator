#include "RenderManager.h"
#include "IOSystem.h"
#include "TextureManager.h"
#include "ECS.h"
#include <cstdio>

RenderManager* RenderManager::renderManager = new RenderManager();

Matrix4x4 RenderManager::calculateCameraView(const Transform& camera) {
	Matrix4x4 temp;
	//temp.setRotation(player.transform->rotation);
	
	Matrix4x4 cam;
	cam.setIdentity();
	cam.setRotation(camera.rotation);
	//cam *= temp;
	
	temp.setIdentity();
	temp.setTranslation(camera.position);
	cam *= temp;
	cam.inverse();
	return cam;
}


void RenderManager::init() {
	//create texture
	const char* textureFiles[] = {
		"floorWooden1.bmp",
		"Tile.bmp",
		"Tile1.bmp",
		"Floortex.bmp",
		"Grass.bmp",
		"Ghost.bmp",
		"Ghost1.bmp",
		"LightBulb.bmp",
		"LightBulbOff.bmp",
		"MotionSensor.bmp"
	};
	for(int i = 0; i < sizeof(textureFiles) / sizeof(const char*); i++) {
		TextureStruct assa  = IOSystem::readBMP(textureFiles[i]);
		TextureManager::CreateTexture(assa);
	}

	//create shader
	const char* shadersFiles[] = {
		"shader.vsh",
		"shader.fsh",
		
		"shader2.vsh",
		"shader2.fsh",
		
		"shader3.vsh",
		"shader3.fsh",
		
		"shader4.vsh",
		"shader4.fsh",
	};
	for(int i = 0; i < sizeof(shadersFiles) / sizeof(const char*); i+=2) {
		shaders.push_back( GraphicsEngine::createShaderProgram({openCFile(shadersFiles[i]).getPtr(), openCFile(shadersFiles[i + 1]).getPtr()}) );
	}
	
	//create shape points
	const char* files[] = {
		"Plane.fbx",
		"HouseMap.fbx",
	};
	for(int i = 0; i < sizeof(files) / sizeof(const char*); i++) {
		std::vector<Mesh> meshes = IOSystem::readFBX(files[i]);
		for(int j = 0; j < meshes.size(); j++) {
			indicies.push_back(GraphicsEngine::createIndexArrayObject({ (unsigned int*)meshes[j].index,  (unsigned int)meshes[j].index_size, meshes[j].number_of_materials, meshes[j].materials }));
			vertexes.push_back(GraphicsEngine::createVertexArrayObject({ meshes[j].vertex, sizeof(Vertex), (unsigned int)meshes[j].vertex_size }));
			//freeMesh(mesh);
		}
	}
	
	//Mesh plane = PerlinNoiseMesh(100, 100, 6.0f, 0, 0.9f, 13);
	//vertexes_indexes2 = GraphicsEngine::createIndexArrayObject({ (unsigned int*)plane.index,  (unsigned int)plane.index_size });
	//vertexes2 = GraphicsEngine::createVertexArrayObject({ plane.vertex, sizeof(Vertex), (unsigned int)plane.vertex_size });
}


void RenderManager::renderCamera(Camera& camera, int renderViewIndex) {
	TextureManager::SetRenderTarget(camera.frameBufferIndex);
	
	Matrix4x4 camView = calculateCameraView(camera.object.transform);
	Matrix4x4 projection;
	projection.setIdentity();
	int width, height;
	if(camera.frameBufferIndex == -1) { 
		std::tie(width, height) = IOSystem::getOutputInstance().getSize();
	} 
	else {
		 std::tie(width, height) = TextureManager::GetTextureSize(camera.frameBufferIndex);
	}
	projection.setOrthoLH((float)width * camera.focalLength, (float)height * camera.focalLength, -4, 4);
	GraphicsEngine::setViewPort(0, 0, width, height);
	GraphicsEngine::clear(camera.color);
	GraphicsEngine::clearColorDepthBuffer();
	
	std::pair<RenderView*, int> s = ECS::GetComponents<RenderView>(renderViewIndex);
	RenderView* renderView = s.first;
	for(int i = s.second - 1; i > -1; i--) {
		if(renderView[i].enabled == false) { continue; }
		
		//set shape
		GraphicsEngine::setVertexArrayObject(vertexes[renderView[i].mesh_index]);
		GraphicsEngine::setIndexArrayObject(indicies[renderView[i].mesh_index]);
		
		//calculate transform
		Transform& transform = renderView[i].object.transform;
		Matrix4x4 world, temp;
		world.setIdentity();
		world.setScale(transform.scale);
		
		temp.setIdentity();
		temp.setTranslation(transform.position);
		world *= temp;
		
		//render object
		int number_of_materials = std::min(indicies[renderView[i].mesh_index]->getNumberOfMaterials(), (unsigned int)renderView[i].shader_indexes.size());
		number_of_materials = std::min((int)renderView[i].texture_indexes.size(), number_of_materials);
		int offset = 0;
		for(int j = 0; j < number_of_materials; j++) {
			//set material
			GraphicsEngine::setShaderProgram(shaders[renderView[i].shader_indexes[j]]);
			GraphicsEngine::setProjectionMatrix(shaders[renderView[i].shader_indexes[j]], projection);
			GraphicsEngine::setCameraViewMatrix(shaders[renderView[i].shader_indexes[j]], camView);
			
			GraphicsEngine::setTexture(TextureManager::GetTextureByID(renderView[i].texture_indexes[j]), shaders[renderView[i].shader_indexes[j]]);
			GraphicsEngine::setVector4( shaders[renderView[i].shader_indexes[j]], Vector4(renderView[i].color.r, renderView[i].color.g, renderView[i].color.b, 1));
			GraphicsEngine::setMatrix(shaders[renderView[i].shader_indexes[j]], world);
			
			int number_of_triangles = indicies[renderView[i].mesh_index]->getMaterialSize(j);
			GraphicsEngine::drawTriangles(number_of_triangles, (void*)(offset * sizeof(int)));
			offset += number_of_triangles;
		}
	}
}

void RenderManager::Render() { 
	GraphicsEngine::disable3D();
	int groupCounter = ECS::GetComponentGroupSize<Camera>();
	for(int i = groupCounter - 1; i >= 0; i--) {
		auto[cameras, size] = ECS::GetComponents<Camera>(i);
		for(int j = 0; j < size; j++) {
			renderCamera(cameras[j], i);
		}
	}
}