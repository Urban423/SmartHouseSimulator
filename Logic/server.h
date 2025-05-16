#pragma once
#include "Transform.h"
#include "ECS.h"

class Server : public Component {
public:

    void addLamp();

    void addThermometr();

    void addClock();

    void addMotionSensor();

    float getTemperature();

    float getTime();

    bool isMovement();
    
};