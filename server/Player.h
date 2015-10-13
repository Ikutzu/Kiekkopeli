#pragma once
#include <SFML\Graphics.hpp>


class Player
{

public:
	Player();
	~Player();
	void Update(float dt);
	void SetPosition(float position);
	void SetYposition(float Yposition);
	sf::RectangleShape GetShape();

private:

	int socket;
	sf::RectangleShape shape;
};

