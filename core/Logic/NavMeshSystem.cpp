#include "NavMeshSystem.h"
#include "Random.h"
#include "ECS.h"

NavMeshSystem* NavMeshSystem::navMeshSystem = new NavMeshSystem();

void NavMeshSystem::Start() {
	auto[navMeshObstacle, size] = ECS::GetComponents<NavMeshObstacle>();
	base = Shapes::Polygon(Rect(-1.0f, -1.0f, 1.0f, 1.0f));
}

void NavMeshSystem::Update() {
	auto[navMeshAgent, size] = ECS::GetComponents<NavMeshAgent>();
	for(int i = 0; i < size; i++) {
		if(navMeshAgent[i].pathStatus == PathComplete) { continue; }
		if(navMeshAgent[i].pathStatus == PathInComplete) {
			//calculate path
			navMeshAgent[i].pathStatus = PathPartial;
			continue;
		}
		
		float deltaPath = navMeshAgent[i].speed * Time::fixedDeltaTime;
		if(deltaPath * deltaPath >= Vector3::SqrDistance(navMeshAgent[i].object.transform.position, navMeshAgent[i].aimTarget)) { 
			navMeshAgent[i].pathStatus = PathComplete;
			navMeshAgent[i].object.transform.position = navMeshAgent[i].aimTarget;
			continue;
		}
		
		navMeshAgent[i].object.transform.position += (navMeshAgent[i].aimTarget - navMeshAgent[i].object.transform.position).normalized() * deltaPath;
	}
}

void NavMeshAgent::SetDestination(Vector3 point) {
	pathStatus = PathInComplete;
	aimTarget = point;
}

Vector3 NavMeshSystem::GetRandomPoint() {
	Vector3 a = getPtr()->base[rand() % getPtr()->base.size()];
	Vector3 b = getPtr()->base[rand() % getPtr()->base.size()];
	float x = noise(0.0f, 1.0f);
	return a * x + b * (1 - x);
}