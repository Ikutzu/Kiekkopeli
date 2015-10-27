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

	KiekkoNetwork::ReceivePackage tempRecv;
	tempRecv = KiekkoNetwork::GetInstance()->GetLatestPackage();
	while (true)
		if (tempRecv.ballX != 0)
			break;
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

	UpdateInput(dt);

	player->Update(dt);
	opponent->Update(dt);
	ball->Update(dt);


	KiekkoNetwork::SendPackage tempSend;
	tempSend.ownPos = player->GetShape().getPosition().x;

	if (KiekkoNetwork::GetInstance()->SendMsg(tempSend))
		return 1;
	
	return 0;
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
