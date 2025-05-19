#include "Server.h"

Object Server::addLamp() {
    Object lamp = ECS::createObject();
    lamp.transform.scale = Vector2(0.03, 0.03);
    lamp.AddComponent<RenderView>(1).texture_indexes[0] = 8;
    lamp.AddComponent<LampComponent>();
    return lamp;
};

Object Server::addThermometr() {
    Object thermometr = ECS::createObject();
    thermometr.transform.scale = Vector2(0.03, 0.03);
    thermometr.AddComponent<RenderView>(1).texture_indexes[0] = 7;
    return thermometr;
};

Object Server::addClock() {
    Object clock = ECS::createObject();
    clock.transform.scale = Vector2(0.03, 0.03);
    clock.AddComponent<RenderView>(1).texture_indexes[0] = 6;
    return clock;
};

Object Server::addMotionSensor() {
    Object motionSenser = ECS::createObject();
    motionSenser.transform.scale = Vector2(0.03, 0.03);
    motionSenser.AddComponent<RenderView>(1).texture_indexes[0] = 9;
    motionSenser.AddComponent<MotionSensor>();
    return motionSenser;
};

float Server::getTemperature() {
    return 36.6;
}

float Server::getTime() {
    return 36.6;
}