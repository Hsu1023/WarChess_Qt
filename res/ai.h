#ifndef AI_H
#define AI_H

#include "character.h"
#include "algorithm.h"
#include "config.h"

class GameAI : public QObject
{
    Q_OBJECT
public:
    FindPathAlgorithm Al;
public:
    GameAI();
    void AIRound(Character *character[], int characterNum);
    void moveCharacter(int id, Character * character[], int CharacterNum);
signals:
    //void repaintScreen();
    void thisCharacterFinished();
};

#endif // AI_H
