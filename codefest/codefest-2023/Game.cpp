#include "Game.h"


void Game::updateMap(std::vector<sio::message::ptr>& map)
{
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

	_mapGame.initializeMap(new_map);

}

void Game::addBomb(Bomb &bomb)
{
	_bombs.push_back(bomb);
}

void Game::removeBomb(Bomb& bomb)
{
	//todo
}
