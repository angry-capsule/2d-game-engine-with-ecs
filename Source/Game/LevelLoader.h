#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "SDL.h"
#include <memory>

class LevelLoader
{
public:
    LevelLoader();
    ~LevelLoader();

    void LoadLevel(std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int level);
};


#endif