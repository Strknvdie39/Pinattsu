#pragma once
#include "Map.h"
#include "Bomb.h"
#include "sio_client.h"
#include "Player.h"

#define COL Game::getInstance()->getMapGame().getSizeCols()
#define ROW Game::getInstance()->getMapGame().getSizeRows()
#define MAP Game::getInstance()->getMapGame().getMap()
#define PLAYER Game::getInstance()->getMapGame().getMap()
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

	bool isCurrentlySafe() {
		Position curr = _player.getPosition();
		int currPosValue = _mapGame.getMap()[curr.getRow()][curr.getCol()];
		return currPosValue == ROAD ||
			currPosValue <= - 600;
	}

	void updatePlayerPosition(std::map<std::string, sio::message::ptr> currentPosition) {
		_player.updatePosotion(currentPosition["row"]->get_int(), currentPosition["col"]->get_int());
	}

	void updateBombs(std::vector<Bomb> bombs)
	{
		_bombs = bombs;
	}

	std::string getPath(int destination, bool potentialPath);
};

