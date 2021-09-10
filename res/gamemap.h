#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "config.h"

class GameMap
{
public:
    explicit GameMap(const int id);
public:
    int m_height, m_width; // 地形size（像素）
    int m_maxCellx, m_maxCelly; // 地形size(格子)
    static int binMap[51][31]; // 四进制储存地形
    QPixmap m_pic; // 地形图片
};

#endif // GAMEMAP_H
