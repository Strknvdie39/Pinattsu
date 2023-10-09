#pragma once
#include "Map.h"
#include "Bomb.h"
#include "sio_client.h"
#include "Player.h"

class Game
{
private:
	Map _mapGame;
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

	void updateMap(std::vector<sio::message::ptr>& map);

	Map getMapGame() {
		return _mapGame;
	}

	void updateBombs(std::vector<Bomb> bombs)
	{
		_bombs = bombs;
	}

	void addBomb(Bomb &bomb);

	//todo
	void removeBomb(Bomb& bomb);

};

