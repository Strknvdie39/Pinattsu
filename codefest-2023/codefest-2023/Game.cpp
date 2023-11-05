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
    if (MAP[dest.getRow()][dest.getCol()] == BALK) return "b";
    if (MAP[dest.getRow()][dest.getCol()] == ENEMY) return "b";
    int dRow = src.getRow() - dest.getRow();
    int dCol = src.getCol() - dest.getCol();
    if (dRow > 0) return UP;
    if (dRow < 0) return DOWN;
    if (dCol > 0) return LEFT;
    if (dCol < 0) return RIGHT;
    return "";
}

bool isValid(const std::vector<std::vector<int>>& map, int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

bool isDest(const std::vector<std::vector<int>>& map, int row, int col)
{
    return map[row][col] == ROAD ||
            map[row][col] == SPEED_EGG ||
            map[row][col] == ATTACK_EGG ||
            map[row][col] == DELAY_EGG ||
            map[row][col] == MYSTIC_EGG ||
            map[row][col] == BALK ||
            map[row][col] == TP_GATE ||
            map[row][col] == ENEMY ||
            map[row][col] <= - 450 - BOMB_OFFSET;
}

bool isSafe(const std::vector<std::vector<int>>& map, int row, int col) {
    return map[row][col] == ROAD ||
        map[row][col] == SPEED_EGG ||
        map[row][col] == ATTACK_EGG ||
        map[row][col] == DELAY_EGG ||
        map[row][col] == MYSTIC_EGG;
}

bool isTemporaySafe(const std::vector<std::vector<int>>& map, int row, int col) {
    return map[row][col] == ROAD ||
        map[row][col] == SPEED_EGG ||
        map[row][col] == ATTACK_EGG ||
        map[row][col] == DELAY_EGG ||
        map[row][col] == MYSTIC_EGG ||
        map[row][col] <= - 450 - BOMB_OFFSET;
}

bool isPowerUp(const std::vector<std::vector<int>>& map, int row, int col) {
    return map[row][col] == SPEED_EGG ||
        map[row][col] == ATTACK_EGG ||
        map[row][col] == DELAY_EGG ||
        map[row][col] == MYSTIC_EGG;
}

// These arrays are used to get row and column
// numbers of 4 neighbours of a given cell
int rowNum[] = { -1, 0, 0, 1 };
int colNum[] = { 0, -1, 1, 0 };

void print_queue(std::queue<queueNode*> q)
{
    while (!q.empty())
    {
        std::cout << q.front()->pt.getRow() << "," << q.front()->pt.getCol() << " : ";
        q.pop();
    }
    std::cout << std::endl;
}

std::string Game::getPath(int destination, bool potentialPath)
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
        if (destination == SAFE) {
            if (isSafe(map, pt.getRow(), pt.getCol())) {
                d = curr;
                break;
            }
        }
        else if (destination == TEMPORARY_SAFE){
            if (isTemporaySafe(map, pt.getRow(), pt.getCol())) {
                d = curr;
                break;
            }
        }
        else if (destination == POWER_UP) {
            if (isPowerUp(map, pt.getRow(), pt.getCol())) {
                d = curr;
                break;
            }
        }
        else {
            if ((map[pt.getRow()][pt.getCol()] == destination)) {
                d = curr;
                break;
            }
        }

        q.pop();
        if (map[pt.getRow()][pt.getCol()] == BALK) continue;
        if (map[pt.getRow()][pt.getCol()] == TP_GATE) continue;
        if (map[pt.getRow()][pt.getCol()] == ENEMY) continue;

        for (int i = 0; i < 4; i++)
        {
            int row = pt.getRow() + rowNum[i];
            int col = pt.getCol() + colNum[i];

            if (isValid(map, row, col) && !visited[row][col] && 
                (isDest(map, row, col) || (potentialPath == true && (map[row][col] == BOMB || map[row][col] < 0))))
            {
                visited[row][col] = true;
                queueNode* Adjcell = new queueNode({row, col}, curr );
                q.push(Adjcell);
            }
        }
    }

    while (d != nullptr && d->previousNode != nullptr && d->previousNode != d) {
        Position sr = d->previousNode->pt;
        Position ds = d->pt;
        if (potentialPath) {
             if (MAP[ds.getRow()][ds.getCol()] == BOMB || MAP[ds.getRow()][ds.getCol()] < 0) return "wait";
        }
        path = getRelativePosition(sr, ds);
        d = d->previousNode;
    }
    return path;
}

void Game::updateMap(std::map<std::string, sio::message::ptr> map_info)
{    
    _mapGame.updateMap(map_info);
}


