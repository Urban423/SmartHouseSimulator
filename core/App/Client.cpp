#include "Client.h"
#include "GraphicsEngine.h"
#include "Window.h"
#include "RenderManager.h"
#include "WindowsPlatform.h"
#include <cstdio>


void Client::create() {
	IOSystem::addWindow(new Window).create("Sanya lol", 1000, 500, false, true);
	IOSystem::addkeyBorad(new keyBoard).create();
	IOSystem::addPlatform(new WindowsPlatform).create();
	
	GraphicsEngine::init();
	GraphicsEngine::clear(Color(0, 0, 0, 1));
	GraphicsEngine::setCullMode(BackFace);
	RenderManager::onCreate();
}


void Client::update() {
    IOSystem::getInstance().update();
    Scene::Update();
    RenderManager::onUpdate();
    IOSystem::getWindow().swapBuffers();
    // IOSystem::getWindow().update();
}