#include "sio_client.h"
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <Windows.h>
#include "Map.h"
#include "Game.h"
#include <conio.h>
#include <chrono>
#include <fstream>


#ifdef WIN32
#define HIGHLIGHT(__O__) std::cout<<__O__<<std::endl
#define EM(__O__) std::cout<<__O__<<std::endl

#include <stdio.h>
#include <tchar.h>
#define MAIN_FUNC int _tmain(int argc, _TCHAR* argv[])
#else
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl

#define MAIN_FUNC int main(int argc ,const char* args[])
#endif

using namespace sio;
using namespace std;

//#define GAME_ID		"3f22d49d-4897-42a1-a42c-af317cf1ccf9"
//std::string key = "player";
//std::string player_id = "player";

int ping = 0;
auto pingStart = chrono::high_resolution_clock::now();
auto pingEnd = chrono::high_resolution_clock::now();

// index = 0 or 1
int player_index = -1;
int enemy_index = -1;

bool isEnemyInPrison = false;
int count_BALK_ABOUT_TO_EXPLODE = 0;

string url, game_id, key, player_id;

std::mutex _lock;
std::condition_variable_any _cond;

sio::socket::ptr current_socket;
Game* Game::_gameInstance = nullptr;

int participants = -1;


class connection_listener
{
	sio::client& handler;

public:
	bool connect_finish = false;

	connection_listener(sio::client& h) :
		handler(h)
	{
	}


	void on_connected()
	{
		_lock.lock();
		_cond.notify_all();
		connect_finish = true;
		_lock.unlock();
	}
	void on_close(client::close_reason const& reason)
	{
		std::cout << "sio closed " << std::endl;
		exit(0);
	}

	void on_fail()
	{
		std::cout << "sio failed " << std::endl;
		exit(0);
	}


};

void join_room(std::string& game_id, std::string& player_id)
{
	std::string socket_event = "join game";
	sio::message::ptr object_message_join_room = sio::object_message::create();
	object_message_join_room->get_map().insert(std::pair<std::string, sio::message::ptr>("game_id", sio::string_message::create(game_id)));
	object_message_join_room->get_map().insert(std::pair<std::string, sio::message::ptr>("player_id", sio::string_message::create(player_id)));

	current_socket->emit(socket_event, object_message_join_room);
}


void bind_events()
{
	current_socket->on("join game", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			_lock.lock();
			EM("event join game");
			std::string game_id = data->get_map()["game_id"]->get_string();
			std::string player_id = data->get_map()["player_id"]->get_string();
			EM("game_id: " << game_id << " player_id: " << player_id);
			_lock.unlock();
		}));

	current_socket->on("ticktack player", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck, message::list& ack_resp)
		{
			_lock.lock();
			if (count_BALK_ABOUT_TO_EXPLODE) {
				pingEnd = chrono::high_resolution_clock::now();
				ping = chrono::duration_cast<chrono::milliseconds>(pingEnd - pingStart).count();
				pingStart = chrono::high_resolution_clock::now();
			}
			

			// Init
			Game* elBombGame = Game::getInstance();
			std::map<std::string, message::ptr> map_info = data->get_map()["map_info"]->get_map();
			std::pair<std::string, int> path1;
			std::pair<std::string, int> path2;
			std::string movePath;
			sio::message::ptr object_message_drive_player = sio::object_message::create();

			// update
			elBombGame->updateMap(map_info);
			
			// move
			/* Go to safe zone */
			movePath = elBombGame->getPath(TEMPORARY_SAFE, false).first;
			if (movePath == "no_temp_safe_zone") movePath = elBombGame->getPath(SAFE, false).first;
			if (movePath != "") goto send;

			if (!elBombGame->isCurrentlySafe()) {
				movePath = elBombGame->getPath(TP_GATE, false).first;
				if (movePath != "") goto send;
			}

			/* Get power up */
			movePath = elBombGame->getPath(POWER_UP, false).first;
			if (movePath != "") goto send;

			/* Follow enemy */
			if (isEnemyInPrison) goto end;
			
			path1 = elBombGame->getPath(ENEMY, false);
			path2 = elBombGame->getPath(ENEMY, true);
			if (path2.second+2< path1.second) {
				movePath = path2.first;
				if (movePath == "wait") goto end;
				if (movePath == "")	goto end;
			}
			else {
				movePath = path1.first;
				if (movePath == "")	goto end;
			}
			
			//movePath = elBombGame->getPath(TP_GATE, false);
			//if (movePath == "") goto end;


			send:
			//object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("x")));
			object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create(movePath)));
			current_socket->emit("drive player", object_message_drive_player);

			end:
			_lock.unlock();
		}));
}

