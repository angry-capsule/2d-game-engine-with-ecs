#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include <string>

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"

class MovementSystem : public System
{
private:
	void OnCollision(CollisionEvent& event)
	{
		Entity a = event.a;
		Entity b = event.b;

		Logger::Log("Collision between " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()) + "!");

		if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles"))
		{
			OnEnemyHitsObstacle(a, b);
		}

		if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies"))
		{
			OnEnemyHitsObstacle(b, a);
		}
	}

	void OnEnemyHitsObstacle(Entity enemy, Entity obstacle)
	{
		if (enemy.HasComponent<RigidbodyComponent>() && enemy.HasComponent<SpriteComponent>())
		{
			auto& rigidbody = enemy.GetComponent<RigidbodyComponent>();
			auto& sprite = enemy.GetComponent<SpriteComponent>();

			if (rigidbody.velocity.x != 0)
			{
				rigidbody.velocity.x *= -1;
				sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			}

			if (rigidbody.velocity.y != 0)
			{
				rigidbody.velocity.y *= -1;
				sprite.flip = sprite.flip == SDL_FLIP_NONE ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
			}
		}
	}

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

			if (entity.HasTag("player"))
			{
				const int paddingLeft = 10;
				const int paddingTop = 10;
				const int paddingRight = 50;
				const int paddingDown = 50;

				transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
				transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
				transform.position.y = transform.position.y < paddingTop ? paddingTop : transform.position.y;
				transform.position.y = transform.position.y > Game::mapHeight - paddingDown ? Game::mapHeight - paddingDown : transform.position.y;
			}

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

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
	}
};

#endif