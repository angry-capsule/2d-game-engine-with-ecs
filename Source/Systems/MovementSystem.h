#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include <string>

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidbodyComponent>();
	}

	void Update(float deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto rigidbody = entity.GetComponent<RigidbodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			bool isEntityOutsideMap = transform.position.x < 0
										|| transform.position.x > Game::mapWidth
										|| transform.position.y < 0
										|| transform.position.y > Game::mapHeight;

			if (isEntityOutsideMap && !entity.HasTag("player"))
			{
				entity.Kill();
			}
		}
	}
};

#endif