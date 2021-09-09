#include "gamemap.h"

int GameMap::binMap[][31] = {};

GameMap::GameMap(int id)
{
    if(id == 1)
        m_pic.load(MAP1_PATH); //
    else
        m_pic.load(MAP2_PATH);

    height = CELL_SIZE * originx[id - 1];
    width = CELL_SIZE * originy[id - 1];
    maxCellx = originy[id - 1];
    maxCelly = originx[id - 1];
    for(int i = 0; i <= originx[id - 1]; i++)//30
    {
        for(int j = 0; j <= originy[id - 1]; j++)//50
        {
            binMap[j][i] = originMap[id - 1][i][j];
        }
    }
}
