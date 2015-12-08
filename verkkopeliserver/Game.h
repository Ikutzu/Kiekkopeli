#pragma once
#include "Player.h"
#include "Ball.h"
#include "KiekkoNetwork.h"
#include <SFML\Graphics.hpp>

class Game
{
public:
	Game() : networkTimer(0.0), threadCount(0), collisionHappening(false){};
	~Game();
	void InitializeGame();
	void WaitForNetwork();
	int WaitForConnections();
	int Update(float dt);
	void Interpolate(float dt);
	void CheckCollision();

private:
	bool collisionHappening;

	double networkTimer;
	int threadCount;

	Player* player1;
	Player* player2;
	Ball* ball;
	
};

