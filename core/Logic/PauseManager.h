#pragma once
#include "Transform.h"
#include "Physic.h"
#include "UIManager.h"
#include "Settings.h"
#include "IOSystem.h"

struct PauseElement: public Component {
    bool activityToPause = true;
};

class PauseManager {
public:
	static void Create();
    static bool IsPaused() { return isLocalPaused; }
    static void SetPaused(bool value) {
        isLocalPaused = value;
        Span<PauseElement> pauseElements = ECS::GetComponents<PauseElement>();
        for(auto& element: pauseElements) {
            if(!element.activityToPause) element.object.GetComponent<Active>().enabled = false;
            else element.object.GetComponent<Active>().enabled = value;
        }
	}
    static void Toggle() { SetPaused(!isLocalPaused); }
	static void Update() {
        IOSystem::getPlatform().showCursor(isLocalPaused);
        IOSystem::lockMouse(!isLocalPaused);
	}
private:
	static bool isLocalPaused;
};