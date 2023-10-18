#pragma once
#include <vector>
#include <sio_message.h>

#define ROAD 0
#define WALL 1
#define BALK 2

#define TP_GATE 3
#define SPEED_EGG 4
#define ATTACK_EGG 5
#define DELAY_EGG 6
#define MYSTIC_EGG 7

#define TEAM_GST_EGG 8
#define ENEMY_GST_EGG 9

class Map
{
public:
	size_t getSizeRows();
	size_t getSizeCols();

	std::vector<std::pair<int, int>> &getBombMap();
	std::vector<std::vector<int>> &getMap();
	void updateMap(std::map<std::string, sio::message::ptr> map_info);
	void updatePositionInMap(int &row, int &col, int &value);
	void print();
	~Map();
	Map();

private:
	int _rows{ 0 };
	int _cols{ 0 };

	std::vector<std::vector<int>> _map;
	std::vector<std::vector<int>> _bombMap;

};
