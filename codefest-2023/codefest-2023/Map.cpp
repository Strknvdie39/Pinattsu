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

extern int ping;
extern int count_BALK_ABOUT_TO_EXPLODE;
extern int player_index;
extern int enemy_index;
extern bool isEnemyInPrison;
extern std::string player_id;
bool firstTime = true;
void Map::updateMap(std::map<std::string, sio::message::ptr> map_info) {
	auto map = map_info["map"]->get_vector();
	auto players = map_info["players"]->get_vector();
	auto bombs = map_info["bombs"]->get_vector();
	auto spoils = map_info["spoils"]->get_vector();
	auto dragonEggGSTArray = map_info["dragonEggGSTArray"]->get_vector();
	std::string dasda = players[1]->get_map()["id"]->get_string();
	// check player index
	if (firstTime) {
		player_index = (players[1]->get_map()["id"]->get_string() == player_id);
		enemy_index = (player_index+1) % 2;
	}

	Game::getInstance()->updatePlayerStats(players[player_index]->get_map()["power"]->get_int(),
											players[player_index]->get_map()["speed"]->get_int(), 
											players[player_index]->get_map()["delay"]->get_int());

	int power[] = { players[0]->get_map()["power"]->get_int(), players[1]->get_map()["power"]->get_int() };
	auto myCurrentPosition = players[player_index]->get_map()["currentPosition"]->get_map();
	auto enemyCurrentPosition = players[enemy_index]->get_map()["currentPosition"]->get_map();
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
	}
	else {
		for (int i = 0; i < _map.size(); i++)
		{
			const sio::message::ptr& map_row = map[i];
			const std::vector<sio::message::ptr> row = map_row->get_vector();
			for (int j = 0; j < row.size(); j++)
			{
				if (_map[i][j] == BALK_ABOUT_TO_EXPLODE && row[j]->get_int() == ROAD) {
					_map[i][j] = -BOMB_OFFSET;
					count_BALK_ABOUT_TO_EXPLODE--;
				}
				else _map[i][j] = row[j]->get_int();
			}
		
		}
	}

	// Eggs	
	for (const auto& egg : spoils) {
		auto _egg = egg->get_map();
		const int row = _egg["row"]->get_int();
		const int col = _egg["col"]->get_int();
		if (_map[row][col] < 0) continue;
		const int type = _egg["spoil_type"]->get_int();
		switch (type) {
		case 3:
			_map[row][col] = SPEED_EGG;
			break;
		case 4:
			_map[row][col] = POWER_EGG;
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

	// Bombs
	for (const auto& bomb : bombs)
	{
		auto _bomb = bomb->get_map();
		int playerId;
		if (_bomb["playerId"]->get_string() == player_id) playerId = player_index;
		else playerId = enemy_index;
		int remainTime =  - (_bomb["remainTime"]->get_int() + BOMB_OFFSET);
		const int row = _bomb["row"]->get_int();
		const int col = _bomb["col"]->get_int();
		_map[row][col] = BOMB;
		for (int j = 0; j < 4; j++) {
			for (int i = 1; i <= power[playerId]; i++)
			{
				int _row = row + i * rowNum[j];
				int _col = col + i * colNum[j];

				if (isValid(_row, _col, _map.size(), _map[0].size())) {
					if (_map[_row][_col] == WALL || 
						_map[_row][_col] == TEAM_GST_EGG || 
						_map[_row][_col] == ENEMY_GST_EGG ||
						_map[_row][_col] == BALK_ABOUT_TO_EXPLODE
					) break;

					if (_map[_row][_col] == BALK) 
					{
						_map[_row][_col] = BALK_ABOUT_TO_EXPLODE;
						count_BALK_ABOUT_TO_EXPLODE++;
						break;
					}

					if (_map[_row][_col] == TP_GATE) continue;

					_map[_row][_col] = remainTime;
				}
			}
		}
	}

	// GST Eggs
	for (const auto& dragonEggGST : dragonEggGSTArray) {
		auto _dragonEggGST = dragonEggGST->get_map();
		int row = _dragonEggGST["row"]->get_int();
		int col = _dragonEggGST["col"]->get_int();
		std::string id = _dragonEggGST["id"]->get_string();
		if (id == player_id) {
			_map[row][col] = TEAM_GST_EGG;
		}
		else {
			_map[row][col] = ENEMY_GST_EGG;
		}
	}

	// Enemy
	if (_map[enemyCurrentPosition["row"]->get_int()][enemyCurrentPosition["col"]->get_int()] != PRISON) {
		_map[enemyCurrentPosition["row"]->get_int()][enemyCurrentPosition["col"]->get_int()] = ENEMY;
		isEnemyInPrison = false;
	}
	else {
		isEnemyInPrison = true;
	}

	firstTime = false;
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
