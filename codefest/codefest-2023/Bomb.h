#pragma once
#include "Position.h"
class Bomb
{
private:
	//todo:
	Position _position;

public:
	Bomb(Position position) :_position{ position } {};
	~Bomb() {};
};

