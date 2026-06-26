#include "TimeSystem.h"

double Time::timeScale = 1.0f;
double Time::deltaTime = 0.0f;
double Time::time = 0.0f;

double Time::systemStartTime = 0.0f;
double Time::realDeltaTime = 0.0f;
double Time::realTime = 0.0f;

double Time::fixedDeltaTime = 0.0f;
double Time::fixedTime = 0.0f;
int Time::tick = 0;