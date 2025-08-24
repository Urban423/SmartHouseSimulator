#pragma once
#include "Transform.h"

#define BATHROOM 1

class LampComponent: public Component {
public:
	void turn(bool on) {
		if(on) {
			object.GetComponent<RenderView>().texture_indexes[0] = 7;
			object.GetComponent<RenderView>().shader_indexes[0] = 4;
		}
	 else {
		object.GetComponent<RenderView>().texture_indexes[0] = 8;
		object.GetComponent<RenderView>().shader_indexes[0] = 0;
	 }

	}
public:
	bool switcher;
	Object light;
	int roomId = 0;
};

inline void callLamps() {
	std::pair<LampComponent*, int> s = ECS::GetComponents<LampComponent>();
	for(int i = 0; i < s.second; i++) {
		if(s.first[i].roomId == BATHROOM) s.first[i].turn(true);
	}
}

class Grab: public Component {
public:
	void grab() {
		object.transform.position.x += 0.1f;
	}
};