#include "IOSystem.h"

// #ifdef Android
// 	#include "AndroidInputManager.h"
// 	#include "AndroidFilter.h"
// 	#include "AndroidFileManager.h"
// #elif windowsOS
// #endif

// void IOSystem::windowLoop() {
//     while(running) {
//         windows[0]->update();
//         WindowSnapshot s;
//         auto size = getSize();
//         s.width  = size.first;
//         s.height = size.second;
//         s.running = windows[0]->isRunning();
//         setWindowState(s);
//     }
// }

void IOSystem::create() {
    input.text.reserve(256);
    input.keyEvents.reserve(256);
    input.movement = Vector2(0, 0);
    input.pointerPosition = Vector2(0, 0);
    input.pointerDelta = Vector2(0, 0);
    input.scroll = 0.0f;
    input.pointerPressed = false;
    input.pointerHold = false;
    input.pointerReleased = false;
    input.jumpPressed = false;
    input.sprint = false;
    input.pausePressed = false;
    input.sitPressed = false;
    input.actionPressed = false;
}

void IOSystem::update() {
    platform->update();
    Vector2 oldPos = input.pointerPosition;
    create();
    platform->getText(input.text);
    platform->getKeyEvents(input.keyEvents);
    if(!windows[0]->focus()) return;
    for(auto keyboard : keyboards) {
        keyboard->update();
        keyboard->updateInput(input);
    }

    input.pointerPosition = platform->getCursorPosition();
    input.pointerDelta = input.pointerPosition - oldPos;
    if(lockPointers) {
        auto rect = getWindow().getInnerSize();
        int centerX = (int)rect.left + (int)(rect.right - rect.left) / 2;
        int centerY = (int)rect.top + (int)(rect.bottom - rect.top) / 2;
        platform->setCursorPosition(centerX, centerY);
        input.pointerPosition = Vector2(centerX, centerY);
    }
}