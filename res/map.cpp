#include "map.h"

Map::Map(QWidget *parent) : QWidget(parent)
{
    m_map1.load(MAP_PATH);
}
void Map::paint(QPainter& painter)const
{

}
