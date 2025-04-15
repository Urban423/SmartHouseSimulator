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
	textures.resize(1);
	textures[0] = GraphicsEngine::createTexture(IOSystem::readBMP("ghost2.bmp"));

	//create shader
	const char* shadersFiles[] = {
		"shader.vsh",
		"shader.fsh",
		
		"shader1.vsh",
		"shader1.fsh",
		
		"shader2.vsh",
		"shader2.fsh",
		
		"shader3.vsh",
		"shader3.fsh",
	};
	shaders.resize(4);
	for(int i = 0; i < 8; i+=2) {
		shaders[i / 2] = GraphicsEngine::createShaderProgram({openCFile(shadersFiles[i]).getPtr(), openCFile(shadersFiles[i + 1]).getPtr()});
	}
	
	//create shape points
	vertexes.resize(3);
	indicies.resize(3);
	const char* files[] = {
		"sphere.fbx",
		"testMesh.fbx",
		"Plane.fbx"
	};
	for(int i = 0; i < 3; i++) {
		Mesh mesh = IOSystem::readFBX(files[i]);
		indicies[i] = GraphicsEngine::createIndexArrayObject({ (unsigned int*)mesh.index,  (unsigned int)mesh.index_size });
		vertexes[i] = GraphicsEngine::createVertexArrayObject({ mesh.vertex, sizeof(Vertex), (unsigned int)mesh.vertex_size });
		//freeMesh(mesh);
	}
	
	//Mesh plane = PerlinNoiseMesh(100, 100, 6.0f, 0, 0.9f, 13);
	//vertexes_indexes2 = GraphicsEngine::createIndexArrayObject({ (unsigned int*)plane.index,  (unsigned int)plane.index_size });
	//vertexes2 = GraphicsEngine::createVertexArrayObject({ plane.vertex, sizeof(Vertex), (unsigned int)plane.vertex_size });
}


void RenderManager::Render() { 
	GraphicsEngine::disable3D();
	std::pair<Camera*, int> s1 = ECS::GetComponents<Camera>(0);
	Camera* camera = s1.first;
	for(int cameraIndex = 0; cameraIndex < s1.second; cameraIndex++) {
		Camera& camData = camera[cameraIndex];
		calculateCameraView(camData.object.transform, camData);
		GraphicsEngine::setProjectionMatrix(shaders[0], camData.projection);
		GraphicsEngine::setCameraViewMatrix(shaders[0], camData.camView);
		
		//printf("%d %d %d %d\n", camData.left, camData.top, camData.right, camData.bottom);
		GraphicsEngine::setViewPort(camData.left, camData.top, camData.right, camData.bottom);
		// GraphicsEngine::clearDepthBuffer();
		// GraphicsEngine::setShaderProgram(shaders[1]);
		// GraphicsEngine::setVertexArrayObject(vertexes[2]);
		// GraphicsEngine::setIndexArrayObject(indicies[2]);
		// GraphicsEngine::drawTriangles(indicies[2]->getNumberOfMaterials(), 0);
		//GraphicsEngine::clearDepthBuffer();
		GraphicsEngine::clearColorDepthBuffer();
		
		std::pair<RenderView*, int> s = ECS::GetComponents<RenderView>(camData.RenderViewDataIndex);
		RenderView* renderView = s.first;
		for(int i = s.second - 1; i > -1; i--) {
			if(renderView[i].enabled == false) { continue; }
			
			//set material
			GraphicsEngine::setShaderProgram(shaders[renderView[i].shader_index]);
			GraphicsEngine::setTexture(textures[renderView[i].texture_index], shaders[renderView[i].shader_index]);
			GraphicsEngine::setVector4( shaders[renderView[i].shader_index], Vector4(renderView[i].color.r, renderView[i].color.g, renderView[i].color.b, 1));
			
			//set shape
			GraphicsEngine::setVertexArrayObject(vertexes[renderView[i].mesh_index]);
			GraphicsEngine::setIndexArrayObject(indicies[renderView[i].mesh_index]);
			
			Transform& transform = renderView[i].object.transform;
			
			Matrix4x4 world, temp;
			world.setIdentity();
			world.setScale(transform.scale);
			
			temp.setIdentity();
			temp.setTranslation(transform.position);
			world *= temp;
			
			GraphicsEngine::setMatrix(shaders[renderView[i].shader_index], world);
			GraphicsEngine::drawTriangles(indicies[renderView[i].mesh_index]->getNumberOfMaterials(), 0);
		}
	}
}