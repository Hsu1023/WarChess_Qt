#include "ai.h"

GameAI::GameAI(QObject * parent):
    QObject(parent)
{
}
inline int ManhattanDist(node x, node y)
{
    return abs(x.first - y.first) + abs(x.second - y.second);
}
void GameAI::moveCharacter(int id, Character * character[], int characterNum)
{
    Character *nowCharacter = character[id];
    Al.init(nowCharacter->m_move, nowCharacter);

    int tempx = nowCharacter->m_cellx;
    int tempy = nowCharacter->m_celly;

    Al.findAvailableCell(tempx, tempy, 0,character, characterNum);

    int minDist = 0x3f3f3f3f;
    node minNode{-1,-1};
    for(ull i = 0; i < Al.v.size(); i++)
    {
        for(int j = 0; j < characterNum; j++)
            if(character[j]->m_belong == MINE&&character[j]->characterState!=Character::DEAD)
        {
            int tempDist = ManhattanDist(Al.v[i], std::make_pair(character[j]->m_cellx, character[j]->m_celly));
            if(minDist > tempDist)
            {
                minNode = Al.v[i];
                minDist = tempDist;
                qDebug()<<"you";
            }
        }
    }

    //Sleep(1000);
    int localCellx = nowCharacter->m_localCellx + minNode.first - nowCharacter->m_cellx;
    int localCelly = nowCharacter->m_localCelly + minNode.second - nowCharacter->m_celly;
    nowCharacter->movePos(minNode.first,minNode.second,localCellx,localCelly,Al.resultMap[minNode.first][minNode.second]);

    if(minDist > nowCharacter->m_attrackable)return;//无法攻击

    qDebug()<<minDist;

}

