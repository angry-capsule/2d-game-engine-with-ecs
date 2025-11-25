#include "Game.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "glm/glm.hpp"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "imgui.h"
#include "imgui_sdl.h"
#include "imgui_impl_sdl.h"

#include "LevelLoader.h"

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Services/AssetProvider.h"
#include "../EventBus/EventBus.h"

#include "../Events/KeyPressedEvent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/RenderGUISystem.h"



int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() : isRunning(false), isDebug(false), millisecondsPreviousFrame(0)
{
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();

    Logger::Log("Game constructor called!");
}

Game::~Game()
{
    Logger::Log("Game destructor called!");
}

void Game::Initialize()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Err("Error initializing SDL.");
        return;
    }

    if (TTF_Init() != 0)
    {
        Logger::Err("Error initializing SDL TTF.");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    //windowWidth = displayMode.w;
    //windowHeight = displayMode.h;
    windowWidth = 1920;
    windowHeight = 1080;
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS
    );
    if (!window)
    {
        Logger::Err("Error creating SDL window.");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        Logger::Err("Error creating SDL renderer.");
        return;
    }
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

    camera.x = 0;
    camera.y = 0;
    camera.w = windowWidth;
    camera.h = windowHeight;

    isRunning = true;
}

void Game::Setup()
{
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<ProjectileEmitSystem>();
    registry->AddSystem<ProjectileLifecycleSystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<RenderHealthBarSystem>();
    registry->AddSystem<RenderGUISystem>();

    LevelLoader loader;
    loader.LoadLevel(registry, assetStore, renderer, 1);
}

void Game::Run()
{
    Setup();

    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();

        int mouseX;
        int mouseY;
        const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
        io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);

                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_d)
                {
                    isDebug = !isDebug;
                }
                break;
        }
    }
}

void Game::Update()
{
    int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - millisecondsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECONDS_PER_FRAME)
    {
        SDL_Delay(timeToWait);
    }

    double deltaTime = (SDL_GetTicks() - millisecondsPreviousFrame) / 1000.0;

    millisecondsPreviousFrame = SDL_GetTicks();

    eventBus->Reset();

    registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

    registry->Update();

    registry->GetSystem<MovementSystem>().Update(deltaTime);
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<CameraMovementSystem>().Update(camera);
    registry->GetSystem<ProjectileEmitSystem>().Update(registry);
    registry->GetSystem<ProjectileLifecycleSystem>().Update();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
    registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
    registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);

    if (isDebug)
    {
        registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
        registry->GetSystem<RenderGUISystem>().Update(registry, camera);
    }

    SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
