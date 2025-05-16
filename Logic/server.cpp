#include "Server.h"

void Server::addLamp() {
    Object lamp = ECS::createObject();
    lamp.transform.scale = Vector2(0.03, 0.03);
    lamp.AddComponent<RenderView>(1).texture_indexes[0] = 6;
};

void Server::addThermometr() {
    Object thermometr = ECS::createObject();
    thermometr.transform.scale = Vector2(0.03, 0.03);
    thermometr.AddComponent<RenderView>(1).texture_indexes[0] = 7;
};

void Server::addClock() {
    Object clock = ECS::createObject();
    clock.transform.scale = Vector2(0.03, 0.03);
    clock.AddComponent<RenderView>(1).texture_indexes[0] = 6;
};

void Server::addMotionSensor() {
    Object motionSenser = ECS::createObject();
    motionSenser.transform.scale = Vector2(0.03, 0.03);
    motionSenser.AddComponent<RenderView>(1).texture_indexes[0] = 6;
};

float Server::getTemperature() {
    return 36.6;
}

float Server::getTime() {
    return 36.6;
}