#pragma once

#include "Transform.h"
#include <chrono>
#include <string>

class Clock : public Component {
public:
    std::chrono::time_point<std::chrono::steady_clock> startTime;

    std::string formattedTime;

    float updateInterval = 1.0f;
    float timeSinceLastUpdate = 0.0f;

    void start();

    void update(float deltaTime);

    std::string getFormattedTime() const;
};