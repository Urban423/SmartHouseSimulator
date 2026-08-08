#pragma once
#include "Transform.h"

enum class ColliderType {
    Sphere,
    Terrain,
    Cube,
    Capsule,
	CharacterController,
	Count,
};

struct AABB {
	Vector3 min;
	Vector3 max;
	int index;
}; 

struct RayHit;
struct Collider : public Component {
    ColliderType type;
    char mask = 0;
};


struct RayHit {
	Collider* collider;
	Vector3 point;
	Vector3 normal;
	float distance;
};

struct SphereCollider: public Collider {
    SphereCollider() { type = ColliderType::Sphere; }

	Vector3 offset;
	float radius = 0.5f;
};


struct CapsuleCollider: public Collider {
    CapsuleCollider() { type = ColliderType::Capsule; }

    void getEdge(Vector3& top, Vector3& bottom) {
        Transform& tr = object.transform;
		Vector3 center = tr.position + offset;
		Quaternion rot = tr.rotation;
        float scale = tr.scale[1];
        float halfHeight = height * 0.5f * scale;

        Vector3 axis{0, halfHeight, 0};
        top = center + rot * axis;
        bottom = center - rot * axis;
    }

	Vector3 offset;
	float radius = 0.5f;
	float height = 1.0f;
};

struct CharacterController : public CapsuleCollider {
    CharacterController() {
        type = ColliderType::CharacterController;
    }

    float stepHeight = 0.3f;
    float slopeLimit = 45.0f;
    bool grounded = false;
    Vector3 groundNormal;
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
};








inline AABB calculateAABB(SphereCollider& collider) {
    Vector3 pos = collider.object.transform.position + collider.offset;
    float radius = collider.radius * collider.object.transform.scale.y;
    return {pos - Vector3(radius, radius, radius), pos + Vector3(radius, radius, radius), collider.object.getID()};
}

inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& hit, SphereCollider& collider) {
    Vector3 c = collider.object.transform.position + collider.offset;
    float r = collider.radius * collider.object.transform.scale.y;
    Vector3 oc = origin - c;
    float a = Vector3::Dot(direction, direction);
    float b = 2.0f * Vector3::Dot(oc, direction);
    float c2 = Vector3::Dot(oc, oc) - r * r;
    float discriminant = b * b - 4 * a * c2;
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
    hit.collider = &collider;
    return true;
}

inline Vector3 getInverseInertia(float mass, SphereCollider& collider) {
    float inertia = 0.4f * mass * collider.radius * collider.radius;
    return 1.0f / inertia;
}

inline Vector3 getCenterMass(SphereCollider& collider) {
    return collider.offset + collider.object.transform.position;
}






inline AABB calculateAABB(CubeCollider& collider) {
    Vector3 center = collider.object.transform.position + collider.offset;
    Vector3 halfSize = Vector3(
        collider.size.x * collider.object.transform.scale.x * 0.5f, 
        collider.size.y * collider.object.transform.scale.y * 0.5f,
        collider.size.z * collider.object.transform.scale.z * 0.5f);
    Quaternion rot = collider.object.transform.rotation;
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

    return {center - extents, center + extents, collider.object.getID()};
}

inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& hit, CubeCollider& collider) {
    Transform& transform = collider.object.transform;
    Vector3 center = transform.position + collider.offset;
    Vector3 halfSize = Vector3(
        collider.size.x * collider.object.transform.scale.x * 0.5f, 
        collider.size.y * collider.object.transform.scale.y * 0.5f,
        collider.size.z * collider.object.transform.scale.z * 0.5f);
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
    hit.collider = &collider;
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

inline Vector3 getInverseInertia(float mass, CubeCollider& collider) {
    Vector3 scale = collider.object.transform.scale;
    float w = scale.x * collider.size.x * 2;
    float h = scale.y * collider.size.y * 2;
    float d = scale.z * collider.size.z * 2;
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









inline AABB calculateAABB(TerrainCollider& collider) {
    Vector3 scale = collider.object.transform.scale;
    Vector3 pos = collider.object.transform.position;
    float minY = -10000.0f;
    float maxY = 100000.0f;
    return {
        Vector3(pos.x, minY, pos.z),
        Vector3(pos.x + collider.width * scale.x, maxY, pos.z + collider.height * scale.z),
        collider.object.getID()
    };
}

inline Vector3 getInverseInertia(float mass, TerrainCollider& collider) {
    return 0.0f;
}

inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& hit, TerrainCollider& collider) {
    float step = 0.5f;
    float t = 0.0f;
    Vector3 prevPoint = origin;
    float prevHeightDiff = 0.0f;
    for (; t < distance; t += step) {
        Vector3 p = origin + direction * t;
        float h = collider.getHeight(p.x, p.z);
        float diff = p.y - h;
        if (diff <= 0.0f) {
            float t0 = t - step;
            float t1 = t;
            for (int i = 0; i < 6; i++) {
                float tm = (t0 + t1) * 0.5f;
                Vector3 pm = origin + direction * tm;
                float hm = collider.getHeight(pm.x, pm.z);
                if (pm.y > hm) t0 = tm;
                else t1 = tm;
            }
            float finalT = t1;
            if (finalT < 0.0f || finalT > distance) return false;
            Vector3 finalP = origin + direction * finalT;
            hit.distance = finalT;
            hit.point = finalP;
            float eps = 1.0f;
            hit.normal = collider.getNormal(finalP.x, finalP.z);
            hit.collider = &collider;
            return true;
        }
        prevPoint = p;
        prevHeightDiff = diff;
    }
    return false;
}













inline AABB calculateAABB(CapsuleCollider& collider) {
    Vector3 points[2];
    collider.getEdge(points[0], points[1]);
    float radius = collider.object.transform.scale.x * collider.radius;

    AABB box;

    box.min.x = std::min(points[0].x, points[1].x) - radius;
    box.min.y = std::min(points[0].y, points[1].y) - radius;
    box.min.z = std::min(points[0].z, points[1].z) - radius;

    box.max.x = std::max(points[0].x, points[1].x) + radius;
    box.max.y = std::max(points[0].y, points[1].y) + radius;
    box.max.z = std::max(points[0].z, points[1].z) + radius;

    return box;
}


inline Vector3 getInverseInertia(float mass, CapsuleCollider& collider)
{
    const float r = collider.radius;
    const float h = std::max(0.0f, collider.height - 2.0f * r);

    const float cylVolume    = Math::PI * r * r * h;
    const float sphereVolume = (4.0f / 3.0f) * Math::PI * r * r * r;

    const float totalVolume = cylVolume + sphereVolume;

    const float cylMass    = mass * cylVolume / totalVolume;
    const float sphereMass = mass * sphereVolume / totalVolume;

    // Цилиндр
    const float IyyCyl = 0.5f * cylMass * r * r;
    const float IxxCyl = (1.0f / 12.0f) * cylMass * (3.0f * r * r + h * h);

    // Сферическая часть (две полусферы)
    const float d = h * 0.5f + 3.0f * r / 8.0f;

    const float IyySphere = 0.4f * sphereMass * r * r;
    const float IxxSphere = IyySphere + sphereMass * d * d;

    const float Ix = IxxCyl + IxxSphere;
    const float Iy = IyyCyl + IyySphere;

    return {
        1.0f / Ix,
        1.0f / Iy,
        1.0f / Ix
    };
}

inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& hit, CapsuleCollider& collider) {
    Vector3 center = collider.object.transform.position + collider.offset;

    float radius = collider.radius * collider.object.transform.scale.x;
    float height = collider.height * collider.object.transform.scale.y;

    // Endpoints of the capsule's center line
    float halfLine = std::max(0.0f, height * 0.5f - radius);

    Vector3 a = center + Vector3(0, -halfLine, 0);
    Vector3 b = center + Vector3(0,  halfLine, 0);

    Vector3 ba = b - a;
    Vector3 oa = origin - a;

    float baba = Vector3::Dot(ba, ba);
    float bard = Vector3::Dot(ba, direction);
    float baoa = Vector3::Dot(ba, oa);
    float rdoa = Vector3::Dot(direction, oa);
    float oaoa = Vector3::Dot(oa, oa);

    float A = baba - bard * bard;
    float B = baba * rdoa - baoa * bard;
    float C = baba * oaoa - baoa * baoa - radius * radius * baba;

    float t = FLT_MAX;
    bool found = false;

    // -------- Cylinder --------
    float h = B * B - A * C;
    if (h >= 0.0f && fabs(A) > 1e-6f)
    {
        float s = (-B - sqrtf(h)) / A;
        float y = baoa + s * bard;

        if (y > 0.0f && y < baba && s >= 0.0f && s <= distance)
        {
            t = s;
            found = true;
        }
    }

    // -------- Sphere Caps --------
    auto testSphere = [&](const Vector3& centerPoint)
    {
        Vector3 oc = origin - centerPoint;

        float a2 = Vector3::Dot(direction, direction);
        float b2 = 2.0f * Vector3::Dot(oc, direction);
        float c2 = Vector3::Dot(oc, oc) - radius * radius;

        float disc = b2 * b2 - 4.0f * a2 * c2;
        if (disc < 0.0f)
            return;

        float sd = sqrtf(disc);

        float t0 = (-b2 - sd) / (2.0f * a2);
        float t1 = (-b2 + sd) / (2.0f * a2);

        float hitT = (t0 >= 0.0f) ? t0 : t1;

        if (hitT >= 0.0f && hitT <= distance && hitT < t)
        {
            t = hitT;
            found = true;
        }
    };

    testSphere(a);
    testSphere(b);

    if (!found)
        return false;

    hit.distance = t;
    hit.point = origin + direction * t;

    Vector3 pa = hit.point - a;
    float proj = Vector3::Dot(pa, ba) / baba;
    proj = std::clamp(proj, 0.0f, 1.0f);

    Vector3 closest = a + ba * proj;

    hit.normal = (hit.point - closest).normalized();
    hit.collider = &collider;

    return true;
}

