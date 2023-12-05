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
	Player _player;
	std::vector<Bomb> _bombs;

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

	Player getPlayer() {
		return _player;
	}

	Map getMapGame() {
		return _mapGame;
	}

	bool isCurrentlySafe() {
		Position curr = _player.getPosition();
		int currPosValue = _mapGame.getMap()[curr.getRow()][curr.getCol()];
		return currPosValue == ROAD;
	}

	void updateMap(std::map<std::string, sio::message::ptr> map_info);

	void updatePlayerPosition(std::map<std::string, sio::message::ptr> currentPosition) {
		_player.updatePosotion(currentPosition["row"]->get_int(), currentPosition["col"]->get_int());
	}

	void updatePlayerPosition(Position pos) {
		_player.updatePosotion(pos.getRow(), pos.getCol());
	}
	void updatePlayerStats(int power, int speed, int delay) {
		_player.updateStats(power, speed, delay);
	}

	void updateBombs(std::vector<Bomb> bombs)
	{
		_bombs = bombs;
	}

	std::pair<std::string, int> getPath(int destination, bool potentialPath);

	void updatePositionInMap(int row, int col, int value) {
		_mapGame.updatePositionInMap(row, col, value);
	}
};

