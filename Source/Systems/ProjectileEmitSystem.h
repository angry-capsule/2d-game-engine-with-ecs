#ifndef PROJECTILEEMITSYSTEM_H
#define PROJECTILEEMITSYSTEM_H

#include "SDL.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"


class ProjectileEmitSystem : public System
{
public:
    ProjectileEmitSystem()
    {
        RequireComponent<ProjectileEmitterComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(std::unique_ptr<Registry>& registry)
    {
        for (auto entity : GetSystemEntities())
        {
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
            const auto transform = entity.GetComponent<TransformComponent>();

            if ((SDL_GetTicks() - projectileEmitter.lastEmissionTime) > projectileEmitter.repeatFrequency)
            {
                glm::vec2 projectilePosition = transform.position;

                if (entity.HasComponent<SpriteComponent>())
                {
                    const auto sprite = entity.GetComponent<SpriteComponent>();
                    projectilePosition.x += (transform.scale.x * sprite.width / 2);
                    projectilePosition.y += (transform.scale.y * sprite.height / 2);
                }

                Entity projectile = registry->CreateEntity();
                projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0f, 1.0f), 0);
                projectile.AddComponent<RigidbodyComponent>(projectileEmitter.projectileVelocity);
                projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
                projectile.AddComponent<BoxColliderComponent>(4, 4, glm::vec2(0.0f, 0.0f));
                projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);

                projectileEmitter.lastEmissionTime = SDL_GetTicks();
            }
        }
    }
};

#endif