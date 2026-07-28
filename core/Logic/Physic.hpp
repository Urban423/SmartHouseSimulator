#pragma once
#include "Physic.h"
#include "Polygon.h"

inline void PhysicSystem::solve(SphereCollider& s, TerrainCollider& t) {
    Vector3 pos = s.object.transform.position + s.offset;
    float r = s.object.transform.scale.x * s.radius;
    float h = t.getHeight(pos.x, pos.z);
    float bottom = pos.y - r;
    if (bottom > h) {
		return;
	}

	ContactPoint p;
	p.point = Vector3(pos.x, h, pos.z);
	p.penetration = h - bottom;
	p.normal = t.getNormal(pos.x, pos.z);
	
	Contact contact;
	contact.a = &s;
	contact.b = &t;
	contact.count = 1;
	contact.points[0] = p;
    addContact(contact);
}

inline void PhysicSystem::solve(SphereCollider& s1, SphereCollider& s2)
{
    Vector3 posA = s1.object.transform.position + s1.offset;
    Vector3 posB = s2.object.transform.position + s2.offset;
    float r1 = s1.object.transform.scale.y * s1.radius;
    float r2 = s2.object.transform.scale.y * s2.radius;
    Vector3 delta = posA - posB;
    float dist2 = delta.sqrMagnitude();
    float radiusSum = r1 + r2;
    if (dist2 >= radiusSum * radiusSum) return;

    float dist = std::sqrt(dist2);
	Vector3 normal;
	if (dist < 0.0001f) normal = Vector3(0,1,0);
	else normal = delta / dist;
    float penetration = radiusSum - dist;


    Contact contact;
	contact.a = &s1;
	contact.b = &s2;
	contact.count = 1;
	contact.points[0].penetration = penetration;
	contact.points[0].point = posA - normal * (r1 - penetration * 0.5f);
	contact.points[0].normal = normal;
    addContact(contact);
}

inline void PhysicSystem::solve(TerrainCollider& terrain, CubeCollider& cube) {
    Contact contact{};
    contact.a = &cube;
    contact.b = &terrain;
    contact.count = 0;
    Vector3 vertices[8];
    cube.getCubeVertices(vertices);
    for (int i = 0; i < 8; i++) {
        const Vector3& v = vertices[i];
        float terrainHeight = terrain.getHeight(v.x, v.z);
        float penetration = terrainHeight - v.y;
        if (penetration <= 0.0f) continue;
        Vector3 point = v;
        if (contact.count < MAX_CONTACT_POINTS) {
            contact.points[contact.count].point = point;
            contact.points[contact.count].penetration = penetration;
            contact.points[contact.count].normal = terrain.getNormal(v.x, v.z);
            contact.count++;
        }
        else {
            int minIndex = 0;
            float minPen = contact.points[0].penetration;
            for (int j = 1; j < 4; j++) {
                if (contact.points[j].penetration < minPen) {
                    minPen = contact.points[j].penetration;
                    minIndex = j;
                }
            }
            if (penetration > minPen) {
                contact.points[minIndex].point = point;
                contact.points[minIndex].penetration = penetration;
                contact.points[minIndex].normal = terrain.getNormal(v.x, v.z);
            }
        }
    }
    addContact(contact);
}

