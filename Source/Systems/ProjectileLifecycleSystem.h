#ifndef PROJECTILELIFECYCLESYSTEM_H
#define PROJECTILELIFECYCLESYSTEM_H

#include "SDL.h"
#include "../Components/ProjectileComponent.h"
#include "../ECS/ECS.h"

class ProjectileLifecycleSystem : public System
{
public:
    ProjectileLifecycleSystem()
    {
        RequireComponent<ProjectileComponent>();
    };

    void Update()
    {
        for (auto entity : GetSystemEntities())
        {
            const auto projectile = entity.GetComponent<ProjectileComponent>();

            if (SDL_GetTicks() - projectile.startTime > projectile.duration)
            {
                entity.Kill();
            }
        }
    }
};

#endif