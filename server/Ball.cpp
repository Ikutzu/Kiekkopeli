#include "Ball.h"
#define PI 3.14159 / 180

Ball::Ball()
{
	shape.setSize(sf::Vector2f(4, 4));
	shape.setOrigin(2, 2);

	shape.setPosition(125, 250);

	angle = 30;
	velocity = 100;
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
	shape.move((cos(angle*PI)*velocity*dt), (sin(angle*PI)*velocity*dt));

	if (shape.getPosition().x < 2.0f)
	{
		shape.setPosition(2.0f, shape.getPosition().y);
		CollisionWith(0.0);
	}
	if (shape.getPosition().x > 248.0f)
	{
		shape.setPosition(248.0f, shape.getPosition().y);
		CollisionWith(180.0f);
	}
	if (shape.getPosition().y < 2.0f)
	{
		shape.setPosition(shape.getPosition().x, 2.0f);
		CollisionWith(90.0f);
	}
	if (shape.getPosition().y > 498.0f)
	{
		shape.setPosition(shape.getPosition().x, 498.0f);
		CollisionWith(270.0f);
	}	
	if (angle >= 360)
		angle -= 360;
	else if (angle < 0)
		angle += 360;
}

void Ball::SetPosition(float x, float y)
{
	shape.setPosition(x, y);
}

void Ball::SetSpeed(int velocity)
{
	this->velocity = velocity;
}

sf::RectangleShape Ball::GetShape()
{
	return shape;
}

void Ball::CollisionWith(float collisionAngle)
{
	??? häpiä
}