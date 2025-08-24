#pragma once
#include "Transform.h"
#include "umath.h"
#include <algorithm>

struct SphereCollision
{
	Vector3 offset;
	float radius = 1;
};

struct AABB
{
	Vector3 min;
	Vector3 max;
	unsigned int index;
}; 

inline void solveSphereToSphere(Vector3& a, Vector3& b,  SphereCollision s1, SphereCollision s2)
{	
	Vector3 posA(a);
	Vector3 posB(b);
	posA += s1.offset;
	posB += s2.offset;
	
	float size = s1.radius;
	Vector3 delta(posA);
	size += s2.radius;
	float sizesqr(size);
	sizesqr *= size;
	delta -= posB;
	sizesqr *= 4;
	float dist = delta.sqrMagnitude();
	
	
	if(dist < sizesqr)
	{
		if(dist == 0) 	{
			a += Vector3(static_cast<float>(rand()), static_cast<float>(rand()), static_cast<float>(rand())).normalized();
            return;
		}
		
		dist = size * Q_rsqrt(dist) - 0.5f;
		delta *= dist;
		posA += delta;
		a = posA - s1.offset;
		posB -= delta;
		b = posB - s2.offset;
	}
}

inline bool checkAABBtoAABB(const AABB box1, const AABB box2)
{	
	if(box1.max.x < box2.min.x || box1.min.x > box2.max.x) { return 0; }
	if(box1.max.y < box2.min.y || box1.min.y > box2.max.y) { return 0; }
	if(box1.max.z < box2.min.z || box1.min.z > box2.max.z) { return 0; }
	
	return 1;
};


inline void simulatePhysicStep(Transform* transform, SphereCollision* collisions, AABB* aabb, int size)
{
	for (int i = 0; i < size; ++i) {
        float r = collisions[aabb[i].index].radius * 2;
        Vector3 pos = transform[aabb[i].index].position;
        aabb[i] = {pos - Vector3(r, r, r), pos + Vector3(r, r, r), aabb[i].index};
    }

    // Sort AABBs based on the X axis
    std::sort(aabb, aabb + size, [](const AABB a, const AABB b) {
        return a.min.x < b.min.x;
    });

    // Detect collisions
    for (int i = 0; i < size; ++i) {
        AABB objA = aabb[i];
        for (int j = i + 1; j < size; ++j) {
            AABB objB = aabb[j];
            if (objB.min.x > objA.max.x) break; // Early exit

			if(objA.max.y < objB.min.y || objA.min.y > objB.max.y) { continue; }
			if(objA.max.z < objB.min.z || objA.min.z > objB.max.z) { continue; }
			{
                solveSphereToSphere(
                    transform[objA.index].position, 
                    transform[objB.index].position, 
                    collisions[objA.index], 
                    collisions[objB.index]
                );
            }
		}	
		transform[i].position += Vector3(0, -0.003, 0);
		//transform[i].position.x = clamp(-10, 10, transform[i].position.x);
		//transform[i].position.y = clamp(  0, 10, transform[i].position.y);
		//transform[i].position.z = clamp(-10, 10, transform[i].position.z);
	}
}

inline void simulateSlowestPhysicStep(Transform* transform, SphereCollision* collisions, AABB* aabb, int size)
{
	for(int i = 0 ; i < size ; i++)
	{
		for(int j = i + 1 ; j < size; j++)
		{
			//if(checkAABBtoAABB(transform[i].position, transform[j].position, aabb[i], aabb[j]))
			{
				solveSphereToSphere(transform[i].position, transform[j].position, collisions[i], collisions[j]);
			}
		}
		transform[i].position += Vector3(0, -0.003, 0);
		transform[i].position.x = clamp(-5, 5, transform[i].position.x);
		transform[i].position.y = clamp(  0, 10, transform[i].position.y);
		transform[i].position.z = clamp(-5, 5, transform[i].position.z);
	}
}

inline void calculatePhysic(Transform* transform, SphereCollision* collisions, AABB* aabbs, int size)
{
    //for(int i = 0; i < 8; i++)
	{
		simulatePhysicStep(transform, collisions, aabbs, size);
		//simulateSlowestPhysicStep(transform, collisions, aabbs, size);
	}   
}

