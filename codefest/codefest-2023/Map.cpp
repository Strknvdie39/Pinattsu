#include "Map.h"

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

void Map::initializeMap(const std::vector<std::vector<int>>& map) {
	_map = map;
}

void Map::updatePositionInMap(int &row, int &col, int &value)
{
	_map[row][col] = value;
}
