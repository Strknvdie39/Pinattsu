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


#define GAME_ID		"32f808af-ca74-468d-8e6c-ef9f73b9b86a"
#define PLAYER_ID1	"player1-xxx"
#define PLAYER_ID2	"player2-xxx"

using namespace sio;
using namespace std;

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
			EM("ticktack");

			std::map<std::string, message::ptr> map_info = data->get_map()["map_info"]->get_map();

			//int64_t map_size_rows = map_info["size"]->get_map()["rows"]->get_int();
			//int64_t map_size_cols = map_info["size"]->get_map()["cols"]->get_int();

			auto map = map_info["map"]->get_vector();

			Game* elBombGame = Game::getInstance();

			// update player
			elBombGame->updateMap(map);

			auto players = map_info["players"]->get_vector();

			//class player


			//for (const auto& info : map_info)
			//{
			//	if (info.first.compare("size") == 0)
			//	{
			//		auto map_size = info.second.get();
			//		int64_t map_size_rows = map_size->get_map()["rows"]->get_int();
			//		int64_t map_size_cols = map_size->get_map()["cols"]->get_int();
			//		EM("rows :" << map_size_rows << " cols :" << map_size_cols);
			//		break;
			//	}
			//}

			//string user = data->get_map()["gameStatus"]->get_string(); //null => crash 
			//string message = data->get_map()["gameRemainTime"]->get_string();
			//EM(user << ":" << message);

			// send drive player  "direction": "1111333332222224444"

			{
				static int count = 0;
				if (count++ > 20)
				{
					
					std::string socket_event = "drive player";
					sio::message::ptr object_message_drive_player = sio::object_message::create();
					object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("1111b2222")));
					current_socket->emit(socket_event, object_message_drive_player);
					count = 0;
					EM("drive player");
				}

			}

			_lock.unlock();




		}));
}

MAIN_FUNC
{
	string game_id = GAME_ID;
	string player_id1 = PLAYER_ID1;
	string player_id2 = PLAYER_ID2;

	sio::client h;
	connection_listener cl(h);

	Game* elBombGame = Game::getInstance();

	h.set_open_listener(std::bind(&connection_listener::on_connected, &cl));
	h.set_close_listener(std::bind(&connection_listener::on_close, &cl, std::placeholders::_1));
	h.set_fail_listener(std::bind(&connection_listener::on_fail, &cl));
	h.connect("http://127.0.0.1");
	_lock.lock();
	if (!cl.connect_finish)
	{
		_cond.wait(_lock);
	}
	_lock.unlock();
	current_socket = h.socket();

	join_room(game_id, player_id1);
	join_room(game_id, player_id2);
	bind_events();

	char input;

	while (true)
	{
		input = _getch(); // Sử dụng hàm _getch() để đọc ký tự từ bàn phím

		if (input == 'q' || input == 'Q') {
			std::cout << "Chương trình kết thúc." << std::endl;
			break; // Thoát khỏi vòng lặp nếu nhấn 'Q'
		}

		// Xử lý ký tự được nhập
		switch (input) {
		case 'w':
		case 'W':
		{
			std::string socket_event = "drive player";
			sio::message::ptr object_message_drive_player = sio::object_message::create();
			object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("3")));
			current_socket->emit(socket_event, object_message_drive_player);
			break;
		}

		case 'a':
		case 'A':
		{
			std::string socket_event = "drive player";
			sio::message::ptr object_message_drive_player = sio::object_message::create();
			object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("1")));
			current_socket->emit(socket_event, object_message_drive_player);

			break;
		}
		case 's':
		case 'S':
		{
			std::string socket_event = "drive player";
			sio::message::ptr object_message_drive_player = sio::object_message::create();
			object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("4")));
			current_socket->emit(socket_event, object_message_drive_player);
			break;
		}
		case 'd':
		case 'D':
		{
			std::string socket_event = "drive player";
			sio::message::ptr object_message_drive_player = sio::object_message::create();
			object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("2")));
			current_socket->emit(socket_event, object_message_drive_player);
			break;
		}
		case 'b':
		case 'B':
		{
			std::string socket_event = "drive player";
			sio::message::ptr object_message_drive_player = sio::object_message::create();
			object_message_drive_player->get_map().insert(std::pair<std::string, sio::message::ptr>("direction", sio::string_message::create("b")));
			current_socket->emit(socket_event, object_message_drive_player);
			break;
		}
		default:
			std::cout << "Ký tự không hợp lệ." << std::endl;
		}
	}

	h.sync_close();
	h.clear_con_listeners();
	return 0;
}