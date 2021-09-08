#include "algorithm.h"
#include "config.h"
#include "gamemap.h"

FindPathAlgorithm::FindPathAlgorithm()
{
    init();
}
void FindPathAlgorithm::init()
{
    memset(resultMap,-1,sizeof resultMap);
    v.clear();
    foundPathOrNot = false;
    path.clear();
}
void FindPathAlgorithm::findAvailableCell(int x, int y, int depth, Character* character[], int characterNum)
{
    for(int i=0;i<characterNum;i++)
        //遇见敌人绕不过去
        if(character[i]!=nowCharacter&&character[i]->characterState!=Character::DEAD)
            if(character[i]->m_cellx==x&&character[i]->m_celly==y)
                return;

    if(resultMap[x][y]>=0)
    {
        resultMap[x][y] = min(depth, resultMap[x][y]);
    }
    else
    {
        resultMap[x][y]=depth;
        v.push_back(std::make_pair(x,y));
    }

    if(depth>=totalMove)return;

    if(x - 1 >= 1 && GameMap::binMap[x - 1][y] != 0)
    {
        if(GameMap::binMap[x - 1][y] == 3)//水
            findAvailableCell(x - 1, y, depth + 2, character, characterNum);
        else
            findAvailableCell(x - 1, y, depth + 1, character, characterNum);
    }
    if(x + 1 <= 50 && GameMap::binMap[x + 1][y] != 0)
    {
        if(GameMap::binMap[x + 1][y] == 3)//水
            findAvailableCell(x + 1, y, depth + 2, character, characterNum);
        else
            findAvailableCell(x + 1, y, depth + 1, character, characterNum);
    }
    if(y + 1 <= 30 && GameMap::binMap[x][y + 1]!=0)
    {

        if(GameMap::binMap[x][y + 1] == 3)//水
            findAvailableCell(x, y + 1, depth + 2, character, characterNum);
        else
            findAvailableCell(x, y + 1, depth + 1, character, characterNum);
    }
    if(y - 1 >= 1 && GameMap::binMap[x][y - 1] != 0)
    {
        if(GameMap::binMap[x][y - 1] == 3)//水
            findAvailableCell(x, y - 1, depth + 2, character, characterNum);
        else
            findAvailableCell(x, y - 1, depth + 1, character, characterNum);
    }
}

void FindPathAlgorithm::findPath(int x, int y, int endx, int endy, int tempsteps, int totalsteps)
{
    if(foundPathOrNot)return;
    if(tempsteps > resultMap[x][y])return ;
    if(x == endx && y == endy)
    {
        foundPathOrNot = true;
        return;
    }
    if(x-1>=1&&GameMap::binMap[x-1][y]!=0)
    {
        path.push_back(LEFT);
        if(GameMap::binMap[x-1][y]!=3)
            findPath(x-1,y,endx,endy,tempsteps+1,totalsteps);
        else
            findPath(x-1,y,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back();
    }
    if(x+1<=50&&GameMap::binMap[x+1][y]!=0)
    {
        path.push_back(RIGHT);
        if(GameMap::binMap[x+1][y]!=3)
            findPath(x+1,y,endx,endy,tempsteps+1,totalsteps);
        else
            findPath(x+1,y,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back();
    }
    if(y+1<=30&&GameMap::binMap[x][y+1]!=0)
    {
        path.push_back(DOWN);
        if(GameMap::binMap[x][y+1]!=3)
            findPath(x,y+1,endx,endy,tempsteps+1,totalsteps);
        else
            findPath(x,y+1,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back();
    }
    if(y-1>=1&&GameMap::binMap[x][y-1]!=0)
    {
        path.push_back(UP);
        if(GameMap::binMap[x][y-1]!=3)
            findPath(x,y-1,endx,endy,tempsteps+1,totalsteps);
        else
            findPath(x,y-1,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back();
    }
}
void FindPathAlgorithm::init(int t_move, Character *t_nowCharacter)
{
    init();
    totalMove = t_move;
    nowCharacter = t_nowCharacter;
    foundPathOrNot = false;
}