inline void PhysicSystem::solve(SphereCollider& sphere, CubeCollider& cube) {
    Vector3 sphereCenter = sphere.object.transform.position + sphere.offset;
    float sphereRadius = sphere.object.transform.scale.x * sphere.radius;
    Vector3 localCenter = Quaternion::Inverse(cube.object.transform.rotation) * (sphereCenter - cube.object.transform.position);
   
    Vector3 half = cube.object.transform.scale * cube.size;
    Vector3 closest;
    closest.x = Math::clamp(localCenter.x, -half.x, half.x);
    closest.y = Math::clamp(localCenter.y, -half.y, half.y);
    closest.z = Math::clamp(localCenter.z, -half.z, half.z);
    
    Vector3 delta = localCenter - closest;
    float dist2 = delta.sqrMagnitude();
    if (dist2 >= sphereRadius * sphereRadius) return;

    Vector3 normal;
    float penetration;
    if (dist2 < 0.0001f) {
        float dx = half.x - fabs(localCenter.x);
        float dy = half.y - fabs(localCenter.y);
        float dz = half.z - fabs(localCenter.z);

        if (dx < dy && dx < dz) {
            normal = { localCenter.x > 0 ? 1.f : -1.f, 0, 0 };
            closest = { localCenter.x > 0 ? half.x : -half.x, localCenter.y, localCenter.z };
            penetration = dx;
        }
        else if (dy < dz) {
            normal = { 0, localCenter.y > 0 ? 1.f : -1.f, 0 };
            closest = { localCenter.x, localCenter.y > 0 ? half.y : -half.y, localCenter.z };
            penetration = dy;
        }
        else {
            normal = { 0, 0, localCenter.z > 0 ? 1.f : -1.f };
            closest = { localCenter.x, localCenter.y, localCenter.z > 0 ? half.z : -half.z };
            penetration = dz;
        }
    } else {
        float dist = std::sqrt(dist2);
        if (dist < 0.0001f) normal = Vector3(0,1,0);
        else normal = delta / dist;
        penetration = dist - sphereRadius;
    }

    Vector3 worldPoint = cube.object.transform.position + cube.object.transform.rotation * closest;
    Vector3 worldNormal = cube.object.transform.rotation * normal;

    Contact contact;
	contact.a = &sphere;
	contact.b = &cube;
	contact.count = 1;
	contact.points[0].penetration = penetration;
	contact.points[0].point = worldPoint;
	contact.points[0].normal = worldNormal;
    addContact(contact);
}

inline void PhysicSystem::solve(CubeCollider& cubeA, CubeCollider& cubeB) {
    Contact contact{};
    contact.a = &cubeA;
    contact.b = &cubeB;
    contact.count = 0;
    Vector3 vertsA[8];
    Vector3 vertsB[8];
    cubeA.getCubeVertices(vertsA);
    cubeB.getCubeVertices(vertsB);
    Vector3 centerA = (vertsA[0] + vertsA[6]) * 0.5f;
    Vector3 centerB = (vertsB[0] + vertsB[6]) * 0.5f;
    Vector3 axes[6];
    axes[0] = (vertsA[1] - vertsA[0]).normalized();
    axes[1] = (vertsA[2] - vertsA[0]).normalized();
    axes[2] = (vertsA[4] - vertsA[0]).normalized();
    axes[3] = (vertsB[1] - vertsB[0]).normalized();
    axes[4] = (vertsB[2] - vertsB[0]).normalized();
    axes[5] = (vertsB[4] - vertsB[0]).normalized();
    float minPen = FLT_MAX;
    Vector3 bestAxis;
    for (int i = 0; i < 6; i++) {
        Vector3 axis = axes[i];
        float minA = FLT_MAX;
        float maxA = -FLT_MAX;
        float minB = FLT_MAX;
        float maxB = -FLT_MAX;
        for (int j = 0; j < 8; j++) {
            float pA = Vector3::Dot(vertsA[j], axis);
            float pB = Vector3::Dot(vertsB[j], axis);
            minA = std::min(minA, pA);
            maxA = std::max(maxA, pA);
            minB = std::min(minB, pB);
            maxB = std::max(maxB, pB);
        }
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < 0) return;
        if (overlap < minPen) {
            minPen = overlap;
            bestAxis = axis;
        }
    }
    if(Vector3::Dot(centerB-centerA,bestAxis) < 0) bestAxis=-bestAxis;
    Vector3 normal = -bestAxis;
    for(int i = 0; i < 8; i++) {
        Vector3 p = vertsA[i];
        float d = Vector3::Dot(vertsB[0] - p, normal);
        if(d >= 0) {
            ContactPoint& cp = contact.points[contact.count];
            cp.point = p;
            cp.penetration = minPen;
            cp.normal = normal;
            contact.count++;
            if(contact.count == MAX_CONTACT_POINTS) break;
        }
    }
    for(int i = 0; i < 8 && contact.count < MAX_CONTACT_POINTS; i++) {
        Vector3 p = vertsB[i];
        float d = Vector3::Dot(p - vertsA[0], -normal);
        if(d >= 0) {
            ContactPoint& cp = contact.points[contact.count];
            cp.point = p;
            cp.normal = normal;
            cp.penetration = minPen;
            contact.count++;
        }
    }
    addContact(contact);
}

