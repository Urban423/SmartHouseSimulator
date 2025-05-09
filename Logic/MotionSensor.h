#pragma once

#include "Transform.h"
#include "ECS.h"
#include <vector>
#include <cmath>

class MotionSenser : public Component {
public:
    float rotation_angle = 0.261799f; // 15 градусов в радианах
    float rudius = 10.0f;
    std::vector<Vector3> old_posituons;
    bool trigger = false;

    void start();
    void update();
};

