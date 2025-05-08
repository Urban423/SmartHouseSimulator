#include "RenderManager.h"
#include "IOSystem.h"
#include "ECS.h"
#include <cstdio>

RenderManager* RenderManager::renderManager = new RenderManager();

void RenderManager::calculateCameraView(const Transform& camera, Camera& cameraData) {
	Matrix4x4 temp;
	//temp.setRotation(player.transform->rotation);
	
	Matrix4x4 cam;
	cam.setIdentity();
	cam.setRotation(camera.rotation);
	//printf("%f %f %f %f\n", player.camera->rotation.x, player.camera->rotation.y, player.camera->rotation.z, player.camera->rotation.w);
	//cam *= temp;
	
	temp.setIdentity();
	temp.setTranslation(camera.position);
	//printf("%f %f %f\n", camera.position.x,  camera.position.y,  camera.position.z);
	cam *= temp;
	
	cameraData.camView = cam;
	cameraData.camView.inverse();
}


void RenderManager::init() {
	//create texture
	const char* textureFiles[] = {
		"floorWooden1.bmp",
		"Tile.bmp",
		"Tile1.bmp",
		"Floortex.bmp",
		"Grass.bmp",
		"Ghost1.bmp",
	};
	for(int i = 0; i < sizeof(textureFiles) / sizeof(const char*); i++) {
		TextureStruct assa  = IOSystem::readBMP(textureFiles[i]);
		textures.push_back(GraphicsEngine::createTexture(assa));
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


void RenderManager::renderCamera(Camera& camera) {
	calculateCameraView(camera.object.transform, camera);
	
	
	GraphicsEngine::setViewPort(camera.left, camera.top, camera.right, camera.bottom);
	// GraphicsEngine::clearDepthBuffer();
	// GraphicsEngine::setShaderProgram(shaders[1]);
	// GraphicsEngine::setVertexArrayObject(vertexes[2]);
	// GraphicsEngine::setIndexArrayObject(indicies[2]);
	// GraphicsEngine::drawTriangles(indicies[2]->getNumberOfMaterials(), 0);
	//GraphicsEngine::clearDepthBuffer();
	GraphicsEngine::clearColorDepthBuffer();
	
	std::pair<RenderView*, int> s = ECS::GetComponents<RenderView>(camera.RenderViewDataIndex);
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
			GraphicsEngine::setProjectionMatrix(shaders[renderView[i].shader_indexes[j]], camera.projection);
			GraphicsEngine::setCameraViewMatrix(shaders[renderView[i].shader_indexes[j]], camera.camView);
			
			GraphicsEngine::setTexture(textures[renderView[i].texture_indexes[j]], shaders[renderView[i].shader_indexes[j]]);
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
	auto[cameras, size] = ECS::GetComponents<Camera>(0);
	for(int i = 0; i < size; i++) {
		renderCamera(cameras[i]);
	}
}