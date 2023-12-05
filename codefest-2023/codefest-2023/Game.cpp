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
    queueNode(Position pt, queueNode* previousNode, int dist) : pt(pt), previousNode(previousNode), dist(dist){}
    Position pt;               // The coordinates of a cell
    queueNode* previousNode;   // direction from the previous cell    
    int dist;                  // Distance to root
};

std::string getRelativePosition(Position src, Position dest) {    
    if (MAP[dest.getRow()][dest.getCol()] == BALK_ABOUT_TO_EXPLODE) return "";
    if (MAP[dest.getRow()][dest.getCol()] == BALK) return "b";
    if (MAP[dest.getRow()][dest.getCol()] == ENEMY) return "b";
    if (MAP[dest.getRow()][dest.getCol()] == ENEMY_GST_EGG) return "b";
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

bool isDest(const std::vector<std::vector<int>>& map, int row, int col, int distance)
{
    Game* elBombGame = Game::getInstance();
    int spd = elBombGame->getPlayer().speed;
    return (map[row][col] != WALL &&
        map[row][col] != TP_GATE &&
        map[row][col] != BOMB &&
        map[row][col] != PRISON &&
        map[row][col] != TEAM_GST_EGG &&
        map[row][col] != BOMB) &&
        map[row][col] == ROAD ||
        map[row][col] == SPEED_EGG ||
        map[row][col] == POWER_EGG ||
        map[row][col] == DELAY_EGG ||
        map[row][col] == MYSTIC_EGG ||
        map[row][col] == ENEMY_GST_EGG ||
        map[row][col] == BALK ||
        map[row][col] == BALK_ABOUT_TO_EXPLODE ||
        map[row][col] == TP_GATE ||
        map[row][col] == ENEMY ||
        (map[row][col] + distance * spd) <= - 150 - spd - BOMB_OFFSET ||
        map[row][col] >= BOMB_OFFSET - 50;
}

bool isSafe(const std::vector<std::vector<int>>& map, int row, int col, int distance) {
    Game* elBombGame = Game::getInstance();
    int spd = elBombGame->getPlayer().speed;
    return (map[row][col] != WALL &&
        map[row][col] != TP_GATE &&
        map[row][col] != BOMB &&
        map[row][col] != PRISON &&
        map[row][col] != TEAM_GST_EGG &&
        map[row][col] != BOMB) && 
        map[row][col] == ROAD ||
        map[row][col] == SPEED_EGG ||
        map[row][col] == POWER_EGG ||
        map[row][col] == DELAY_EGG ||
        map[row][col] == MYSTIC_EGG ||
        map[row][col] <= - 150 - spd - BOMB_OFFSET ||
        map[row][col] >= BOMB_OFFSET - 50;
    
}

bool isTemporaySafe(const std::vector<std::vector<int>>& map, int row, int col, int distance) {
    Game* elBombGame = Game::getInstance();
    int spd = elBombGame->getPlayer().speed;
    return map[row][col] == ROAD ||
        map[row][col] == SPEED_EGG ||
        map[row][col] == POWER_EGG ||
        map[row][col] == DELAY_EGG ||
        map[row][col] == MYSTIC_EGG;
}

bool isPowerUp(const std::vector<std::vector<int>>& map, int row, int col) {
    Game* elBombGame = Game::getInstance();
    Player p = elBombGame->getPlayer();
    //printf("spd p d : %d %d %d\n", p.speed, p.power, p.delay);
    return (map[row][col] == SPEED_EGG && p.speed < MAX_SPEED)||
        (map[row][col] == POWER_EGG && p.power < MAX_POWER) ||
        (map[row][col] == DELAY_EGG && p.delay > MAX_DELAY);
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

std::pair<std::string, int> Game::getPath(int destination, bool potentialPath)
{
    Game* elBombGame = Game::getInstance();
    std::vector<std::vector<int>> map = MAP;
    int spd = elBombGame->getPlayer().speed;
    if (map.empty()) return std::pair<std::string, int>("", 0);
    Position src = _player.getPosition();
    std::pair<std::string, int> path; // (move, distance)
    std::queue<queueNode*> q;
    bool visited[100][100];

    int distance = 0;
    path.second = distance;
    if (destination == ENEMY && map[src.getRow()][src.getCol()] == ENEMY) {
        path.first = "b";
        goto end;
    }    

    queueNode* d = nullptr;
    queueNode* s = new queueNode(src, nullptr, distance);

    // Create a queue for BFS
    memset(visited, false, sizeof visited);
    visited[src.getRow()][src.getCol()] = true;

    q.push(s);
    int max_remain_time = -10000;
    while (!q.empty())
    {
        queueNode* curr = q.front();
        Position pt = curr->pt;
        distance = curr->dist;
        if (map[pt.getRow()][pt.getCol()] == TP_GATE && destination != TP_GATE) {
            q.pop();
            continue;
        }

        if (map[pt.getRow()][pt.getCol()] < 0) map[pt.getRow()][pt.getCol()] += distance * spd;
        if (destination == SAFE) {
            if (isSafe(map, pt.getRow(), pt.getCol(), distance)) {
                if (max_remain_time < map[pt.getRow()][pt.getCol()]) {
                    d = curr;
                    max_remain_time = map[pt.getRow()][pt.getCol()];
                    if (max_remain_time >= 0) break;
                }
            }
        }
        else if (destination == TEMPORARY_SAFE) {
            if (isTemporaySafe(map, pt.getRow(), pt.getCol(), distance)) {
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
        if (destination != ENEMY && destination != ENEMY_GST_EGG && map[pt.getRow()][pt.getCol()] == BALK) continue;
        if (map[pt.getRow()][pt.getCol()] == ENEMY) continue;
        if (map[pt.getRow()][pt.getCol()] == TP_GATE) continue;
        //int maxDistance = 10;
        for (int i = 0; i < 4; i++)
        {
            int row = pt.getRow() + rowNum[i];
            int col = pt.getCol() + colNum[i];

            if (isValid(map, row, col) && !visited[row][col] && 
                (isDest(map, row, col, distance) ||
                (potentialPath == true && (map[row][col] == BOMB || map[row][col] < 0)))
               )
            {
                visited[row][col] = true;
                queueNode* Adjcell = new queueNode({row, col}, curr, curr->dist+1);
                q.push(Adjcell);
            }
        }
    }
    path.second = distance;

    while (d != nullptr && d->previousNode != nullptr && d->previousNode != d) {
        Position sr = d->previousNode->pt;
        Position ds = d->pt;
        if (potentialPath && map[ds.getRow()][ds.getCol()] < 0) path.first = "";
        else path.first = getRelativePosition(sr, ds);
        d = d->previousNode;
    }

end:
    if (path.first == "b") {
        int temp = MAP[src.getRow()][src.getCol()];
        elBombGame->updatePositionInMap(src.getRow(), src.getCol(), BOMB);
        std::string movePath = getPath(TEMPORARY_SAFE, false).first;
        elBombGame->updatePositionInMap(src.getRow(), src.getCol(), temp);
        if (movePath == "") return std::pair<std::string, int>("", 0);
    }
    return path;
}

void Game::updateMap(std::map<std::string, sio::message::ptr> map_info)
{    
    _mapGame.updateMap(map_info);
}
