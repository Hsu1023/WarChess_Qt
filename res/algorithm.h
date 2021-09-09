#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "character.h"
#include "gamemap.h"

class FindPathAlgorithm
{
public:
    int resultMap[51][31]; // dfs后移动/攻击的可行域，其中-1代表不可行，
                            //正数代表可行域距离当前位置的曼哈顿距离，此距离为所有路径的最小值
    FindPathAlgorithm(); // 0为不可行，1为可行
    int totalMove; // 可行域圈定的最大距离
    bool foundPathOrNot; // 是否找到可行的路
    Character * nowCharacter; // 场上角色数
    std::vector<node> v; // 储存所有可行格子，以(x,y)形式存储
    std::vector<int> path;// 走到 某一可行格子的路径，由"config.h"中枚举变量"DIRACTION"规定
public:
    void findAvailableCell(int x,int y,int depth, Character*[], int characterNum);
    void findPath(int x, int y, int endx, int endy, int tempsteps, int totalsteps);
    void init();
    void init(int, Character *t_nowCharacter);
};

#endif // ALGORITHM_H
