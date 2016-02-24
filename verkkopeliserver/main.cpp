#include "Game.h"

#include <SFML/System.hpp>

bool gameIsRunning = false;
Game* game = 0;

void StartGame()
{
	game = new Game();
	gameIsRunning = game->InitializeGame();
}

void KillGame()
{
	if(game != 0)
		delete game;

	gameIsRunning = false;
	printf("sleeping\n");
	sf::sleep(sf::seconds(5.0f));
	printf("waking up\n");
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
