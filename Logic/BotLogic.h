#pragma once
#include "Transform.h"
#include "NavMeshSystem.h"

class BotLogic: public Component {
public:
	void Update() {
		NavMeshAgent& navMeshAgent = object.GetComponent<NavMeshAgent>();
		if(navMeshAgent.pathStatus == PathComplete) {
			navMeshAgent.SetDestination(NavMeshSystem::GetRandomPoint());
		}
		object.transform.rotation *= Quaternion::FromEuler(Time::fixedDeltaTime, 0, 0);
	};
};