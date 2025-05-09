#pragma once
#include <string>

class Server {
public:

    void addMotionSensor(std::string name);

    void addLamp(std::string name);

    void addWatch(std::string name);

    void addThermometer(std::string name);

    void deleteMotionSensor(std::string name);

    void deleteLamp(std::string name);

    void deleteWatch(std::string name);

    void deleteThermometer(std::string name);

    void addScenario(std::string scenario);

    void deleteScenario(std::string scenario);
    
};