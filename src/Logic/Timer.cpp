#include "Timer.h"


float Time::deltaTime 	= 0;
float Time::timeScale 	= 1;
float Time::time 		= 0;

void Time::UpdateTimeSystem() {
	deltaTime = timeScale * 0.033;
	time += deltaTime;
}