#pragma once
class Position
{
private:
	int _row;
	int _col; 
public:
	Position(int &row, int &col) :_row{ row }, _col{ col }{}
	~Position(){}
	inline int getRow() { return _row; }
	inline int getCol() { return _col; }
};

