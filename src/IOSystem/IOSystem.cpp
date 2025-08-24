#include "IOSystem.h"

_Input IOSystem::input;
_Window IOSystem::window;


//main
void IOSystem::onCreate(const char* windowName, int width, int height, bool fullscreen)
{
    window.init(windowName, width, height, fullscreen);
    input.create();
}

void IOSystem::onUpdate()
{
    window.handleInput();
    input.update();
}



//time
void IOSystem::initTime() {	}
double IOSystem::getDeltaTime(){return 0;}



//window
void IOSystem::setCenterCursorPos()
{
	Rect rect = window.getCenter();
	input.setCursorPos(rect.width(), rect.height());
}
