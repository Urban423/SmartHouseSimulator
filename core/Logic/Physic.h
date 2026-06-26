#pragma once
#include "Transform.h"
#include "ECS.h"
#include "umath.h"
#include <cfloat>
#include <algorithm>
#include <cstdint>


enum class ColliderType {
    Sphere,
    Terrain,
    Cube,
	Count,
};

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

struct AABB {
	Vector3 min;
	Vector3 max;
	int index;
}; 


struct RayHit;
struct Collider : public Component {
    ColliderType type;
	virtual AABB calculateAABB() = 0;
	virtual bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit) = 0;
	virtual Vector3 getInverseInertia(float mass) = 0;
};

struct RayHit {
	Collider* collider;
	Vector3 point;
	Vector3 normal;
	float distance;
};

struct SphereCollider: public Collider
{
    SphereCollider() { type = ColliderType::Sphere; }

	Vector3 offset;
	float radius = 1;

	AABB calculateAABB() {
		Vector3 pos = object.transform.position + offset;
		return {pos - Vector3(radius, radius, radius), pos + Vector3(radius, radius, radius), object.getID()};
	}

	inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance,  RayHit& hit) {
		Vector3 c = object.transform.position + offset;
		float r = radius;
		Vector3 oc = origin - c;
		float a = Vector3::Dot(direction, direction);
		float b = 2.0f * Vector3::Dot(oc, direction);
		float c2 = Vector3::Dot(oc, oc) - r * r;
		float discriminant = b * b - 4 * a * c2;
		printf("chechiking\n");
		if (discriminant < 0.0f) return false;
		float sqrtD = sqrt(discriminant);
		float t1 = (-b - sqrtD) / (2.0f * a);
		float t2 = (-b + sqrtD) / (2.0f * a);
		float t = t1;
		if (t < 0.0f) t = t2;
		if (t < 0.0f || t > distance) return false;
		hit.distance = t;
		hit.point = origin + direction * t;
		hit.normal = (hit.point - c).normalized();
		hit.collider = this;
		return true;
	}

	inline Vector3 getInverseInertia(float mass) {
		float inertia = 0.4f * mass * radius * radius;
		return 1.0f / inertia;
	}
};

struct CubeCollider: public Collider {
	CubeCollider() { type = ColliderType::Cube; }

	Vector3 offset = Vector3(0, 0, 0);
	Vector3 size= Vector3(1, 1, 1);

	inline void getCubeVertices(Vector3 out[8]) {
		Transform& tr = object.transform;
		Vector3 center = tr.position + offset;
		Vector3 half(size.x * tr.scale.x * 0.5f, size.y * tr.scale.y * 0.5f, size.z * tr.scale.z * 0.5f);
		Quaternion rot = tr.rotation;
		int idx = 0;
		for (int x = -1; x <= 1; x += 2) {
			for (int y = -1; y <= 1; y += 2) {
				for (int z = -1; z <= 1; z += 2) {
					Vector3 local(half.x * x, half.y * y, half.z * z);
					out[idx++] = center + rot * local;
				}
			}
		}
	}

	AABB calculateAABB() {
		Vector3 center = object.transform.position + offset;
		Vector3 halfSize = Vector3(size.x * object.transform.scale.x * 0.5f, size.y * object.transform.scale.y * 0.5f, size.z * object.transform.scale.z * 0.5f);
		Quaternion rot = object.transform.rotation;
		Vector3 right   = rot * Vector3(1, 0, 0);
		Vector3 up      = rot * Vector3(0, 1, 0);
		Vector3 forward = rot * Vector3(0, 0, 1);
		Vector3 extents(
			fabs(right.x)   * halfSize.x +
			fabs(up.x)      * halfSize.y +
			fabs(forward.x) * halfSize.z,

			fabs(right.y)   * halfSize.x +
			fabs(up.y)      * halfSize.y +
			fabs(forward.y) * halfSize.z,

			fabs(right.z)   * halfSize.x +
			fabs(up.z)      * halfSize.y +
			fabs(forward.z) * halfSize.z
		);

		return {center - extents, center + extents, object.getID()};
	}

	inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& hit) {
		Transform& transform = object.transform;
		Vector3 center = transform.position + offset;
		Vector3 halfSize = Vector3(size.x * object.transform.scale.x * 0.5f, size.y * object.transform.scale.y * 0.5f, size.z * object.transform.scale.z * 0.5f);
		Quaternion invRot = Quaternion::Inverse(transform.rotation);
		Vector3 localOrigin = invRot * (origin - center);
		Vector3 localDir = invRot * direction;
		float tMin = 0.0f;
		float tMax = distance;
		for (int i = 0; i < 3; i++) {
			float o = localOrigin[i];
			float d = localDir[i];
			float min = -halfSize[i];
			float max =  halfSize[i];
			if (fabs(d) < 0.00001f) {
				if (o < min || o > max) return false;
			}
			else {
				float invD = 1.0f / d;
				float t1 = (min - o) * invD;
				float t2 = (max - o) * invD;
				if (t1 > t2) std::swap(t1, t2);
				tMin = std::max(tMin, t1);
				tMax = std::min(tMax, t2);
				if (tMin > tMax) return false;
			}
		}
		float t = tMin;
		if (t < 0.0f || t > distance) return false;
		hit.distance = t;
		hit.collider = this;
		hit.point = origin + direction * t;
		Vector3 localHit = localOrigin + localDir * t;
		const float eps = 0.001f;
		Vector3 localNormal(0, 0, 0);
		if (fabs(localHit.x - halfSize.x) < eps) localNormal = { 1, 0, 0 };
		else if (fabs(localHit.x + halfSize.x) < eps) localNormal = { -1, 0, 0 };
		else if (fabs(localHit.y - halfSize.y) < eps) localNormal = { 0, 1, 0 };
		else if (fabs(localHit.y + halfSize.y) < eps) localNormal = { 0, -1, 0 };
		else if (fabs(localHit.z - halfSize.z) < eps) localNormal = { 0, 0, 1 };
		else if (fabs(localHit.z + halfSize.z) < eps) localNormal = { 0, 0, -1 };
		hit.normal = transform.rotation * localNormal;
		return true;
	}

	inline Vector3 getInverseInertia(float mass) {
		Vector3 scale = object.transform.scale;
		float w = scale.x * size.x * 2;
		float h = scale.y * size.y * 2;
		float d = scale.z * size.z * 2;
		Vector3 inertia;
		inertia.x = (1.0f / 12.0f) * mass * (h*h + d*d);
		inertia.y = (1.0f / 12.0f) * mass * (w*w + d*d);
		inertia.z = (1.0f / 12.0f) * mass * (w*w + h*h);
		return Vector3(
			1.0f / inertia.x,
			1.0f / inertia.y,
			1.0f / inertia.z
		);
	}
};

struct TerrainCollider: public Collider {
    TerrainCollider() { type = ColliderType::Terrain; }

	std::vector<float> heightMap;
    int width = 0;
    int height = 0;

	inline float getHeight(float x, float z) {
		Vector3 pos = object.transform.position;
		Vector3 scale = object.transform.scale;
		float localX = (x - pos.x) / scale.x;
		float localZ = (z - pos.z) / scale.z;
		int x0 = (int)floor(localX);
		int x1 = x0 + 1;
		int z0 = (int)floor(localZ);
		int z1 = z0 + 1;

		if (x0 < 0) x0 = 0;
		if (z0 < 0) z0 = 0;
		if (x1 < 0) x1 = 0;
		if (z1 < 0) z1 = 0;

		if (x0 >= width)  x0 = width - 1;
		if (x1 >= width)  x1 = width - 1;
		if (z0 >= height) z0 = height - 1;
		if (z1 >= height) z1 = height - 1;

		float tx = localX - x0;
		float tz = localZ - z0;

		float h00 = heightMap[z0 * width + x0];
		float h10 = heightMap[z0 * width + x1];
		float h01 = heightMap[z1 * width + x0];
		float h11 = heightMap[z1 * width + x1];

		float h0 = h00 * (1 - tx) + h10 * tx;
		float h1 = h01 * (1 - tx) + h11 * tx;

		return pos.y + scale.y * (h0 * (1 - tz) + h1 * tz);
	}
	
	inline Vector3 getNormal(float x, float z) {
		Vector3 pos = object.transform.position;
		Vector3 scale = object.transform.scale;
		float localX = (x - pos.x) / scale.x;
		float localZ = (z - pos.z) / scale.z;
		int ix = (int)floor(localX);
		int iz = (int)floor(localZ);

		if (ix <= 0 || ix >= width - 1 || iz <= 0 || iz >= height - 1) {
			return Vector3(0.0f, 1.0f, 0.0f);
		}

		float hL = heightMap[iz * width + (ix - 1)];
		float hR = heightMap[iz * width + (ix + 1)];
		float hD = heightMap[(iz - 1) * width + ix];
		float hU = heightMap[(iz + 1) * width + ix];

		float dx = (hR - hL) * scale.y / (2.0f * scale.x);
		float dz = (hU - hD) * scale.y / (2.0f * scale.z);

		Vector3 normal(-dx, 1.0f, -dz);
		return normal.normalized();
	}