void Setup(string filepath)
{
	ifstream inputFile(filepath);
	if (getline(inputFile, url) &&
		getline(inputFile, game_id) &&
		getline(inputFile, key) &&
		getline(inputFile, player_id))
	{}
	else
	{
		cerr << "Error file" << endl;
	}
	inputFile.close();
}

MAIN_FUNC
{
	//string game_id = GAME_ID;
	//string URI = "http://127.0.0.1";


	//string url,game_id, key, player_id;
	//cout << "url: ";
	//cin >> url;
	//cout << "game id: ";
	//cin >> game_id;
	//cout << "key: ";
	//cin >> key;
	//cout << "player id: ";
	//cin >> player_id;

	string input = "input.txt";
	Setup(input);

	sio::client h;
	connection_listener cl(h);
	sio::message::ptr object_message_drive_player = sio::object_message::create();

	Game* elBombGame = Game::getInstance();

	h.set_open_listener(std::bind(&connection_listener::on_connected, &cl));
	h.set_close_listener(std::bind(&connection_listener::on_close, &cl, std::placeholders::_1));
	h.set_fail_listener(std::bind(&connection_listener::on_fail, &cl));
	h.connect(url);
	_lock.lock();
	if (!cl.connect_finish)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();
	current_socket = h.socket();

	join_room(game_id, key);
	bind_events();

	//char input;

	while (true)
	{
	//	input = _getch(); // Sử dụng hàm _getch() để đọc ký tự từ bàn phím

	//	if (input == 'q' || input == 'Q') {
	//		std::cout << "Chương trình kết thúc." << std::endl;
	//		break; // Thoát khỏi vòng lặp nếu nhấn 'Q'
	//	}

	//	if (input == 'p' || input == 'P') {
	//		Game::getInstance()->getMapGame().print();
	//	}


	//	// Xử lý ký tự được nhập
	//	switch (input) {
	//	case 'w':
	//	case 'W':
	//	{
	//		std::string socket_event = "drive player";
	//		sio::message::ptr object_message_drive_player = sio::object_message::create();
	//		object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("3")));
	//		current_socket->emit(socket_event, object_message_drive_player);
	//		break;
	//	}

	//	case 'a':
	//	case 'A':
	//	{
	//		std::string socket_event = "drive player";
	//		sio::message::ptr object_message_drive_player = sio::object_message::create();
	//		object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("1")));
	//		current_socket->emit(socket_event, object_message_drive_player);

	//		break;
	//	}
	//	case 's':
	//	case 'S':
	//	{
	//		std::string socket_event = "drive player";
	//		sio::message::ptr object_message_drive_player = sio::object_message::create();
	//		object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("4")));
	//		current_socket->emit(socket_event, object_message_drive_player);
	//		break;
	//	}
	//	case 'd':
	//	case 'D':
	//	{
	//		std::string socket_event = "drive player";
	//		sio::message::ptr object_message_drive_player = sio::object_message::create();
	//		object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("2")));
	//		current_socket->emit(socket_event, object_message_drive_player);
	//		break;
	//	}
	//	case 'b':
	//	case 'B':
	//	{
	//		std::string socket_event = "drive player";
	//		sio::message::ptr object_message_drive_player = sio::object_message::create();
	//		object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("b")));
	//		current_socket->emit(socket_event, object_message_drive_player);
	//		break;
	//	}
	//	default:
	//		std::cout << "Ký tự không hợp lệ." << std::endl;
	//	}
	}

	h.sync_close();
	h.clear_con_listeners();
	return 0;
}