inline void PhysicSystem::solve(TerrainCollider& terrain, CapsuleCollider& capsule) {
    Vector3 points[2];
    capsule.getEdge(points[0], points[1]);
    float r1 = capsule.object.transform.scale.x * capsule.radius;

    Contact contact;
    contact.count = 0;
    contact.a = &capsule;
    contact.b = &terrain;

    for (int i = 0; i < 2; i++) {
        Vector3 point = points[i];
        float bottom = point.y - r1;
        float terrainHeight = terrain.getHeight(point.x, point.z);
        if (terrainHeight < bottom) continue;
        
        ContactPoint& cp = contact.points[contact.count++];
        cp.point = Vector3(point.x, terrainHeight, point.z);
	    cp.penetration = terrainHeight - bottom;
        cp.normal = terrain.getNormal(point.x, point.z);
    }
    if(contact.count) addContact(contact);
}

inline void PhysicSystem::solve(SphereCollider& sphere, CapsuleCollider& capsule) {
    Vector3 sphereCenter = sphere.object.transform.position + sphere.offset;
    Vector3 a1, a2;
    capsule.getEdge(a1, a2);
    float r1 = sphere.object.transform.scale.x * sphere.radius;
    float r2 = capsule.object.transform.scale.x * capsule.radius;

    Vector3 p1 = closestPointOnSegment(a1, a2,sphereCenter);

    Vector3 delta = sphereCenter - p1;
    float dist2 = delta.sqrMagnitude();
    float radiusSum = r1 + r2;
    if (dist2 >= radiusSum * radiusSum) return;

    float dist = std::sqrt(dist2);
	Vector3 normal;
	if (dist < 0.0001f) normal = Vector3(0,1,0);
	else normal = delta / dist;
    float penetration = radiusSum - dist;

    Contact contact;
    contact.count = 1;
    contact.a = &sphere;
    contact.b = &capsule;

    ContactPoint& cp = contact.points[0];
    cp.normal = normal;
    cp.penetration = penetration;
    cp.point = p1 + cp.normal * r1;

    addContact(contact);
}

