#include "Game.h"


Game::~Game()
{
	delete player1;
	delete player2;
	delete ball;
}

void Game::InitializeGame()
{
	WaitForNetwork();

	player1 = new Player();
	player1->SetYposition(450);
	player2 = new Player();
	player2->SetYposition(50);

	ball = new Ball();
}

void Game::WaitForNetwork()
{
	KiekkoNetwork::GetInstance();
	while (WaitForConnections())
	{ }
	printf("Game Running...");
}

int Game::WaitForConnections()
{
	if (threadCount < 2)
	{
		KiekkoNetwork::GetInstance()->Update(threadCount);
		threadCount++;
		return 1;
	}

	return 0;
}

int Game::Update(float dt)
{
	//networking
	if (KiekkoNetwork::GetInstance()->newPackage)
	{
		KiekkoNetwork::ReceivePackage temp = KiekkoNetwork::GetInstance()->GetLatestPackage();
				
		player1->SetPosition(temp.player1Pos);
		player2->SetPosition(temp.player2Pos);
	}

	ball->Update(dt);

	CheckCollision();
	
	networkTimer += dt;
	if (networkTimer >= 0.2)
	{
		networkTimer = 0.0;
		KiekkoNetwork::SendPackage temp;
		temp.player1Pos = player1->GetShape().getPosition().x;
		temp.player2Pos = player2->GetShape().getPosition().x;
		temp.ballX = ball->GetShape().getPosition().x;
		temp.ballY = ball->GetShape().getPosition().y;
		temp.ballAngle = ball->angle;
		temp.ballVelocity = ball->speed;

		if (KiekkoNetwork::GetInstance()->SendMsg(temp))
			return 0;
	}
	
	return 1;
}

void Game::CheckCollision()
{
	//player1
	if (player1->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->CollisionByNormal(270.0f);
	}
	//player2
	if (player2->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->CollisionByNormal(90.0f);
	}
}