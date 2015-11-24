#pragma once
#include "Player.h"
#include "Ball.h"
#include "KiekkoNetwork.h"
#include <SFML\Graphics.hpp>

class Game
{
public:
	Game() : networkTimer(0.0), threadCount(0){};
	~Game();
	void InitializeGame();
	void WaitForNetwork();
	int WaitForConnections();
	int Update(float dt);
	void CheckCollision();

private:

	double networkTimer;
	int threadCount;

	Player* player1;
	Player* player2;
	Ball* ball;
	
};