inline void PhysicSystem::solve(CubeCollider& cube, CapsuleCollider& capsule) {
    Vector3 points[2];
    capsule.getEdge(points[0], points[1]);
    Quaternion invRot = Quaternion::Inverse(cube.object.transform.rotation);
    Vector3 p0 = invRot * (points[0] - cube.object.transform.position);
    Vector3 p1 = invRot * (points[1] - cube.object.transform.position);

    float radius = capsule.object.transform.scale.x * capsule.radius;

    Vector3 segment = p1 - p0;
    Vector3 segAxis = segment.normalized();
    Vector3 half = 0.5f * cube.size * cube.object.transform.scale;

    Vector3 axes[7];
    axes[0] = {1,0,0};
    axes[1] = {0,1,0};
    axes[2] = {0,0,1};
    axes[3] = segAxis;
    axes[4] = Vector3::Cross(segAxis, {1,0,0});
    axes[5] = Vector3::Cross(segAxis, {0,1,0});
    axes[6] = Vector3::Cross(segAxis, {0,0,1});
    float minPen = FLT_MAX;
    Vector3 bestAxis;
    
    for(int i = 0; i < 4; i++) {
        Vector3 axis = axes[i];
        float len2 = axis.sqrMagnitude();
        if(len2 < 0.00001f) continue;


        float a = Vector3::Dot(p0, axis);
        float b = Vector3::Dot(p1, axis);
        if(a > b) std::swap(a, b);
        float capsuleMin = a - radius;
        float capsuleMax = b + radius;

        float extent = half.x * fabs(axis.x) + half.y * fabs(axis.y) + half.z * fabs(axis.z);
        float boxMin = -extent;
        float boxMax = extent;
        float overlap = std::min(capsuleMax, boxMax) - std::max(capsuleMin, boxMin);

        if(overlap < 0) return;
        if(overlap < minPen) {
            minPen = overlap;
            bestAxis = axis;
        }
    }
    Vector3 boxCenter = Vector3(0,0,0);
    Vector3 capsuleCenter = (p0 + p1) * 0.5f;
    if(Vector3::Dot(capsuleCenter, bestAxis) < 0.0f) bestAxis = -bestAxis;


   Vector3 boxPoint;

    float ax = fabs(bestAxis.x);
    float ay = fabs(bestAxis.y);
    float az = fabs(bestAxis.z);

    if(ax > ay && ax > az)
    {
        boxPoint.x = bestAxis.x > 0 ? half.x : -half.x;
        boxPoint.y = Math::clamp(capsuleCenter.y, -half.y, half.y);
        boxPoint.z = Math::clamp(capsuleCenter.z, -half.z, half.z);
    }
    else if(ay > az)
    {
        boxPoint.y = bestAxis.y > 0 ? half.y : -half.y;
        boxPoint.x = Math::clamp(capsuleCenter.x, -half.x, half.x);
        boxPoint.z = Math::clamp(capsuleCenter.z, -half.z, half.z);
    }
    else
    {
        boxPoint.z = bestAxis.z > 0 ? half.z : -half.z;
        boxPoint.x = Math::clamp(capsuleCenter.x, -half.x, half.x);
        boxPoint.y = Math::clamp(capsuleCenter.y, -half.y, half.y);
    }

    Vector3 worldPoint = cube.object.transform.position + cube.object.transform.rotation * boxPoint;
    Vector3 worldNormal = cube.object.transform.rotation * bestAxis;

    Contact contact;
    contact.a = &capsule;
    contact.b = &cube;
    contact.count = 1;
    ContactPoint& cp = contact.points[0];
    cp.penetration = minPen;
    cp.point = worldPoint;
    cp.normal = worldNormal;
    addContact(contact);
}

inline void PhysicSystem::solve(CapsuleCollider& capsule1, CapsuleCollider& capsule2) {
    Vector3 a1, a2;
    Vector3 b1, b2;
    capsule1.getEdge(a1, a2);
    capsule2.getEdge(b1, b2);
    float r1 = capsule1.object.transform.scale.x * capsule1.radius;
    float r2 = capsule2.object.transform.scale.x * capsule2.radius;

    Vector3 p1, p2;
    closestPointsSegmentSegment(a1, a2, b1, b2, p1, p2);

    Vector3 delta = p2 - p1;
    float dist2 = delta.sqrMagnitude();
    float radiusSum = r1 + r2;
    if (dist2 >= radiusSum * radiusSum) return;

    float dist = std::sqrt(dist2);
	Vector3 normal;
	if (dist < 0.0001f) normal = Vector3(0,1,0);
	else normal = delta / dist;
    float penetration = radiusSum - dist;

    Contact contact;
    contact.count = 1;
    contact.a = &capsule2;
    contact.b = &capsule1;

    ContactPoint& cp = contact.points[0];
    cp.normal = normal;
    cp.penetration = penetration;
    cp.point = p1 + cp.normal * r1;

    addContact(contact);
}






template<typename A, typename B>
void PhysicSystem::solveAdapter(Collider* a, Collider* b) {
    solve(static_cast<A&>(*a), static_cast<B&>(*b));
}

