#ifndef AI_H
#define AI_H

#include "character.h"
#include "algorithm.h"
#include "config.h"

class GameAI: public QObject
{
    Q_OBJECT
public:
    FindPathAlgorithm Al;
public:
    GameAI(QObject *parent = nullptr);
    void AIRound(Character *character[], int characterNum);
    void moveCharacter(int id, Character * character[], int CharacterNum);
signals:
    void repaintScreen();
    void AIRoundFinished();
};

#endif // AI_H
