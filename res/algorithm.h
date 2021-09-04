#ifndef ALGORITHM_H
#define ALGORITHM_H


class FindPathAlgorithm
{
public:
    int resultMap[100][100];
    FindPathAlgorithm();//0为不可行，1为可行
    int totalMove;
public:
    void findAvailableCell(int x,int y,int depth);
    void setMove(int);
};

#endif // ALGORITHM_H