inline void PhysicSystem::solve(Collider* a, Collider* b)
{
    if (a->type > b->type) std::swap(a, b);
    using SolveFn = void (PhysicSystem::*)(Collider*, Collider*);
    static SolveFn dispatch[(int)ColliderType::Count][(int)ColliderType::Count] = { nullptr };
    static bool init = false;
    if (!init) {
        dispatch[(int)ColliderType::Sphere][(int)ColliderType::Sphere] = &PhysicSystem::solveAdapter<SphereCollider, SphereCollider>;
        dispatch[(int)ColliderType::Sphere][(int)ColliderType::Terrain] = &PhysicSystem::solveAdapter<SphereCollider, TerrainCollider>;

        dispatch[(int)ColliderType::Sphere][(int)ColliderType::Cube] = &PhysicSystem::solveAdapter<SphereCollider, CubeCollider>;
        dispatch[(int)ColliderType::Terrain][(int)ColliderType::Cube] =  &PhysicSystem::solveAdapter<TerrainCollider, CubeCollider>;
        dispatch[(int)ColliderType::Cube][(int)ColliderType::Cube] =  &PhysicSystem::solveAdapter<CubeCollider, CubeCollider>;

        dispatch[(int)ColliderType::Sphere][(int)ColliderType::Capsule] =  &PhysicSystem::solveAdapter<SphereCollider, CapsuleCollider>;
        dispatch[(int)ColliderType::Terrain][(int)ColliderType::Capsule] =  &PhysicSystem::solveAdapter<TerrainCollider, CapsuleCollider>;
        dispatch[(int)ColliderType::Cube][(int)ColliderType::Capsule] =  &PhysicSystem::solveAdapter<CubeCollider, CapsuleCollider>;
        dispatch[(int)ColliderType::Capsule][(int)ColliderType::Capsule] =  &PhysicSystem::solveAdapter<CapsuleCollider, CapsuleCollider>;
        init = true;
    }
    int typeA = (int)a->type;
    int typeB = (int)b->type;
    if(dispatch[typeA][typeB]) (this->*dispatch[typeA][typeB])(a, b);
}

inline bool PhysicSystem::calculateRayCast(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit) {
	PhysicView physicView;
	bool found = false;
    float closest = distance;
    direction = direction.normalized();
	for(int i = 0; i < physicView.size(); i++) {
        Collider& c = physicView[i];
        RayHit tempHit;
		if(calculateRayHit(origin, direction, distance, tempHit, c)) {
			if(tempHit.distance < closest) {
				closest = tempHit.distance;
				rayHit = tempHit;
				found = true;
			}
		}
    }
	return found;
}

