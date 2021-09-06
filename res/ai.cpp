#include "ai.h"

GameAI::GameAI()
{
}
void GameAI::moveCharacter(int id, Character * character[], int characterNum)
{
    Character *nowCharacter = character[id];
    Al.init(nowCharacter->m_move, nowCharacter);

    int tempx = nowCharacter->m_cellx;
    int tempy = nowCharacter->m_celly;

    Al.findAvailableCell(tempx, tempy, 0, character, characterNum);

    int minDist = 0x3f3f3f3f;
    node minNode{-1,-1};
    int attrackid = -1;
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
                attrackid = j;
                //qDebug()<<"you";
            }
        }
    }

    Al.findPath(nowCharacter->m_cellx, nowCharacter->m_celly, minNode.first, minNode.second, 0, Al.resultMap[minNode.first][minNode.second]);
    //for(int i=0;i<moveAl.path.size();i++){qDebug()<<moveAl.path[i];}

    nowCharacter->movePos(Al.resultMap[minNode.first][minNode.second],Al.path);

    //emit repaintScreen();

    connect(nowCharacter->mover, &MoveAnimation::animationFinished, [=](){
        if(minDist <= nowCharacter->m_attrackable)
            emit character[attrackid]->beAttracked(nowCharacter->m_attrack);
        emit thisCharacterFinished();
    });
}

