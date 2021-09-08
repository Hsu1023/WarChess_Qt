#include "aicontroller.h"

AIController::AIController(Character *t_character[], int t_characterNum, QWidget *parent):
    QTimer(parent), count(-1), characterNum(t_characterNum)
{
    character = t_character;
    gameAI = new GameAI;
}
void AIController::reset(int t_aicount)
{
    count = -1;
    aicount = t_aicount;
    usedAI = 0;
    qDebug()<<"aicount"<<aicount;
}
void AIController::start()
{
    emit AIRoundBegin();
    //connect(gameAI, &GameAI::repaintScreen, [=](){emit repaintScreen();});
    connect(gameAI, &GameAI::thisCharacterFinished, this, &AIController::loop, Qt::UniqueConnection);
    loop();
}
void AIController::loop()
{
    count++;
    if(aicount == usedAI)
    {
        emit AIRoundFinished();
        return;
    }
    for(;count < characterNum; count++)
    {
        if(character[count]->m_belong == YOURS&&character[count]->characterState!=Character::DEAD)
        {
            usedAI++;
            gameAI->moveCharacter(count, character, characterNum);
            return;
        }
    }
}
