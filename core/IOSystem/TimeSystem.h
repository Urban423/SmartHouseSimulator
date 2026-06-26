#pragma once
#include "IOSystem.h"

class Time {
public:
    //game time
    static double timeScale;
    static double deltaTime;
    static double time;
    //real time
    static double systemStartTime;
    static double realDeltaTime;
    static double realTime;
    //fixed time
    static int tick;
    static double fixedDeltaTime;
    static double fixedTime;
    static constexpr double fixedStep = 1.0 / 60.0;

    inline static void start() {
        systemStartTime = IOSystem::getPlatform().getTime(); 
        realTime = 0;
        time = 0;
    }

    inline static void update() {
        double newSystemTime = IOSystem::getPlatform().getTime();
        double newRealTime = newSystemTime - systemStartTime;
        double newTime = newRealTime * timeScale;
        deltaTime = newTime - time;
        realDeltaTime = newRealTime - realTime;
        time += deltaTime;
        realTime = newRealTime;
    }

    inline static void fixedUpdate() {
        fixedDeltaTime = fixedStep * timeScale;
        fixedTime += fixedDeltaTime;
        ++tick;
    }
};