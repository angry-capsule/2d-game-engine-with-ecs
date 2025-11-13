#include "Game.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

#include "glm/glm.hpp"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Services/AssetProvider.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

static std::string GetAssetPath(const std::string& rel);

Game::Game() : isRunning(false), millisecondsPreviousFrame(0)
{
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
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
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	//windowWidth = displayMode.w;
	//windowHeight = displayMode.h;
	windowWidth = 800;
	windowHeight = 600;
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
	isRunning = true;
}

void Game::Setup() 
{
	LoadLevel(1);
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
		switch (sdlEvent.type) 
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) 
			{
				isRunning = false;
			}
			break;
		}
	}
}

void Game::LoadLevel(int level)
{
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();

	assetStore->AddTexture(renderer, "tank-image", "Assets/Images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "Assets/Images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "Assets/Images/chopper.png");
	assetStore->AddTexture(renderer, "radar-image", "Assets/Images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "Assets/Tilemaps/jungle.png");

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
			tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	Entity chopper = registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 15, true);

	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidbodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
	radar.AddComponent<AnimationComponent>(8, 5, true);

	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidbodyComponent>(glm::vec2(30.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 1);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidbodyComponent>(glm::vec2(20.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 2);
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

	registry->Update();

	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
}

//void Game::Render()
//{
//	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
//	SDL_RenderClear(renderer);
//
//	// const std::string assetPath = GetAssetPath("Assets/Images/tank-tiger-right.png");
//
//	/*
//	SDL_Surface* surface = IMG_Load(assetPath.c_str());
//	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//	SDL_FreeSurface(surface);
//
//	SDL_Rect dstRect = { 
//		10, 
//		20, 
//		32, 
//		32 };
//
//	SDL_RenderCopy(renderer, texture, NULL, &dstRect);
//
//	SDL_DestroyTexture(texture);
//	*/
//
//	registry->GetSystem<RenderSystem>().Update(renderer);
//
//	SDL_RenderPresent(renderer);
//}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}