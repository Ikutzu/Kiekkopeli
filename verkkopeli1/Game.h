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
	void CheckCollision();
	void UpdateInput(float dt);
	void Draw();
	void Extrapolate(float dt);

private:

	double networkTimer;
	double updateTimer;
	double updateMinusOne;

	float position;
	float positionMinusOne;

	bool first;

	sf::RenderWindow* _window;
	Player* player;
	Player* opponent;
	Ball* ball;
	
};

