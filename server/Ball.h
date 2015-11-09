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
	void SetSpeed(int velocity);
	sf::RectangleShape GetShape();
	void CollisionWith(float collisionAngle);
	float angle;
	int velocity;

private:

	sf::RectangleShape shape;
};