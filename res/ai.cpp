#include "ai.h"

GameAI::GameAI()
{
}
void GameAI::moveCharacter(int id, Character * t_character[], int characterNum)
{
    character = t_character;
    nowCharacter = character[id];
    Al.init(nowCharacter->m_move, nowCharacter);

    int tempx = nowCharacter->m_cellx;
    int tempy = nowCharacter->m_celly;

    Al.findAvailableCell(tempx, tempy, 0, character, characterNum);

    minDist = 0x3f3f3f3f;
    node minNode{-1,-1};
    attrackid = -1;


    for(ull i = 0; i < Al.v.size(); i++)
    {
        for(int j = 0; j < characterNum; j++)
            if(character[j]->m_belong == MINE&&character[j]->characterState!=Character::DEAD)
        {
            //if(character[1]->characterState==Character::DEAD)qDebug()<<"DEAD";else qDebug()<<"EXISTING";
            int tempDist = ManhattanDist(Al.v[i], std::make_pair(character[j]->m_cellx, character[j]->m_celly));
            if(minDist > tempDist)
            {
                minNode = Al.v[i];
                minDist = tempDist;
                attrackid = j;
            }
        }
    }
    Al.findPath(nowCharacter->m_cellx, nowCharacter->m_celly, minNode.first, minNode.second, 0, Al.resultMap[minNode.first][minNode.second]);

    connect(nowCharacter->mover, &MoveAnimation::animationFinished, this, &GameAI::attrackCharacter,Qt::UniqueConnection);

    nowCharacter->movePos(Al.resultMap[minNode.first][minNode.second],Al.path);
}
void GameAI::attrackCharacter()
{
    if(minDist <= nowCharacter->m_attrackable)
    {
        nowCharacter->attracker->startMove(nowCharacter, nowCharacter->m_localCellx, nowCharacter->m_localCelly,
                             character[attrackid]->m_localCellx, character[attrackid]->m_localCelly);
        connect(nowCharacter->attracker, &AttrackAnimation::animationFinished,this, &GameAI::waitFunc,Qt::UniqueConnection);
    }
    else
    {
        emit thisCharacterFinished();
    }
}
void GameAI::waitFunc()
{
    emit character[attrackid]->beAttracked(nowCharacter->m_attrack);
    emit thisCharacterFinished();qDebug()<<"emit";
}
