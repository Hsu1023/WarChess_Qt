#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "config.h"

class GameMap
{
public:
    GameMap(int id);
public:
    QPixmap m_pic;
    int height, width;
    int maxCellx, maxCelly;
    static int binMap[51][31];
};

#endif // GAMEMAP_H
