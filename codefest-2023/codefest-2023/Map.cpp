#include "Map.h"
#include <iostream>
#include "Game.h"

Map::Map()
{

}


Map::~Map()
{
}

size_t Map::getSizeRows()
{
	return _map.size();
}

size_t Map::getSizeCols()
{
	return _map[0].size();
}

std::vector<std::vector<int>>& Map::getMap() {
	return _map;
}

// These arrays are used to get row and column
// numbers of 4 neighbours of a given cell
extern int rowNum[];
extern int colNum[];

bool isValid(const int& row, const int& col, const int& row_size, const int& col_size) {
	return 0 <= row && row < row_size && 0 <= col && col < col_size;
}

extern int player_id;
extern std::string player_id_str;
bool firstTime = true;
void Map::updateMap(std::map<std::string, sio::message::ptr> map_info) {
	int enemy_id = player_id % 2;
	auto map = map_info["map"]->get_vector();
	auto players = map_info["players"]->get_vector();
	auto bombs = map_info["bombs"]->get_vector();
	auto spoils = map_info["spoils"]->get_vector();
	auto dragonEggGSTArray = map_info["dragonEggGSTArray"]->get_vector();

	int power[] = { players[0]->get_map()["power"]->get_int(), players[1]->get_map()["power"]->get_int() };
	auto myCurrentPosition = players[player_id - 1]->get_map()["currentPosition"]->get_map();
	auto enemyCurrentPosition = players[enemy_id]->get_map()["currentPosition"]->get_map();
	Game::getInstance()->updatePlayerPosition(myCurrentPosition);
	
	// Walls
	if (firstTime) {
		std::vector<std::vector<int>> new_map;
		for (const auto& map_row : map)
		{
			std::vector<int> row;
			for (const auto& r : map_row->get_vector())
			{
				row.push_back(r->get_int());
			}
			new_map.push_back(row);
		}
		_map = new_map;
		firstTime = false;
	}
	else {
		for (int i = 0; i < _map.size(); i++)
		{
			const sio::message::ptr& map_row = map[i];
			const std::vector<sio::message::ptr> row = map_row->get_vector();
			for (int j = 0; j < row.size(); j++)
			{
				_map[i][j] = row[j]->get_int();
			}
		}
	}
	_map[enemyCurrentPosition["row"]->get_int()][enemyCurrentPosition["col"]->get_int()] = 1;

	// Bombs
	for (const auto& bomb : bombs)
	{
		auto _bomb = bomb->get_map();
		const int playerId = stoi(_bomb["playerId"]->get_string().substr(6, 1));
		int remainTime = _bomb["remainTime"]->get_int();
		const int row = _bomb["row"]->get_int();
		const int col = _bomb["col"]->get_int();
		_map[row][col] = 1;
		for (int j = 0; j < 4; j++) {
			for (int i = 1; i <= power[playerId - 1]; i++)
			{
				int _row = row + i * rowNum[j];
				int _col = col + i * colNum[j];

				if (isValid(_row, _col, _map.size(), _map[0].size())) {
					if (_map[_row][_col] == WALL) break;
					if (_map[_row][_col] == ROAD) _map[_row][_col] = -remainTime;
				}
			}
		}
	}

	// Eggs	
	for (const auto& egg : spoils) {
		auto _egg = egg->get_map();
		const int row = _egg["row"]->get_int();
		const int col = _egg["col"]->get_int();
		const int type = _egg["spoil_type"]->get_int();
		switch (type) {
		case 3:
			_map[row][col] = SPEED_EGG;
			break;
		case 4:
			_map[row][col] = ATTACK_EGG;
			break;
		case 5:
			_map[row][col] = DELAY_EGG;
			break;
		case 6:
			_map[row][col] = MYSTIC_EGG;
			break;
		default:
			break;
		}
	}

	// GST Eggs
	for (const auto& dragonEggGST : dragonEggGSTArray) {
		auto _dragonEggGST = dragonEggGST->get_map();
		int row = _dragonEggGST["row"]->get_int();
		int col = _dragonEggGST["col"]->get_int();
		std::string id = _dragonEggGST["id"]->get_string();
		if (id == player_id_str) {
			_map[row][col] = TEAM_GST_EGG;
		}
		else {
			_map[row][col] = ENEMY_GST_EGG;
		}
	}	
}

void Map::updatePositionInMap(int &row, int &col, int &value)
{
	_map[row][col] = value;
}

void Map::print() {
	for (std::vector<int> m : _map) {
		for (int i : m) {
			std::cout << i << " ";
		}
		std::cout << std::endl;
	}
}
