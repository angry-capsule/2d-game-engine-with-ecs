#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H

#include <string>
#include "SDL_keyboard.h"
#include "../ECS/ECS.h"
#include "../Events/KeyPressedEvent.h"
#include "../EventBus/EventBus.h"

#include "../Components/KeyboardControlComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"

class KeyboardControlSystem : public System
{
private:
    void OnKeyPressed(KeyPressedEvent& event)
    {
        Logger::Log("Key was pressed: " + std::string(SDL_GetKeyName(event.keyCode)));

        for (auto entity : GetSystemEntities())
        {
            const auto keyboardControl = entity.GetComponent<KeyboardControlComponent>();
            auto& sprite = entity.GetComponent<SpriteComponent>();
            auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

            switch (event.keyCode)
            {
            case SDLK_UP:
                rigidbody.velocity = keyboardControl.upVelocity;
                sprite.srcRect.y = sprite.height * 0;
                break;
            case SDLK_RIGHT:
                rigidbody.velocity = keyboardControl.rightVelocity;
                sprite.srcRect.y = sprite.height * 1;
                break;
            case SDLK_DOWN:
                rigidbody.velocity = keyboardControl.downVelocity;
                sprite.srcRect.y = sprite.height * 2;
                break;
            case SDLK_LEFT:
                rigidbody.velocity = keyboardControl.leftVelocity;
                sprite.srcRect.y = sprite.height * 3;
                break;
            }
        }
    }

public:
    KeyboardControlSystem() 
    {
        RequireComponent<KeyboardControlComponent>();
        RequireComponent<RigidbodyComponent>();
        RequireComponent<SpriteComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void Update()
    {

    }
};

#endif