#ifndef AICONTROLLER_H
#define AICONTROLLER_H

#include <QObject>
#include "config.h"
#include "character.h"
#include "ai.h"

class AIController : public QTimer
{
    Q_OBJECT
public:
    int count;
    int characterNum, aicount, usedAI;
    Character ** character;
    GameAI *gameAI;
public:
    AIController(Character *t_character[], int t_characterNum, QWidget* parent = nullptr);
    void reset(int aicount);
    void start();
    void loop();
signals:
    void AIRoundFinished();
    void AIRoundBegin();
    //void repaintScreen();
};

#endif // AICONTROLLER_H
