#include "ClientServerSystem.h"
#include "NetworkManager.h"

void Start() {
    
}

void FixedUpdate() {
    //recieve
    switch(NetworkManager::getInstance().networkMode) {
        case(NetworkMode::Host): {
            NetworkManager::getInstance().server.getInputs(MessageType::Snapshot, &IOSystem::getInput(), sizeof(IOSystem::getInput()));
            break;
        }

        case(NetworkMode::Client): {
            NetworkManager::getInstance().client.getSnapshots(MessageType::Input, &IOSystem::getInput(), sizeof(IOSystem::getInput()));
            break;
        }

        default: break;
    }

    //send
    switch(NetworkManager::getInstance().networkMode) {
        case(NetworkMode::Host): {
            NetworkManager::getInstance().server.sendMessage(MessageType::Snapshot, &IOSystem::getInput(), sizeof(IOSystem::getInput()));
            break;
        }

        case(NetworkMode::Client): {
            NetworkManager::getInstance().client.sendMessage(MessageType::Input, &IOSystem::getInput(), sizeof(IOSystem::getInput()));
            break;
        }

        default: break;
    }
}