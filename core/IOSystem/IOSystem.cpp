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
    platform->getSupportedResolutions(supportedResolutions);
    platform->init();
    supportedResolutions.erase(
        std::remove_if(
            supportedResolutions.begin(),
            supportedResolutions.end(),
            [](const auto& r) {
                int width = r.first;
                int height = r.second;
                if(width < 1280 || height < 720) return false;
                float aspect = (float)width / height;
                if(std::abs(aspect - 16.0f / 9.0f) > 0.15f) return true;
                return false;
            }
        ),
        supportedResolutions.end()
    );
    std::sort(supportedResolutions.begin(), supportedResolutions.end(),
        [](const std::pair<int,int>& a, const std::pair<int,int>& b) {
        int areaA = a.first * a.second;
        int areaB = b.first * b.second;
        return areaA > areaB;
    });
}

void IOSystem::update() {
    platform->update();
    Vector2 oldPos = input.pointerPosition;

    input = Input();
    input.text.reserve(256);
    input.keyEvents.reserve(256);

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