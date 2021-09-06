#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "character.h"

class FindPathAlgorithm
{
public:
    int resultMap[100][100];
    FindPathAlgorithm();// 0为不可行，1为可行
    int totalMove;
    bool foundPathOrNot;
    Character * nowCharacter;
    std::vector<node> v;
    std::vector<int> path;// DIRECTION
public:
    void findAvailableCell(int x,int y,int depth, Character*[], int characterNum);
    void findPath(int x, int y, int endx, int endy, int tempsteps, int totalsteps);
    void init();
    void init(int, Character *t_nowCharacter);
};

#endif // ALGORITHM_H
