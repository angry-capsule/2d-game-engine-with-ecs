#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H

#include <string>
#include "SDL_keyboard.h"
#include "../ECS/ECS.h"
#include "../Events/KeyPressedEvent.h"
#include "../EventBus/EventBus.h"

class KeyboardControlSystem : public System
{
private:
    void OnKeyPressed(KeyPressedEvent& event)
    {
        Logger::Log("Key was pressed: " + std::string(SDL_GetKeyName(event.keyCode)));
    }

public:
    KeyboardControlSystem() { }

    void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
    {
        eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void Update()
    {

    }
};

#endif