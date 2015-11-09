#pragma once
#include "Player.h"
#include "Ball.h"
#include "KiekkoNetwork.h"
#include <SFML\Graphics.hpp>

class Game
{
public:
	Game(sf::RenderWindow* window);
	~Game();
	void InitializeGame();
	void WaitForNetwork();
	int Update(float dt);
	void UpdateInput(float dt);
	void Draw();

private:

	double networkTimer;
	sf::RenderWindow* _window;
	Player* player;
	Player* opponent;
	Ball* ball;
	
};

