#pragma once
#include "Position.h"
class Bomb
{
private:
	//todo:
	Position _position;
	float _remainTime;

public:
	Bomb(Position position, float remainTime) :_position{ position }, _remainTime{ remainTime } {};
	~Bomb() {};
};

