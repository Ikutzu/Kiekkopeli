#pragma once
#include "Player.h"
#include <SFML\Graphics.hpp>

class Game
{
public:
	Game(sf::RenderWindow* window);
	~Game();
	void InitializeGame();
	void Update(float dt);
	void UpdateInput(float dt);
	void Draw();

private:

	sf::RenderWindow* _window;
	Player* player;
	Player* opponent;
	
};

