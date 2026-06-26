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
		if(NetworkManager::getInstance().networkMode == NetworkMode::Host) NetworkManager::getInstance().server.receive();
		else if(NetworkManager::getInstance().networkMode == NetworkMode::Client) NetworkManager::getInstance().client.receive();
		server.update();
		client.update();
		if(NetworkManager::getInstance().networkMode == NetworkMode::Host) NetworkManager::getInstance().server.send();
		else if(NetworkManager::getInstance().networkMode == NetworkMode::Client) NetworkManager::getInstance().client.send();
	}
}