#ifndef GAME_H
#define GAME_H

#include <memory>
#include "SDL.h"
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"


const int FPS = 144;
const int MILLISECONDS_PER_FRAME = 1000 / FPS;

class Game
{
private:
	bool isRunning;
	bool isDebug;
	int millisecondsPreviousFrame;
	
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;

public:
	int windowWidth;
	int windowHeight;

	Game();
	~Game();

	void Initialize();
	void LoadLevel(int level);
	void Setup();
	void Run();
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();
};

#endif