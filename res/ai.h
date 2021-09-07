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
    int attrackid, minDist;
    Character * nowCharacter;
    Character ** character;
    GameAI();
    void AIRound(Character *character[], int characterNum);
    void moveCharacter(int id, Character * t_character[], int CharacterNum);
    void attrackCharacter();
    void waitFunc();
signals:
    //void repaintScreen();
    void thisCharacterFinished();
};

#endif // AI_H
