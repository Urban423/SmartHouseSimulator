#include "NavMeshSystem.h"
#include "Random.h"
#include "ECS.h"

NavMeshSystem* NavMeshSystem::navMeshSystem = new NavMeshSystem();

void NavMeshSystem::Start() {
	Span<NavMeshObstacle> navMeshObstacles = ECS::GetComponents<NavMeshObstacle>();
	base = Shapes::Polygon(Rect(-1.0f, -1.0f, 1.0f, 1.0f));
}

void NavMeshSystem::Update() {
	Span<NavMeshAgent> navMeshAgents = ECS::GetComponents<NavMeshAgent>();
	for(int i = 0; i < navMeshAgents.size(); i++) {
		if(navMeshAgents[i].pathStatus == PathComplete) { continue; }
		if(navMeshAgents[i].pathStatus == PathInComplete) {
			//calculate path
			navMeshAgents[i].pathStatus = PathPartial;
			continue;
		}
		
		float deltaPath = navMeshAgents[i].speed * Time::fixedDeltaTime;
		if(deltaPath * deltaPath >= Vector3::SqrDistance(navMeshAgents[i].object.transform.position, navMeshAgents[i].aimTarget)) { 
			navMeshAgents[i].pathStatus = PathComplete;
			navMeshAgents[i].object.transform.position = navMeshAgents[i].aimTarget;
			continue;
		}
		
		navMeshAgents[i].object.transform.position += (navMeshAgents[i].aimTarget - navMeshAgents[i].object.transform.position).normalized() * deltaPath;
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