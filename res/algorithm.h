#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "character.h"

class FindPathAlgorithm
{
public:
    int resultMap[100][100];
    FindPathAlgorithm();//0为不可行，1为可行
    int totalMove;
    Character * nowCharacter;
public:
    void findAvailableCell(int x,int y,int depth, Character*[], int characterNum);
    void setMove(int,Character *t_nowCharacter);
};

#endif // ALGORITHM_H
