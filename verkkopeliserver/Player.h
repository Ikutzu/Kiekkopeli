#pragma once
#include <SFML\Graphics.hpp>


class Player
{

public:
	Player();
	~Player();
	void Update(float dt);
	void SetPosition(int position);
	void SetYposition(int Yposition);
	sf::RectangleShape GetShape();
	
private:

	int socket;
	sf::RectangleShape shape;
};

