#pragma once
#include "Map.h"
#include "Bomb.h"
#include "sio_client.h"
#include "Player.h"

#define COL Game::getInstance()->getMapGame().getSizeCols()
#define ROW Game::getInstance()->getMapGame().getSizeRows()
#define MAP Game::getInstance()->getMapGame().getMap()
class Game
{
private:
	Map _mapGame;
	Player _player, _enemy;
	std::vector<Bomb> _bombs;
	std::vector<Player> _players;

	static Game *_gameInstance;
	Game() {};
public:
	Game(const Game& obj) = delete;
	~Game() {};

	static Game* getInstance() {
		if (!_gameInstance) {
			_gameInstance = new Game();
		}
		return _gameInstance;
	}

	void updateMap(std::map<std::string, sio::message::ptr> map_info);

	Map getMapGame() {
		return _mapGame;
	}

	void updatePlayerPosition(std::map<std::string, sio::message::ptr> currentPosition) {
		_player.updatePosotion(currentPosition["row"]->get_int(), currentPosition["col"]->get_int());
	}

	void updateBombs(std::vector<Bomb> bombs)
	{
		_bombs = bombs;
	}

	std::string getPath(bool (*destinationCriteria)(const std::vector<std::vector<int>>& map, int row, int col));
};

