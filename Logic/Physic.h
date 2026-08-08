#pragma once
#include "ECS.h"
#include "umath.h"
#include "Colliders.h"


enum AxisLock : char {
    None = 0,
    X = 1 << 0,
    Y = 1 << 1,
    Z = 1 << 2
};

struct Rigidbody: public Component {
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 force;
    Vector3 angularVelocity;
    Vector3 torque;
	Vector3 prevPosition;
    float mass = 1.0f;
    bool isKinematic = false;
	char linearLock = 0;
    char angularLock = 0;

	inline void AddImpulse(const Vector3 impulse) { velocity += mass * impulse; }
	inline void AddForce(const Vector3 force) { this->force += force; }
};


constexpr int MAX_CONTACT_POINTS = 4;
struct ContactPoint
{
    Vector3 point;
    Vector3 normal;
    float penetration;
    float normalImpulse = 0.0f;
    float tangentImpulse = 0.0f;
};

struct Contact
{
    ContactPoint points[MAX_CONTACT_POINTS];
    Collider* a;
    Collider* b;
    char count;
};


class PhysicSystem {
public:
	PhysicSystem() {};
	~PhysicSystem() {};

	void solve(SphereCollider& s1, SphereCollider& s2);
	void solve(SphereCollider& s, TerrainCollider& t);

	void solve(SphereCollider& sphere, CubeCollider& cube);
	void solve(TerrainCollider& terrain, CubeCollider& cube);
	void solve(CubeCollider& cubeA, CubeCollider& cubeB);

	void solve(TerrainCollider& terrain, CapsuleCollider& capsule);
	void solve(SphereCollider& sphere, CapsuleCollider& capsule);
	void solve(CubeCollider& cube, CapsuleCollider& capsule);
	void solve(CapsuleCollider& capsule1, CapsuleCollider& capsule2);

	template<typename A, typename B>
    void solveAdapter(Collider* a, Collider* b);

	bool calculateRayCast(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit, char mask);
	void simulatePhysicStep();
	void calculatePhysic();
	void solve(Collider* a, Collider* b);

	inline void addContact(Contact& contact) {
		if(contact.count <= 0) return;
		uint64_t key = makePairKey(contact.a, contact.b);
		auto it = contactCache.find(key);
		if(it != contactCache.end()) {
			Contact& old = it->second;
			for(int i = 0; i < contact.count; i++) {
				for(int j = 0; j < old.count; j++) {
					if((contact.points[i].point - old.points[j].point).length() < 0.02f) {
						contact.points[i].normalImpulse = old.points[j].normalImpulse;
						contact.points[i].tangentImpulse = old.points[j].tangentImpulse;
					}
				}
			}
		}
		contacts.push_back(contact);
	}
	inline uint64_t  makePairKey(Collider* a, Collider* b) {
		uint64_t  A = a->object.getID();
		uint64_t  B = b->object.getID();
		if(A > B) std::swap(A,B);
		return (A << 32) ^ B;
	}
	inline static PhysicSystem* getInstance() {
		static PhysicSystem instance;
		return &instance;
	}
	inline static bool RayCast(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit, char mask) {
		return getInstance()->calculateRayCast(origin, direction, distance, rayHit, mask);
	}
private:
	std::vector<AABB> aabb;
	std::vector<Contact> contacts;
	std::unordered_map<uint64_t, Contact> contactCache;
};












class PhysicView {
public:
	PhysicView() {
		sphereColliders = ECS::GetComponents<SphereCollider>();
		terrainColliders = ECS::GetComponents<TerrainCollider>();
		cubeColliders = ECS::GetComponents<CubeCollider>();
		capsuleColliders = ECS::GetComponents<CapsuleCollider>();
		characterControllers = ECS::GetComponents<CharacterController>();
	};
	~PhysicView() {};

	Collider& operator[](unsigned int index) {
		unsigned int offset = 0;
        if (index < offset + sphereColliders.size()) {
            return sphereColliders[index - offset];
        }

        offset += sphereColliders.size();
        if (index < offset + terrainColliders.size()) {
            return terrainColliders[index - offset];
        }

        offset += terrainColliders.size();
        if (index < offset + cubeColliders.size()) {
            return cubeColliders[index - offset];
        }

        offset += cubeColliders.size();
        if (index < offset + capsuleColliders.size()) {
            return capsuleColliders[index - offset];
        }

        offset += capsuleColliders.size();
        return characterControllers[index - offset];
	}

	unsigned int size() const {
		return sphereColliders.size() + terrainColliders.size() + cubeColliders.size() + capsuleColliders.size() + characterControllers.size();
	}
private:
	Span<SphereCollider> sphereColliders;
	Span<TerrainCollider> terrainColliders;
	Span<CubeCollider> cubeColliders;
	Span<CapsuleCollider> capsuleColliders;
	Span<CharacterController> characterControllers;
};

#include "Physic.hpp"