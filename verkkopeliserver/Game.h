#pragma once
#include "Player.h"
#include "Ball.h"
#include "KiekkoNetwork.h"
#include <SFML/Graphics/RectangleShape.hpp>

class Game
{
public:
	Game();
	~Game();
	int InitializeGame();
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
	KiekkoNetwork* networkInstance;
};

