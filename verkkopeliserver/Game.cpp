#include "Game.h"


Game::Game()
: collisionHappening(false)
, networkTimer(0.0)
, threadCount(0)
, networkInstance(0)
{}

Game::~Game()
{
	if(networkInstance != 0)
		networkInstance->CloseConnections();

	delete player1;
	delete player2;
	delete ball;
}

int Game::InitializeGame()
{
	WaitForNetwork();

	player1 = new Player();
	player1->SetYposition(450);
	player2 = new Player();
	player2->SetYposition(50);

	ball = new Ball();

	return 1;
}

void Game::WaitForNetwork()
{
	printf("Waiting for network...\n");

	while(networkInstance == 0)
		networkInstance = KiekkoNetwork::GetInstance();

	printf("KiekkoNetwork initialized\nWaiting for players...\n");
	while (WaitForConnections())
	{}
	printf("Game Running...\n");
}

int Game::WaitForConnections()
{
	if (threadCount < 2)
	{
		if(networkInstance->Update(threadCount))
			threadCount++;
		return 1;
	}

	return 0;
}

int Game::Update(float dt)
{
	//networking
	if (KiekkoNetwork::newPackage)
	{
		KiekkoNetwork::ReceivePackage temp = networkInstance->GetLatestPackage();
				
		player1->SetPosition(temp.player1Pos);
		player2->SetPosition(temp.player2Pos);
	}
	
	ball->Update(dt);

	ball->speed += 1*dt;
	CheckCollision();
	
	networkTimer += dt;
	if (networkTimer >= 0.1)
	{
		networkTimer = 0.0;
		KiekkoNetwork::SendPackage temp;
		temp.player1Pos = player1->GetShape().getPosition().x;
		temp.player2Pos = player2->GetShape().getPosition().x;
		temp.ballX = ball->GetShape().getPosition().x;
		temp.ballY = ball->GetShape().getPosition().y;
		temp.ballAngle = ball->angle;
		temp.ballVelocity = ball->speed;

		if (networkInstance->SendMsg(temp))
			return 0;
	}

	return 1;
}


void Game::CheckCollision()
{
	if (!collisionHappening)
	{
		//player1
		if (player1->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
		{
			collisionHappening = true;
			ball->CollisionByNormal(270.0f);
		}
		//player2
		if (player2->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
		{
			collisionHappening = true;
			ball->CollisionByNormal(90.0f);
		}
	}
	else
	{
		if (!player1->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()) && !player2->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
			collisionHappening = false;
	}
}
