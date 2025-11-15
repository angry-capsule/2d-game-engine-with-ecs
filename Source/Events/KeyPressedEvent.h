#ifndef KEYPRESSEDEVENT_H
#define KEYPRESSEDEVENT_H

#include "SDL.h"
#include "../EventBus/Event.h"

class KeyPressedEvent : public Event
{
public:
    SDL_Keycode keyCode;

    KeyPressedEvent(SDL_Keycode keyCode) : keyCode(keyCode) { }
};

#endif