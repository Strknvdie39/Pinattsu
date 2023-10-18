#include "Game.h"
#include <queue>
#include <iostream>
#define LEFT "1"
#define RIGHT "2"
#define UP "3"
#define DOWN "4"

class queueNode
{
public:
    queueNode(Position pt, queueNode* previousNode) : pt(pt), previousNode(previousNode){}
    Position pt;               // The coordinates of a cell
    queueNode* previousNode;  // direction from the previous cell    
};

std::string getRelativePosition(Position src, Position dest) {
    int dRow = src.getRow() - dest.getRow();
    int dCol = src.getCol() - dest.getCol();
    if (dRow > 0) return UP;
    if (dRow < 0) return DOWN;
    if (dCol > 0) return LEFT;
    if (dCol < 0) return RIGHT;
    return "";
}

bool isValid(int row, int col)
{
    // return true if row number and column number
    // is in range
    return (row >= 0) && (row < ROW) &&
        (col >= 0) && (col < COL);
}

// These arrays are used to get row and column
// numbers of 4 neighbours of a given cell
int rowNum[] = { -1, 0, 0, 1 };
int colNum[] = { 0, -1, 1, 0 };

// find path to closest safe cell
std::string findPath(const std::vector<std::vector<int>>& map, Position src)
{
    return "";
}

void print_queue(std::queue<queueNode*> q)
{
    while (!q.empty())
    {
        std::cout << q.front()->pt.getRow() << "," << q.front()->pt.getCol() << " : ";
        q.pop();
    }
    std::cout << std::endl;
}

std::string Game::getPath(bool (*destinationCriteria)(const std::vector<std::vector<int>>& map, int row, int col))
{
    std::vector<std::vector<int>> map = MAP;
    Position src = _player.getPosition();
    if (map.empty()) return "";

    std::string path = "";
    queueNode* d = nullptr;
    queueNode* s = new queueNode(src, nullptr);

    // Create a queue for BFS
    std::queue<queueNode*> q;
    bool visited[100][100];
    memset(visited, false, sizeof visited);
    visited[src.getRow()][src.getCol()] = true;

    q.push(s);
    while (!q.empty())
    {
        d = nullptr;
        queueNode* curr = q.front();
        Position pt = curr->pt;
        //std::cout << ":::" << pt.getRow() << "," << pt.getCol() << ":::" << std::endl;
        //print_queue(q);
        if (destinationCriteria(map, pt.getRow(), pt.getCol())) {
            d = curr;
            //std::cout << "================" << std::endl;
            break;
        }

        q.pop();

        for (int i = 0; i < 4; i++)
        {
            int row = pt.getRow() + rowNum[i];
            int col = pt.getCol() + colNum[i];

            if (isValid(row, col) && map[row][col] <= 0 && !visited[row][col])
            {
                visited[row][col] = true;
                queueNode* Adjcell = new queueNode({row, col}, curr );
                q.push(Adjcell);
            }
        }
        //std::cout << "================" << std::endl;
    }

    while (d != nullptr && d->previousNode != nullptr && d->previousNode != d) {
        Position sr = d->previousNode->pt;
        Position ds = d->pt;
        path = getRelativePosition(sr, ds);
        //std::cout << "path " << sr.getRow() << ", " << sr.getCol() << "->" << ds.getRow() << ", " << ds.getCol() << ": " << path << std::endl;
        d = d->previousNode;
    }
    return path;
}

void Game::updateMap(std::map<std::string, sio::message::ptr> map_info)
{    
    _mapGame.updateMap(map_info);
}


