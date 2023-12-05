#pragma once
#include <map>
#include <sio_message.h>
#include "Position.h"

#define MAX_POWER 3
#define MAX_SPEED 250
#define MAX_DELAY 1000

class Player
{
public:
	int power, speed, delay;

	Player() {};
	~Player() {};

	std::string moveTo(int row, int col);
	void updatePosotion(int row, int col);
	void updateStats(int power, int speed, int delay) {
		this->power = power;
		this->speed = speed;
		this->delay = delay;
	}
	Position getPosition() { return _currentpos; }	
private:
	Position _currentpos;
};

