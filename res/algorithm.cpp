#include "algorithm.h"
#include "config.h"

FindPathAlgorithm::FindPathAlgorithm()
{
    memset(resultMap,-1,sizeof resultMap);
}
void FindPathAlgorithm::findAvailableCell(int x, int y, int depth, Character* character[], int characterNum)
{
    if(binMap[x][y]==0)return;
    qDebug()<<resultMap[11][15];
    for(int i=0;i<characterNum;i++)
        //遇见敌人绕不过去
        if(character[i]!=nowCharacter&&character[i]->characterState!=Character::DEAD)
        {
            if(character[i]->m_cellx==x&&character[i]->m_celly==y)return;
        }
    if(resultMap[x][y]>=0)
    {
        resultMap[x][y] = min(depth, resultMap[x][y]);
    }
    else resultMap[x][y]=depth;
    if(depth==totalMove)return;
    if(x-1>=1&&binMap[x-1][y]!=0)
        findAvailableCell(x-1,y,depth+1,character,characterNum);
    if(x+1<=50&&binMap[x+1][y]!=0)
        findAvailableCell(x+1,y,depth+1,character,characterNum);
    if(y+1<=30&&binMap[x][y+1]!=0)
        findAvailableCell(x,y+1,depth+1,character,characterNum);
    if(y-1>=1&&binMap[x][y-1]!=0)
        findAvailableCell(x,y-1,depth+1,character,characterNum);
}
void FindPathAlgorithm::setMove(int t_move, Character *t_nowCharacter)
{
    totalMove=t_move;
    nowCharacter=t_nowCharacter;
}
