#include "algorithm.h"
#include "config.h"
#include "gamemap.h"

FindPathAlgorithm::FindPathAlgorithm()
{
    init();
}
// 数据初始化
void FindPathAlgorithm::init()
{
    memset(resultMap,-1,sizeof resultMap);
    v.clear();
    foundPathOrNot = false;
    path.clear();
}
void FindPathAlgorithm::init(int t_move, Character *t_nowCharacter)
{
    init();
    totalMove = t_move;
    nowCharacter = t_nowCharacter;
    foundPathOrNot = false;
}
// dfs寻找可行域
void FindPathAlgorithm::findAvailableCell(int x, int y, int depth, Character* character[], int characterNum)
{
    for(int i = 0; i < characterNum; i++)
        if(character[i]!=nowCharacter&&character[i]->characterState!=Character::DEAD)
            if(character[i]->m_cellx==x&&character[i]->m_celly==y) //遇见有人挡路绕不过去
                return;

    if(resultMap[x][y] >= 0) //如果此点已经可行，则比较已知最短路和当前路哪个更短
    {
        resultMap[x][y] = min(depth, resultMap[x][y]);
    }
    else // 如果此点还没确定可行，则更新为可行
    {
        resultMap[x][y]=depth;
        v.push_back(std::make_pair(x,y));
    }

    if(depth>=totalMove)return; // 行动点数耗尽

    if(x - 1 >= 1 && GameMap::binMap[x - 1][y] != 0) // 向左走
    {
        if(GameMap::binMap[x - 1][y] == 3) // 如果是水，则行动力减少得更多
            findAvailableCell(x - 1, y, depth + 2, character, characterNum);
        else
            findAvailableCell(x - 1, y, depth + 1, character, characterNum);
    }
    if(x + 1 <= 50 && GameMap::binMap[x + 1][y] != 0) // 向右走
    {
        if(GameMap::binMap[x + 1][y] == 3) // 如果是水，则行动力减少得更多
            findAvailableCell(x + 1, y, depth + 2, character, characterNum);
        else
            findAvailableCell(x + 1, y, depth + 1, character, characterNum);
    }
    if(y + 1 <= 30 && GameMap::binMap[x][y + 1]!=0) // 向下走
    {

        if(GameMap::binMap[x][y + 1] == 3) // 如果是水，则行动力减少得更多
            findAvailableCell(x, y + 1, depth + 2, character, characterNum);
        else
            findAvailableCell(x, y + 1, depth + 1, character, characterNum);
    }
    if(y - 1 >= 1 && GameMap::binMap[x][y - 1] != 0) // 向上走
    {
        if(GameMap::binMap[x][y - 1] == 3) // 如果是水，则行动力减少得更多
            findAvailableCell(x, y - 1, depth + 2, character, characterNum);
        else
            findAvailableCell(x, y - 1, depth + 1, character, characterNum);
    }
}
// 找当前点到达可行域点的路径，以DIRECTION的enum值存入vector
void FindPathAlgorithm::findPath(int x, int y, int endx, int endy, int tempsteps, int totalsteps)
{
    if(foundPathOrNot)
        return; // 找到路了，剪枝
    if(tempsteps > resultMap[x][y])
        return ; // 如果当前路径消耗的行动点数大于已知最少行动点数，则当前路径不为所求
    if(x == endx && y == endy)
    {
        // 走到终点，且消耗的行动点数最少，则当前路就是要寻找的路
        foundPathOrNot = true;
        return;
    }
    if(x - 1 >= 1&&GameMap::binMap[x - 1][y]!=0) //向左走
    {
        path.push_back(LEFT); // 标记当前路径
        if(GameMap::binMap[x-1][y]!=3)
            findPath(x - 1 ,y,endx,endy,tempsteps + 1,totalsteps);
        else // 如果是水，则行动力减少得更多
            findPath(x - 1,y,endx,endy,tempsteps + 2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back(); // 解除标记
    }
    if(x+1<=50&&GameMap::binMap[x+1][y]!=0) // 向右走
    {
        path.push_back(RIGHT); // 标记当前路径
        if(GameMap::binMap[x+1][y]!=3)
            findPath(x+1,y,endx,endy,tempsteps+1,totalsteps);
        else // 如果是水，则行动力减少得更多
            findPath(x+1,y,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back(); // 解除标记
    }
    if(y+1<=30&&GameMap::binMap[x][y+1]!=0) // 向下走
    {
        path.push_back(DOWN); // 标记当前路径
        if(GameMap::binMap[x][y+1]!=3)
            findPath(x,y+1,endx,endy,tempsteps+1,totalsteps);
        else // 如果是水，则行动力减少得更多
            findPath(x,y+1,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back(); // 解除标记
    }
    if(y-1>=1&&GameMap::binMap[x][y-1]!=0) // 向上走
    {
        path.push_back(UP); // 标记当前路径
        if(GameMap::binMap[x][y-1]!=3)
            findPath(x,y-1,endx,endy,tempsteps+1,totalsteps);
        else // 如果是水，则行动力减少得更多
            findPath(x,y-1,endx,endy,tempsteps+2,totalsteps);
        if(foundPathOrNot)return;
        path.pop_back(); // 解除标记
    }
}
