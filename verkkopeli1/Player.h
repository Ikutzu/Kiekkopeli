#pragma once
#include <SFML\Graphics.hpp>


class Player
{

public:
	Player();
	~Player();
	void Draw(sf::RenderWindow* win);
	void Update(float dt);
	void Move(float multiplier);
	sf::RectangleShape GetShape();

private:
	float speed;
	sf::RectangleShape shape;
};

