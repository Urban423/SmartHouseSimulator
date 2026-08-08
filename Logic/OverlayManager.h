#pragma once
#include "Transform.h"
#include "Physic.h"
#include "UIManager.h"
#include "Settings.h"
#include "IOSystem.h"

enum class SystemState {
    Menu,
    Playing,
    Paused
};

struct OverlayElement: public Component {
    bool useState = true;
    SystemState state = SystemState::Menu;
};

class OverlayManager {
public:
	static void Create();
    static bool IsPaused() { return state != SystemState::Playing; }
    static void SetState(SystemState newState) {
        state = newState;
        Span<OverlayElement> overlayElements = ECS::GetComponents<OverlayElement>();
        for(auto& element: overlayElements) {
            Active& active = element.object.GetComponent<Active>();
            
            if(!element.useState) active.enabled = false;
            else active.enabled = element.state == newState;
        }
	}
    static void Toggle() {
        if(state == SystemState::Menu) return;
        if(state == SystemState::Paused) {
            SetState(SystemState::Playing); 
        }
        else {
            SetState(SystemState::Paused); 
        }
    }
	static void Update() {
        bool isLocalPaused = IsPaused();
        IOSystem::getPlatform().showCursor(isLocalPaused);
        IOSystem::lockMouse(!isLocalPaused);
	}
private:
	static SystemState state;
};