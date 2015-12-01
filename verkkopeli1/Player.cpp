#include "Player.h"


Player::Player()
{
	shape.setSize(sf::Vector2f(30.0f, 6.0f));
	shape.setOrigin(15, 3);
	shape.setPosition(125, 0);

	speed = 100;

}

Player::~Player()
{
}

void Player::Draw(sf::RenderWindow* win)
{
	win->draw(shape);
}

void Player::Update(float dt)
{
	if (shape.getPosition().x < 15.0f)
		shape.setPosition(15.0f, yPosition);
	if (shape.getPosition().x > 235.0f)
		shape.setPosition(235.0f, yPosition);
}

void Player::Move(float multiplier)
{
	shape.move(speed*multiplier, 0.0f);
}

void Player::SetPosition(float position)
{
	shape.setPosition(position, shape.getPosition().y);
}

void Player::SetYposition(float Yposition)
{
	yPosition = Yposition;
	shape.setPosition(shape.getPosition().x, yPosition);
}

sf::RectangleShape Player::GetShape()
{
	return shape;
}