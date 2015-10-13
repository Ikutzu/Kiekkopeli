#pragma once
#include <SFML\Graphics.hpp>

class Ball
{

public:
	Ball();
	~Ball();
	void Draw(sf::RenderWindow* win);
	void Update(float dt);
	void SetPosition(float x, float y);
	void SetSpeed(float speedX, float speedY);
	sf::RectangleShape GetShape();
	void MirrorX();
	void MirrorY();

private:

	float spdX;
	float spdY;
	sf::RectangleShape shape;
};