#include "aicontroller.h"

AIController::AIController(Character *t_character[], int t_characterNum, QWidget *parent):
    QTimer(parent), count(-1), characterNum(t_characterNum)
{
    character = t_character;
    gameAI = new GameAI;
}
void AIController::reset()
{
    count = -1;
    qDebug()<<"reset";
}
void AIController::start()
{
    //connect(gameAI, &GameAI::repaintScreen, [=](){emit repaintScreen();});
    connect(gameAI, &GameAI::thisCharacterFinished, [=](){loop();});
    loop();
}
void AIController::loop()
{
    count++;
    if(count == characterNum)
    {
        emit AIRoundFinished();
        return;
    }
    //qDebug()<<count;
    for(;count < characterNum; count++)
    {
        //qDebug()<<count;
        if(character[count]->m_belong == YOURS&&character[count]->characterState!=Character::DEAD)
        {
            gameAI->moveCharacter(count, character, characterNum);
            return;
        }
    }
    if(count == characterNum)
    {
        emit AIRoundFinished();
        return;
    }
}
