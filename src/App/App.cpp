#include "App.h"
#include "Color.h"
#include "Rect.h"
#include "InputEventSystem.h"
#include <cstdio>

void App::GraphicInit()
{
	GraphicsEngine::init();
	GraphicsEngine::clear(Color(0, 0, 0, 1));
	GraphicsEngine::setCullMode(BackFace);
	//Rect size = IOSystem::getInnerSize();
	//setSize(size.width(), size.height());
}

void App::onCreate() {
	//create input/output system
	IOSystem::onCreate("Sanya lol", 1000 , 500, false);
	
	//create graphics egine
	GraphicInit();
	Rect winSize = IOSystem::getOutputInstance().getInnerSize();
	int width  = winSize.right - winSize.left;
	int height = winSize.bottom - winSize.top;
	scenes.resize(1);
	scenes[0].Start();
	RenderManager::onCreate();
	IOSystem::getOutputInstance().setIES(ECS::GetComponents<InputEventSystem>().first);
	IOSystem::getOutputInstance().setApp(this);
	IOSystem::getOutputInstance().setSize();
}

void  App::Move() { }

void App::onUpdate() {
	while(IOSystem::getOutputInstance().isRunning()) {
		if(!mode) {
			for(int i = 0; i < scenes.size(); i++) {
				scenes[i].Update();
			}
			RenderManager::onUpdate();
			IOSystem::getOutputInstance().broadcast();
		}
		IOSystem::getOutputInstance().handleInput();
		IOSystem::getInputInstance().update();
	}
}

void App::onUpdateIO() {
	IOSystem::getOutputInstance().setRenderContext();
	while(IOSystem::getOutputInstance().isRunning() && mode) {
		for(int i = 0; i < scenes.size(); i++) {
			scenes[i].Update();
		}
		RenderManager::onUpdate();
		IOSystem::getOutputInstance().broadcast();
	}
	IOSystem::getOutputInstance().setRenderContextNULL();
}


void App::enterResizingMode() {
	mode = true;
	IOSystem::getOutputInstance().setRenderContextNULL();
	resizeThread = std::thread(&App::onUpdateIO, this);
}


void App::exitResizingMode() {
	mode = false;
	 if (resizeThread.joinable()) {
		resizeThread.join();
	}
	IOSystem::getOutputInstance().setRenderContext();
}

