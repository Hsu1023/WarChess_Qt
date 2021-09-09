#include "gamemap.h"

int GameMap::binMap[][31] = {};

GameMap::GameMap(int id)
{
    // 载入图片
    if(id == 1)
        m_pic.load(MAP1_PATH); //
    else
        m_pic.load(MAP2_PATH);

    // 设置数据
    height = CELL_SIZE * originx[id - 1];
    width = CELL_SIZE * originy[id - 1];
    maxCellx = originy[id - 1];
    maxCelly = originx[id - 1];

    // 向binMap导入地图
    for(int i = 0; i <= originx[id - 1]; i++)
    {
        for(int j = 0; j <= originy[id - 1]; j++)
        {
            binMap[j][i] = originMap[id - 1][i][j];
        }
    }
}
