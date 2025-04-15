#pragma once
#include "Transform.h"

#define BATHROOM 1

class LampComponent: public Component {
public:
	void turn(bool on) {
		if(on) {
			light.GetComponent<RenderView>().enabled = true;
		}
		else {
			light.GetComponent<RenderView>().enabled = false;
		}
	}
public:
	bool switcher;
	Object light;
	int roomId = 0;
};

void callLamps() {
	std::pair<LampComponent*, int> s = ECS::GetComponents<LampComponent>();
	for(int i = 0; i < s.second; i++) {
		if(s.first[i].roomId == BATHROOM) s.first[i].turn(true);
	}
}