	AABB calculateAABB() {
		Vector3 scale = object.transform.scale;
		Vector3 pos = object.transform.position;
		float minY = -10000.0f;
		float maxY = 100000.0f;
		return {
			Vector3(pos.x, minY, pos.z),
			Vector3(pos.x + width * scale.x, maxY, pos.z + height * scale.z),
			object.getID()
		};
	}

	inline Vector3 getInverseInertia(float mass) {
		return 0.0f;
	}

	inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& hit) {
		float step = 0.5f;
		float t = 0.0f;
		Vector3 prevPoint = origin;
		float prevHeightDiff = 0.0f;
		for (; t < distance; t += step) {
			Vector3 p = origin + direction * t;
			float h = getHeight(p.x, p.z);
			float diff = p.y - h;
			if (diff <= 0.0f) {
				float t0 = t - step;
				float t1 = t;
				for (int i = 0; i < 6; i++) {
					float tm = (t0 + t1) * 0.5f;
					Vector3 pm = origin + direction * tm;
					float hm = getHeight(pm.x, pm.z);
					if (pm.y > hm) t0 = tm;
					else t1 = tm;
				}
				float finalT = t1;
				if (finalT < 0.0f || finalT > distance) return false;
				Vector3 finalP = origin + direction * finalT;
				hit.distance = finalT;
				hit.point = finalP;
				float eps = 1.0f;
				float hL = getHeight(finalP.x - eps, finalP.z);
				float hR = getHeight(finalP.x + eps, finalP.z);
				float hD = getHeight(finalP.x, finalP.z - eps);
				float hU = getHeight(finalP.x, finalP.z + eps);
				hit.normal = Vector3(hL - hR, 2.0f, hD - hU).normalized();
				hit.collider = this;
				return true;
			}
			prevPoint = p;
			prevHeightDiff = diff;
		}
		return false;
	}
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

	template<typename A, typename B>
    void solveAdapter(Collider* a, Collider* b);

	bool calculateRayCast(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit);
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
		uint64_t  A = (uint64_t )a;
		uint64_t  B = (uint64_t )b;
		if(A > B) std::swap(A,B);
		return (A << 32) ^ B;
	}
	inline static PhysicSystem* getInstance() {
		static PhysicSystem instance;
		return &instance;
	}
	inline static bool RayCast(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit) {
		return getInstance()->calculateRayCast(origin, direction, distance, rayHit);
	}
private:
	std::vector<AABB> aabb;
	std::vector<Contact> contacts;
	std::unordered_map<uint64_t, Contact> contactCache;
};












class PhysicView {
public:
	PhysicView() {
		auto [sphereColliders, sphereColliderCount] = ECS::GetComponents<SphereCollider>();
		this->sphereColliders = sphereColliders;
		this->sphereColliderCount = sphereColliderCount;

		auto [terrainColliders, terrainColliderCount] = ECS::GetComponents<TerrainCollider>();
		this->terrainColliders = terrainColliders;
		this->terrainColliderCount = terrainColliderCount;

		auto [cubeColliders, cubeColliderCount] = ECS::GetComponents<CubeCollider>();
		this->cubeColliders = cubeColliders;
		this->cubeColliderCount = cubeColliderCount;
	};
	~PhysicView() {};

	Collider& operator[](unsigned int index) {
		unsigned int spheresAndTerrainsCount = sphereColliderCount + terrainColliderCount;
		if (index < sphereColliderCount) {
			return sphereColliders[index];
		} else if(index < spheresAndTerrainsCount) {
			return terrainColliders[index - sphereColliderCount];
		} else {
			return cubeColliders[index - spheresAndTerrainsCount];
		}
	}

	unsigned int size() const {
		return sphereColliderCount + terrainColliderCount + cubeColliderCount;
	}
private:
	SphereCollider* sphereColliders;
	unsigned int sphereColliderCount;

	TerrainCollider* terrainColliders;
	unsigned int terrainColliderCount;

	CubeCollider* cubeColliders;
	unsigned int cubeColliderCount;
};

#include "Physic.hpp"