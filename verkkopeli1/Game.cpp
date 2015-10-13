#include "Game.h"


Game::Game(sf::RenderWindow* window)
{
	_window = window;
}


Game::~Game()
{
	delete player;
	delete opponent;
	delete ball;
}

void Game::InitializeGame()
{
	player = new Player();
	player->SetYposition(450);
	WaitForNetwork();
	
	opponent = new Player();
	opponent->SetYposition(50);

	ball = new Ball();
}

void Game::WaitForNetwork()
{
	KiekkoNetwork::GetInstance();
}

int Game::Update(float dt)
{
	//networking
	if (KiekkoNetwork::GetInstance()->newPackage)
	{
		KiekkoNetwork::ReceivePackage tempRecv;
		tempRecv = KiekkoNetwork::GetInstance()->GetLatestPackage();

		player->SetPosition(tempRecv.playerPos);
		opponent->SetPosition(tempRecv.enemyPos);

		ball->SetPosition(tempRecv.ballX, tempRecv.ballY);
		ball->SetSpeed(tempRecv.ballXVel, tempRecv.ballYVel);
	}

	UpdateInput(dt);

	player->Update(dt);
	opponent->Update(dt);
	ball->Update(dt);

	CheckCollision();

	KiekkoNetwork::SendPackage tempSend;
	tempSend.ownPos = player->GetShape().getPosition().x;

	if (KiekkoNetwork::GetInstance()->SendMsg(tempSend))
		return 1;
	
	return 0;
}

void Game::CheckCollision()
{
	if (player->GetShape().getGlobalBounds().intersects(ball->GetShape().getGlobalBounds()))
	{
		ball->MirrorY();
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
