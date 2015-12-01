#include "Ball.h"
#define PI 3.1415

Ball::Ball()
{
	shape.setSize(sf::Vector2f(4, 4));
	shape.setOrigin(2, 2);

	angle = 0;
	speed = 0;
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
	shape.move(cos(angle * PI / 180.0) * speed * dt,
		sin(angle * PI / 180.0) * speed * dt);

	if (shape.getPosition().x < 2.0f)
	{
		shape.setPosition(2.0f, shape.getPosition().y);
		CollisionByNormal(0.0f);
	}
	if (shape.getPosition().x > 248.0f)
	{
		shape.setPosition(248.0f, shape.getPosition().y);
		CollisionByNormal(180.0f);
	}
	if (shape.getPosition().y < 2.0f)
	{
		shape.setPosition(shape.getPosition().x, 2.0f);
		CollisionByNormal(90.0f);
	}
	if (shape.getPosition().y > 498.0f)
	{
		shape.setPosition(shape.getPosition().x, 498.0f);
		CollisionByNormal(270.0f);
	}
}

void Ball::SetPosition(float x, float y)
{
	shape.setPosition(x, y);
}

void Ball::SetSpeed(float newAngle, float newSpeed)
{
	angle = newAngle;
	speed = newSpeed;
}

sf::RectangleShape Ball::GetShape()
{
	return shape;
}

void Ball::CollisionByNormal(float normalAngle)
{
	angle += 180;

	angle -= normalAngle;
	angle = normalAngle - angle;
}