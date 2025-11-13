#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

class CollisionSystem : public System
{
	CollisionSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update()
	{
		for (auto entity1 : GetSystemEntities())
		{
			auto transform1   = entity1.GetComponent<TransformComponent>();
			auto boxCollider1 = entity1.GetComponent<BoxColliderComponent>();

			float colliderOneSideHalfLengthX1 = boxCollider1.width / 2.0f;
			float colliderOneSideHalfLengthY1 = boxCollider1.height / 2.0f;

			float left1   = transform1.position.x - colliderOneSideHalfLengthX1 + boxCollider1.offset.x;
			float right1  = transform1.position.x + colliderOneSideHalfLengthX1 + boxCollider1.offset.x;
			float top1    = transform1.position.y + colliderOneSideHalfLengthY1 + boxCollider1.offset.y;
			float bottom1 = transform1.position.y - colliderOneSideHalfLengthY1 + boxCollider1.offset.y;

			for (auto entity2 : GetSystemEntities())
			{
				auto transform2   = entity2.GetComponent<TransformComponent>();
				auto boxCollider2 = entity2.GetComponent<BoxColliderComponent>();

				float colliderOneSideHalfLengthX2 = boxCollider2.width / 2.0f;
				float colliderOneSideHalfLengthY2 = boxCollider2.height / 2.0f;

				float left2   = transform2.position.x - colliderOneSideHalfLengthX2 + boxCollider2.offset.x;
				float right2  = transform2.position.x + colliderOneSideHalfLengthX2 + boxCollider2.offset.x;
				float top2    = transform2.position.y + colliderOneSideHalfLengthY2 + boxCollider2.offset.y;
				float bottom2 = transform2.position.y - colliderOneSideHalfLengthY2 + boxCollider2.offset.y;

				// TODO::Check AABB Collision
			}
		}
	}
};

#endif