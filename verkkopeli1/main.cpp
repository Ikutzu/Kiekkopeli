#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML\System.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(250, 500), "Verkkopeli");
	Game game(&window);
	game.InitializeGame();

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (game.Update((float)clock.restart().asSeconds()))
		{
			printf("Shutting down...");
			Sleep(1000);
			window.close();
		}


		window.clear();
		game.Draw();
		window.display();
	}

	return 0;
}