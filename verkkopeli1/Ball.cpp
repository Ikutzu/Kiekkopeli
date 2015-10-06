#include "Ball.h"


Ball::Ball()
{
	shape.setSize(sf::Vector2f(4, 4));
	shape.setOrigin(2, 2);

	spdX = 0;
	spdY = 0;
}

Ball::~Ball()
{
}

void Ball::Draw(sf::RenderWindow* win)
{
	win->draw(shape);
}

void Ball::Update(float dt)
{
	shape.move((spdX*dt), (spdY*dt));

	if (shape.getPosition().x < 2.0f)
		shape.setPosition(2.0f, shape.getPosition().y);
	if (shape.getPosition().x > 248.0f)
		shape.setPosition(248.0f, shape.getPosition().y);
	if (shape.getPosition().y < 2.0f)
		shape.setPosition(shape.getPosition().x, 2.0f);
	if (shape.getPosition().y > 498.0f)
		shape.setPosition(shape.getPosition().x, 498.0f);
}

void Ball::SetPosition(float x, float y)
{
	shape.setPosition(x, y);
}

void Ball::SetSpeed(float speedX, float speedY)
{
	spdX = speedX;
	spdY = speedY;
}