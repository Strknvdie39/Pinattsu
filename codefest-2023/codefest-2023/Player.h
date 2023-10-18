#pragma once
#include <map>
#include <sio_message.h>
#include "Position.h"
class Player
{
public:
	Player() {};
	~Player() {};

	std::string moveTo(int row, int col);
	void updatePosotion(int row, int col);
	Position getPosition() { return _currentpos; }	
private:
	Position _currentpos;
};

