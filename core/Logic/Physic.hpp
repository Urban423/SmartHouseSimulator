#pragma once
#include "Physic.h"
#include "Polygon.h"

inline void PhysicSystem::solve(SphereCollider& s, TerrainCollider& t) {
    Vector3 pos = s.object.transform.position + s.offset;
    float h = t.getHeight(pos.x, pos.z);
    float bottom = pos.y - s.radius;
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
    Vector3 delta = posA - posB;
    float dist2 = delta.sqrMagnitude();
    float radiusSum = s1.radius + s2.radius;
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
	contact.points[0].point = posA - normal * (s1.radius - penetration * 0.5f);
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
    Contact contact{};
    contact.a = &sphere;
    contact.b = &cube;
    contact.count = 0;
    Vector3 sphereCenter = sphere.object.transform.position + sphere.offset;
    Vector3 vertices[8];
    cube.getCubeVertices(vertices);
    Vector3 cubeCenter = (vertices[0] + vertices[6]) * 0.5f;
    Vector3 axes[3];
    axes[0] = (vertices[1] - vertices[0]).normalized();
    axes[1] = (vertices[2] - vertices[0]).normalized(); 
    axes[2] = (vertices[4] - vertices[0]).normalized();
    float minPen = FLT_MAX;
    Vector3 bestAxis;
    for (int i = 0; i < 3; i++) {
        Vector3 axis = axes[i];
        float cubeMin = FLT_MAX;
        float cubeMax = -FLT_MAX;
        for (int j = 0; j < 8; j++) {
            float p = Vector3::Dot(vertices[j], axis);
            cubeMin = std::min(cubeMin, p);
            cubeMax = std::max(cubeMax, p);
        }
        float sphereProj = Vector3::Dot(sphereCenter, axis);
        float sphereMin = sphereProj - sphere.radius;
        float sphereMax = sphereProj + sphere.radius;
        float overlap = std::min(cubeMax, sphereMax) - std::max(cubeMin, sphereMin);
        if (overlap < 0) return;
        if (overlap < minPen) {
            minPen = overlap;
            bestAxis = axis;
        }
    }
    if (Vector3::Dot(sphereCenter - cubeCenter, bestAxis) < 0) bestAxis = -bestAxis;
    ContactPoint& cp = contact.points[0];
    cp.penetration = minPen;
    cp.point = sphereCenter - bestAxis * sphere.radius;
    cp.normal = bestAxis;
    contact.count = 1;
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
        dispatch[(int)ColliderType::Sphere][(int)ColliderType::Cube] = &PhysicSystem::solveAdapter<SphereCollider, CubeCollider>;
        dispatch[(int)ColliderType::Sphere][(int)ColliderType::Terrain] = &PhysicSystem::solveAdapter<SphereCollider, TerrainCollider>;
        dispatch[(int)ColliderType::Terrain][(int)ColliderType::Cube] =  &PhysicSystem::solveAdapter<TerrainCollider, CubeCollider>;
        dispatch[(int)ColliderType::Cube][(int)ColliderType::Cube] =  &PhysicSystem::solveAdapter<CubeCollider, CubeCollider>;
        init = true;
    }
    SolveFn fn = dispatch[(int)a->type][(int)b->type];
    if (fn) (this->*fn)(a, b);
}

inline bool PhysicSystem::calculateRayCast(Vector3 origin, Vector3 direction, float distance, RayHit& rayHit) {
	PhysicView physicView;
	bool found = false;
    float closest = distance;
    direction = direction.normalized();
	for(int i = 0; i < physicView.size(); i++) {
        Collider& c = physicView[i];
        RayHit tempHit;
		if(c.calculateRayHit(origin, direction, distance, tempHit)) {
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
	auto [rigidbodies, rigidbody_count] = ECS::GetComponents<Rigidbody>();
	Vector3 gravity = Vector3(0, -9.81f, 0);
	for (int i = 0; i < rigidbody_count; i++)
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
        aabb[i] = physicView[i].calculateAABB();
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
            Vector3 inverseInertiaA = A ? c.a->getInverseInertia(A->mass) : 0.0f;
            Vector3 inverseInertiaB = B ? c.b->getInverseInertia(B->mass) : 0.0f;
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
                    float percent = 0.1f;
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
                    if (velAlongNormal < -2.0f) restitution = 0.3f;
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

    for(int i =0; i < rigidbody_count; i++) {
        if (rigidbodies[i].isKinematic) continue;
        rigidbodies[i].object.transform.position = rigidbodies[i].prevPosition + rigidbodies[i].velocity * dt;
    }
}

inline void PhysicSystem::calculatePhysic() {
	simulatePhysicStep(); 
}

