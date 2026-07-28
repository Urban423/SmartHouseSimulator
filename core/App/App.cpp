#include "App.h"
#include "IOSystem.h"
#include "NetworkManager.h"

void App::onCreate() {
	client.create();
	server.create();
}


void App::onUpdate() {
	while(IOSystem::getWindow().isRunning()) 
	{
		NetworkManager::getInstance().receive();
		server.update();
		client.update();
		NetworkManager::getInstance().update();
		NetworkManager::getInstance().send();
		client.render();
	}
}