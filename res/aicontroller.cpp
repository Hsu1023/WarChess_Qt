#include "aicontroller.h"

AIController::AIController(Character *t_character[], int t_characterNum, QWidget *parent):
    QTimer(parent), characterNum(t_characterNum), count(-1)
{
    character = t_character;
    gameAI = new GameAI;
}
// 开始AI回合前，需要调用以初始化参数
void AIController::reset(int t_aicount)
{
    count = -1;
    aicount = t_aicount;
    usedAI = 0;
}
// 进入AI循环
void AIController::start()
{
    emit AIRoundBegin();
    connect(gameAI, &GameAI::thisCharacterFinished, this, &AIController::loop, Qt::UniqueConnection);
    loop();
}
// 等待到GameAI操纵角色行动完成后，使下一个角色行动
void AIController::loop()
{
    count++;
    if(aicount == usedAI)
    {
        // 如果所有AI都用完，则发射AI回合结束信号
        emit AIRoundFinished();
        return;
    }
    for(;count < characterNum; count++)// 遍历所有角色
    {
        // 如果此角色属于AI方且没死，则用GameAI移动
        if(character[count]->m_belong == YOURS&&character[count]->characterState!=Character::DEAD)
        {
            usedAI++;
            gameAI->moveCharacter(count, character, characterNum);
            return;
        }
    }
}
