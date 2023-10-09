#pragma once
#include <vector>

class Map
{
public:
	size_t getSizeRows();
	size_t getSizeCols();

	std::vector<std::vector<int>> &getMap();
	void initializeMap(const std::vector<std::vector<int>>& map);
	void updatePositionInMap(int &row, int &col, int &value);
	~Map();
	Map();

private:
	int _rows{ 0 };
	int _cols{ 0 };

	std::vector<std::vector<int>> _map;

};
