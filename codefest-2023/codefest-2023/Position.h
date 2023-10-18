#pragma once
class Position
{
private:
	int _row;
	int _col; 
public:
	Position(int &row, int &col) :_row{ row }, _col{ col }{}
	Position(){}
	~Position(){}
	int getRow() { return _row; }
	int getCol() { return _col; }
	void setRow(int row) { _row = row; }
	void setCol(int col) { _col = col; }
};

