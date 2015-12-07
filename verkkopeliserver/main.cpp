#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML\System.hpp>

bool gameIsRunning = false;
Game* game;

void StartGame()
{
	game = new Game();
	game->InitializeGame();
	gameIsRunning = true;
}

void KillGame()
{
	delete game;
	gameIsRunning = false;
	sf::Clock killer;
	killer.restart();

	while (true)
	{
		if (killer.getElapsedTime().asSeconds() > 5.0)
			return;
	}
}

int main()
{
	sf::Clock clock;

	while (true)
	{
		if (!gameIsRunning)
		{
			StartGame();
		}

		if (!game->Update((float)clock.restart().asSeconds()))
		{
			KillGame();
		}
	}

	return 0;
}