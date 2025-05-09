#pragma once

#include "Transform.h"

class Thermometer : public Component {
public:
    float currentTemperature = 0.0f;

    float minTemperature = -50.0f;
    float maxTemperature = 100.0f;

    bool alert = false;

    void start();

    void update(float deltaTime);

    void setTemperature(float temperature);

    bool isTemperatureOutOfRange() const;
};
