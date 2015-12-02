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
	void SetPosition(float position);
	void SetYposition(float Yposition);
	sf::RectangleShape GetShape();
	float speed;

private:

	float yPosition;
	sf::RectangleShape shape;
};

