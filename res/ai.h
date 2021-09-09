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
    int attrackid, minDist; // 要攻击的对象和没移动前距离对方棋子所有距离的最小值
    Character * nowCharacter; // 当前要移动的角色
    Character ** character; // 场上的角色的头指针
    GameAI();
    void AIRound(Character *character[], int characterNum);
    void moveCharacter(int id, Character * t_character[], int CharacterNum);
    void attrackCharacter();
    void waitFunc();
signals:
    void thisCharacterFinished(); // AI移动一个棋子结束
};

#endif // AI_H
