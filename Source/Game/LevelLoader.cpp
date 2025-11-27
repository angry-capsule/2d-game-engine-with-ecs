#include "LevelLoader.h"

#include <memory>
#include <fstream>
#include <string>

#include "SDL.h"
#include "sol/sol.hpp"

#include "Game.h"
#include "../Services/AssetProvider.h"
#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/BoxColliderComponent.h"

LevelLoader::LevelLoader()
{

}

LevelLoader::~LevelLoader()
{

}

void LevelLoader::LoadLevel(sol::state& lua, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int levelId)
{
    std::string pathToLevel = AssetProvider::GetAssetPath("Assets/Scripts/Level" + std::to_string(levelId) + ".lua");

    sol::load_result scriptLoadingResult = lua.load_file(pathToLevel);

    if (!scriptLoadingResult.valid())
    {
        sol::error err = scriptLoadingResult;
        Logger::Err("Problem with loading script at path: " + pathToLevel + "\nWith error: " + err.what());
        return;
    }

    lua.script_file(pathToLevel);

    sol::table level = lua["Level"];

    sol::table assets = level["assets"];

    int i = 0;
    while (true)
    {
        sol::optional<sol::table> hasAsset = assets[i];

        if (hasAsset == sol::nullopt)
        {
            break;
        }

        sol::table asset = assets[i];

        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
        std::string assetPath = asset["file"];

        if (assetType == "texture")
        {
            assetStore->AddTexture(renderer, assetId, assetPath);
            Logger::Log("Texture added with id: " + assetId + " at path: " + assetPath);
        }
        if (assetType == "font")
        {
            assetStore->AddFont(assetId, assetPath, asset["font_size"]);
            Logger::Log("Font added with id: " + assetId + " at path: " + assetPath);
        }

        i++;
    }

    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open(AssetProvider::GetAssetPath("Assets/Tilemaps/jungle.map"));

    for (int y = 0; y < mapNumRows; y++)
    {
        for (int x = 0; x < mapNumCols; x++)
        {
            char ch;
            mapFile.get(ch);
            Logger::Log(std::string("Read character: ") + ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                                                  glm::vec2(tileScale, tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();
    Game::mapWidth = mapNumCols * tileSize * tileScale;
    Game::mapHeight = mapNumRows * tileSize * tileScale;

    Entity chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
    chopper.AddComponent<AnimationComponent>(2, 15, true);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0.0f, -200.0f), glm::vec2(200.0f, 0.0f), glm::vec2(0.0f, 200.0f), glm::vec2(-200.0f, 0.0f));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0f, 150.0f), 0, 10000, 10, true);

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(Game::windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(500.0, 500.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(40.0, 0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0, 0));
    // tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0f, 0.0f), 5000, 3000, 10, false);
    tank.AddComponent<HealthComponent>(100);

    Entity truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0, 0));
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0f, 100.0f), 2000, 5000, 10, false);
    truck.AddComponent<HealthComponent>(100);

    Entity treeA = registry->CreateEntity();
    treeA.Group("obstacles");
    treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 495.0), glm::vec2(1.0, 1.0), 0.0);
    treeA.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    treeA.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0, 0));

    Entity treeB = registry->CreateEntity();
    treeB.Group("obstacles");
    treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 495.0), glm::vec2(1.0, 1.0), 0.0);
    treeB.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
    treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
    treeB.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0, 0));
}
