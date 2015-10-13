#include "Player.h"


Player::Player()
{
	shape.setSize(sf::Vector2f(30.0f, 6.0f));
	shape.setOrigin(15, 3);
	shape.setPosition(125, 0);
}

Player::~Player()
{
}


void Player::Update(float dt)
{
	if (shape.getPosition().x < 15.0f)
		shape.setPosition(15.0f, 450.0f);
	if (shape.getPosition().x > 235.0f)
		shape.setPosition(235.0f, 450.0f);
}

void Player::SetPosition(float position)
{
	shape.setPosition(position, shape.getPosition().y);
}

void Player::SetYposition(float Yposition)
{
	shape.setPosition(shape.getPosition().x, Yposition);
}

sf::RectangleShape Player::GetShape()
{
	return shape;
}