#pragma once
#include "Polygon.h"
#include "Transform.h"
//#include "Graph.h"


#define PathInComplete 	0
#define PathComplete 	1
#define PathPartial  	2

class NavMeshSystem {
public:
	static Vector3 GetRandomPoint();
	inline static NavMeshSystem* getPtr() { return navMeshSystem; } 
	void Start();
	void Update();
private:
	static NavMeshSystem* navMeshSystem;
	Shapes::Polygon base;
	//MatrixGraph<float> graph;
};


class NavMeshObstacle: public Component {
	Vector3 center;
	Vector3 size;
};

class NavMeshAgent: public Component {
public:
	float speed = 1.0f;
	char pathStatus = PathComplete;
	
	Vector3 aimTarget;
	void SetDestination(Vector3 point);
};