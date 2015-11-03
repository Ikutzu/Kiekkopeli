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
	player2 = new Player();

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
	if (KiekkoNetwork::GetInstance()->activeSocket.size() < 2)
	{
		KiekkoNetwork::GetInstance()->Update();
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
	
	KiekkoNetwork::SendPackage temp;
	temp.player1Pos = player1->GetShape().getPosition().x;
	temp.player2Pos = player2->GetShape().getPosition().x;
	temp.ballX = ball->GetShape().getPosition().x;
	temp.ballY = ball->GetShape().getPosition().y;
	temp.ballXVel = ball->spdX;
	temp.ballYVel = ball->spdY;

	printf("Sending msg");
	if (KiekkoNetwork::GetInstance()->SendMsg(temp))
		return 0;
	
	return 1;
}

void Game::CheckCollision()
{
	//player1
	if (player1->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->MirrorY();
	}
	if (player1->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->MirrorY();
	}
	//player2
	if (player2->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->MirrorY();
	}
	if (player2->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->MirrorY();
	}
}