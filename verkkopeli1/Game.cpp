#include "Game.h"
#include <iostream>


Game::Game(sf::RenderWindow* window): 
	_window(window), 
	networkTimer(0.0)
{}

Game::~Game()
{
	delete player;
	delete opponent;
	delete ball;
}

void Game::InitializeGame()
{
	WaitForNetwork();

	player = new Player();
	player->SetYposition(450);
		
	opponent = new Player();
	opponent->SetYposition(50);

	ball = new Ball();
}

void Game::WaitForNetwork()
{
	KiekkoNetwork::ReceivePackage tempRecv;
	tempRecv = KiekkoNetwork::GetInstance()->GetLatestPackage();
	int sucksession = 1;
	while (sucksession)
	{
		std::string temp;
		std::cin >> temp;
		KiekkoNetwork::GetInstance()->SERVER = temp;
		sucksession = KiekkoNetwork::GetInstance()->InitializeNetwork();
	}
	while (true)
	{
		tempRecv = KiekkoNetwork::GetInstance()->GetLatestPackage();

		if (tempRecv.ballX != 0)
			break;
	}
}

int Game::Update(float dt)
{

	//networking
	if (KiekkoNetwork::GetInstance()->newPackage)
	{
		KiekkoNetwork::ReceivePackage tempRecv;
		tempRecv = KiekkoNetwork::GetInstance()->GetLatestPackage();
		
		opponent->SetPosition(tempRecv.enemyPos);
		
		ball->SetPosition(tempRecv.ballX, tempRecv.ballY);
		ball->SetSpeed(tempRecv.ballXVel, tempRecv.ballYVel);
	}

	if (_window->hasFocus())
		UpdateInput(dt);

	player->Update(dt);
	opponent->Update(dt);
	ball->Update(dt);
	CheckCollision();

	networkTimer += dt;
	if (networkTimer >= 0.2)
	{
		networkTimer = 0.0;
		KiekkoNetwork::SendPackage tempSend;
		tempSend.ownPos = player->GetShape().getPosition().x;
	
		if (KiekkoNetwork::GetInstance()->SendMsg(tempSend))
			return 1;
	}
	
	return 0;
}

void Game::CheckCollision()
{
	//player1
	if (player->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->CollisionByNormal(270.0f);
	}
	//player2
	if (opponent->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->CollisionByNormal(90.0f);
	}
}

void Game::UpdateInput(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		player->Move(-dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		player->Move(dt);
}

void Game::Draw()
{
	player->Draw(_window);
	opponent->Draw(_window);
	ball->Draw(_window);
}
