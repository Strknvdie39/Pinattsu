#pragma once

class Item
{
	enum flag
	{
		flag_none = 0,
		flag_balk,
		flag_wall,
		flag_road,
		flag_teleport_gate,
		flag_quarantine_place,
		flag_daragon_egg = 6,
	};
private:
	int type_item{ flag_none };

public:
	//todo
};

