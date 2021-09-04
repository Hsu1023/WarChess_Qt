#include "algorithm.h"
#include "config.h"

FindPathAlgorithm::FindPathAlgorithm()
{
    memset(resultMap,-1,sizeof resultMap);
}
void FindPathAlgorithm::findAvailableCell(int x, int y, int depth)
{
    if(resultMap[x][y]>=0)
        resultMap[x][y] = min(depth, resultMap[x][y]);
    else resultMap[x][y]=depth;
    qDebug()<<resultMap[1][1];
    if(depth==totalMove)return;
    if(x-1>=1&&binMap[x-1][y]!=0)findAvailableCell(x-1,y,depth+1);
    if(x+1<=50&&binMap[x+1][y]!=0)findAvailableCell(x+1,y,depth+1);
    if(y+1<=30&&binMap[x][y+1]!=0)findAvailableCell(x,y+1,depth+1);
    if(y-1>=1&&binMap[x][y-1]!=0)findAvailableCell(x,y-1,depth+1);
}
void FindPathAlgorithm::setMove(int t_move)
{
    totalMove=t_move;
}
