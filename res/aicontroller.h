#ifndef AICONTROLLER_H
#define AICONTROLLER_H

#include <QObject>
#include "config.h"
#include "character.h"
#include "ai.h"


// 此类控制AI方的所有角色
// 待一个角色通过GameAI类完成行动后，再使下一个角色行动，直至AI回合完成
class AIController : public QTimer
{
    Q_OBJECT
public:
    Character ** character; // 场上的角色的头指针
    int characterNum; // 场上角色数
    int count; // 遍历0~characterNum-1的循环变量
    int aicount, usedAI; // AI方有多少角色及已使用角色数
    GameAI *gameAI; // GameAI对象
public:
    AIController(Character *t_character[], int t_characterNum, QWidget* parent = nullptr);
    void reset(int aicount);
    void start();
    void loop();
signals:
    void AIRoundFinished(); // AI回合开始信号
    void AIRoundBegin(); // AI回合结束信号
};

#endif // AICONTROLLER_H