inline void PhysicSystem::simulatePhysicStep() {
	float dt = Time::fixedDeltaTime;
    if(dt == 0) return;

    float linearDamping = 0.05f;
    float angularDamping = 0.05f;
    Span<Rigidbody> rigidbodies = ECS::GetComponents<Rigidbody>();
	Vector3 gravity = Vector3(0, -9.81f, 0);
	for (int i = 0; i < rigidbodies.size(); i++)
    {
        Rigidbody& rb = rigidbodies[i];
		rb.prevPosition = rb.object.transform.position;
        if (rb.isKinematic) continue;

        rb.force += gravity * rb.mass;
        rb.acceleration = rb.force / rb.mass;
        rb.velocity += rb.acceleration * dt;
		rb.velocity *= std::exp(-linearDamping * dt);
        rb.force = Vector3(0, 0, 0);
        rb.angularVelocity += rb.torque * dt;
        rb.angularVelocity *= std::exp(-angularDamping * dt);
        rb.torque = Vector3(0, 0, 0);
		if (rb.linearLock & AxisLock::X) rb.velocity.x = 0;
		if (rb.linearLock & AxisLock::Y) rb.velocity.y = 0;
		if (rb.linearLock & AxisLock::Z) rb.velocity.z = 0;
        rb.object.transform.position += rb.velocity * dt;

		if (rb.angularLock & AxisLock::X) rb.angularVelocity.x = 0;
		if (rb.angularLock & AxisLock::Y) rb.angularVelocity.y = 0;
		if (rb.angularLock & AxisLock::Z) rb.angularVelocity.z = 0;
        float speed = rb.angularVelocity.length();
        if (speed > 0.00001f) {
            Vector3 axis = rb.angularVelocity / speed;
            Quaternion dq = Quaternion::FromAxisAngle(axis, speed * dt);
            rb.object.transform.rotation = dq * rb.object.transform.rotation;
        }
    }






	PhysicView physicView;
	contacts.clear();
	aabb.clear();
	aabb.resize(physicView.size());
	for (int i = 0; i < physicView.size(); ++i) {
        aabb[i] = calculateAABB(physicView[i]);
		aabb[i].index = i;
    }


    // Sort AABBs based on the X axis
    std::sort(aabb.begin(), aabb.end(), [](const AABB a, const AABB b) {
        return a.min.x < b.min.x;
    });

    // Detect collisions
    for (int i = 0; i < aabb.size(); ++i) {
        AABB objA = aabb[i];
        for (int j = i + 1; j < aabb.size(); ++j) {
            AABB objB = aabb[j];
            if (objB.min.x > objA.max.x) break; // Early exit

			if(objA.max.y < objB.min.y || objA.min.y > objB.max.y) { continue; }
			if(objA.max.z < objB.min.z || objA.min.z > objB.max.z) { continue; } 
			solve(&physicView[objA.index], &physicView[objB.index]);
		}
	}

    const int iterations = 8;
    for (int iter = 0; iter < iterations; iter++)
    {
        for (Contact& c : contacts) {
            Rigidbody* A = c.a->object.HasComponent<Rigidbody>() ? &c.a->object.GetComponent<Rigidbody>() : nullptr;
            Rigidbody* B = c.b->object.HasComponent<Rigidbody>() ? &c.b->object.GetComponent<Rigidbody>() : nullptr;
            if (!A && !B) continue;
            Vector3 inverseInertiaA = A ? getInverseInertia(A->mass, *c.a) : 0.0f;
            Vector3 inverseInertiaB = B ? getInverseInertia(B->mass, *c.b) : 0.0f;
            float invMassA = A ? 1.0f / A->mass : 0.0f;
            float invMassB = B ? 1.0f / B->mass : 0.0f;
            float invMassSum = invMassA + invMassB;
            if (invMassSum == 0) continue;
            Vector3 startVelocityA = A ? A->velocity : Vector3(0);
            Vector3 startVelocityB = B ? B->velocity : Vector3(0);
            Vector3 startAngularA = A ? A->angularVelocity : Vector3(0);
            Vector3 startAngularB = B ? B->angularVelocity : Vector3(0);
            for(int i = 0; i < c.count; i++) {
                Vector3 normal = c.points[i].normal;
                Vector3 contactPoint = c.points[i].point;
                if(iter == 0) {
                    float slop = 0.01f;
                    float percent = 0.2f;
                    float correctionAmount = std::max(c.points[i].penetration - slop, 0.0f) / invMassSum * percent;
                    Vector3 correction = normal * correctionAmount;
                    if(A) A->prevPosition += correction * invMassA;
                    if(B) B->prevPosition -= correction * invMassB;
                }

                Vector3 Apos = c.a->object.transform.position;
                Vector3 Bpos = c.b->object.transform.position;
                if(A) Apos = A->prevPosition;
                if(B) Bpos = B->prevPosition;
                Vector3 ra = contactPoint - Apos;
                Vector3 rb = contactPoint - Bpos;
                float normalImpulse = 0;
                float oldImpulse = c.points[i].normalImpulse;
                Vector3 rv(0,0,0);
                if(A) rv += startVelocityA + Vector3::Cross(startAngularA, ra);
                if(B) rv -= startVelocityB + Vector3::Cross(startAngularB, rb);
                float velAlongNormal = Vector3::Dot(rv, normal);
                if(velAlongNormal < -0.0f) {
                    Vector3 raCrossN = Vector3::Cross(ra, normal);
                    Vector3 rbCrossN = Vector3::Cross(rb, normal);
                    float denominator = invMassSum + Vector3::Dot(raCrossN, inverseInertiaA * raCrossN) + Vector3::Dot(rbCrossN, inverseInertiaB * rbCrossN);
                    float restitution = 0.0f;
                    if (velAlongNormal < -2.0f) restitution = 0.1f;
                    normalImpulse = -(1.0f + restitution) * velAlongNormal / denominator;
                    normalImpulse /= c.count;
                    
                    float newImpulse = oldImpulse + normalImpulse;
                    newImpulse = std::max(newImpulse, 0.0f);
                    float deltaImpulse = newImpulse - oldImpulse;
                    c.points[i].normalImpulse = newImpulse;
                    Vector3 impulse = normal * deltaImpulse; 
                    if(A) {
                        A->velocity += impulse * invMassA;
                        Vector3 angularImpulse = Vector3::Cross(ra, impulse);
                        A->angularVelocity += Vector3(angularImpulse.x * inverseInertiaA.x, angularImpulse.y * inverseInertiaA.y, angularImpulse.z * inverseInertiaA.z);
                    }
                    if(B) {
                        B->velocity -= impulse * invMassB;
                        Vector3 angularImpulse = Vector3::Cross(rb, impulse);
                        B->angularVelocity -= Vector3(angularImpulse.x * inverseInertiaB.x, angularImpulse.y * inverseInertiaB.y, angularImpulse.z * inverseInertiaB.z);
                    }
                }
                Vector3 tangent = rv - normal * Vector3::Dot(rv, normal);
                float tangentLen = tangent.length();
                if (tangentLen > 0.0001f) tangent = tangent / tangentLen;
                else tangent = Vector3(0, 0, 0);
                float denom =
                    invMassSum
                    + Vector3::Dot(Vector3::Cross(ra, tangent), inverseInertiaA * Vector3::Cross(ra, tangent))
                    + Vector3::Dot(Vector3::Cross(rb, tangent), inverseInertiaB * Vector3::Cross(rb, tangent));
                float jt = -Vector3::Dot(rv, tangent);
                jt /= denom;
                float mu = 0.6f;
                Vector3 frictionImpulse;
                float maxFriction = normalImpulse * mu;
                if (std::abs(jt) < maxFriction) {
                    frictionImpulse = tangent * jt;
                }
                else {
                    frictionImpulse = tangent * (-maxFriction);
                }
                if (A) {
                    A->velocity += frictionImpulse * invMassA;
                    Vector3 angularImpulse = Vector3::Cross(ra, frictionImpulse);
                    A->angularVelocity += Vector3(angularImpulse.x * inverseInertiaA.x, angularImpulse.y * inverseInertiaA.y, angularImpulse.z * inverseInertiaA.z);
                }
                if (B) {
                    B->velocity -= frictionImpulse * invMassB;
                    Vector3 angularImpulse = Vector3::Cross(rb, frictionImpulse);
                    B->angularVelocity -= Vector3(angularImpulse.x * inverseInertiaB.x, angularImpulse.y * inverseInertiaB.y, angularImpulse.z * inverseInertiaB.z);
                }
            }
        }
    }

    for(Contact& c : contacts)
    {
        contactCache[makePairKey(c.a,c.b)] = c;
    }

    for(int i =0; i < rigidbodies.size(); i++) {
        if (rigidbodies[i].isKinematic) continue;
        rigidbodies[i].object.transform.position = rigidbodies[i].prevPosition + rigidbodies[i].velocity * dt;
    }
}

inline void PhysicSystem::calculatePhysic() {
	simulatePhysicStep(); 
}