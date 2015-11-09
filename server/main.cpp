#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML\System.hpp>

int main()
{
	Game game;
	game.InitializeGame();

	sf::Clock clock;

	while (game.Update((float)clock.restart().asSeconds()))
	{
	}

	return 0;
}