#include "Game.h"


Game::Game(sf::RenderWindow* window)
{
	_window = window;
}


Game::~Game()
{
	delete player;
	delete opponent;
}

void Game::InitializeGame()
{
	player = new Player();
	//wait for server to confirm opponent
	opponent = new Player();

}

void Game::Update(float dt)
{
	UpdateInput(dt);

	player->Update(dt);
	opponent->Update(dt);
	// UpdateNetwork
}

void Game::Draw()
{
	player->Draw(_window);
}

void Game::UpdateInput(float dt)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		player->Move(-dt);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		player->Move(dt);
}