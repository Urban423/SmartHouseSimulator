#pragma once
#include "Transform.h"
#include "ECS.h"
#include "Lamp.h"
#include "MotionSensor.h"

class Server : public Component {
public:

    Object addLamp();

    Object addThermometr();

    Object addClock();

    Object addMotionSensor();

    float getTemperature();

    float getTime();

    bool isMovement();
    
};