inline Vector3 getCenterMass(CapsuleCollider& collider) {
    return collider.object.transform.position + collider.offset;
}









inline AABB calculateAABB(Collider& collider) {
    switch (collider.type) {
        case ColliderType::Sphere:
            return calculateAABB(static_cast<SphereCollider&>(collider));

        case ColliderType::Cube:
            return calculateAABB(static_cast<CubeCollider&>(collider));

        case ColliderType::Terrain:
            return calculateAABB(static_cast<TerrainCollider&>(collider));

        case ColliderType::Capsule:
            return calculateAABB(static_cast<CapsuleCollider&>(collider));

        case ColliderType::CharacterController:
            return calculateAABB(static_cast<CapsuleCollider&>(collider));

        default:
            return AABB{};
    }
}

inline bool calculateRayHit(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit, Collider& collider, char mask) {
    if(collider.mask != mask) return false;
    switch (collider.type) {
        case ColliderType::Sphere:
            return calculateRayHit(origin, direction, distance, rayHit, static_cast<SphereCollider&>(collider));

        case ColliderType::Cube:
            return calculateRayHit(origin, direction, distance, rayHit, static_cast<CubeCollider&>(collider));

        case ColliderType::Terrain:
            return calculateRayHit(origin, direction, distance, rayHit, static_cast<TerrainCollider&>(collider));

        case ColliderType::Capsule:
            return calculateRayHit(origin, direction, distance, rayHit, static_cast<CapsuleCollider&>(collider));

        case ColliderType::CharacterController:
            return calculateRayHit(origin, direction, distance, rayHit, static_cast<CapsuleCollider&>(collider));

        default:
            return false;
    }
}

inline Vector3 getInverseInertia(float mass, Collider& collider) {
    switch (collider.type) {
        case ColliderType::Sphere:
            return getInverseInertia(mass, static_cast<SphereCollider&>(collider));

        case ColliderType::Cube:
            return getInverseInertia(mass, static_cast<CubeCollider&>(collider));

        case ColliderType::Terrain:
            return getInverseInertia(mass, static_cast<TerrainCollider&>(collider));

        case ColliderType::Capsule:
        case ColliderType::CharacterController:
            return getInverseInertia(mass, static_cast<CapsuleCollider&>(collider));

        default:
            return Vector3{};
    }
}

inline Vector3 getCenterMass(Collider& collider) {
    switch (collider.type) {
        case ColliderType::Sphere:
            return getCenterMass(static_cast<SphereCollider&>(collider));


        case ColliderType::Capsule:
        case ColliderType::CharacterController:
            return getCenterMass(static_cast<CapsuleCollider&>(collider));


        case ColliderType::Cube:
        case ColliderType::Terrain:
        default:
            return Vector3{};
    }
}