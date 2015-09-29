#include "Player.h"


Player::Player()
{
	shape.setSize(sf::Vector2f(30.0f, 6.0f));
	shape.setOrigin(15, 3);
	shape.setPosition(125, 450);

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
		shape.setPosition(15.0f, 450.0f);
	if (shape.getPosition().x > 235.0f)
		shape.setPosition(235.0f, 450.0f);
}

void Player::Move(float multiplier)
{
	shape.move(speed*multiplier, 0.0f);
}

sf::RectangleShape Player::GetShape()
{
	return shape;
}