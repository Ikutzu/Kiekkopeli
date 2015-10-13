#include "Game.h"


Game::~Game()
{
	delete player1;
	delete player2;
	delete ball;
}

void Game::InitializeGame()
{
	player1 = new Player();
	player1->SetYposition(450);
	WaitForNetwork();
	
	player2 = new Player();
	player2->SetYposition(50);

	ball = new Ball();
}

void Game::WaitForNetwork()
{
	KiekkoNetwork::GetInstance();
}

int Game::WaitForConnections()
{
	
	return 1;

	return 0;
}

int Game::Update(float dt)
{
	//networking
	

	ball->Update(dt);

	CheckCollision();

	if (KiekkoNetwork::GetInstance()->SendMsg())
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