#include "minimap.h"


Minimap::Minimap(Character **t_character, int t_characterNum, int t_maxCellx, int t_maxCelly, QWidget *parent):
    QWidget(parent)
{
    character = t_character;
    characterNum = t_characterNum;
    maxCellx = t_maxCellx;
    maxCelly = t_maxCelly;
    setFixedSize(10 * maxCellx, 10 * maxCelly);
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
}
void Minimap::paintEvent(QPaintEvent *)
{
    enum COLOR{GREEN, BLUE, RED, WATER, SAND};
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    for(int i = 1; i <= maxCellx; i++)
    {

        for(int j = 1; j <=maxCelly; j++)
        {
            int color = GREEN;
            for(int k = 0; k < characterNum; k++)
            {
                if(character[k]->characterState == Character::DEAD)continue;
                if(character[k]->m_cellx == i && character[k]->m_celly == j)
                {
                    if(character[k]->m_belong == MINE)color = BLUE;
                    else color = RED;

                }
            }
            if(color == GREEN)
            {
                if(GameMap::binMap[i][j]==3)color = WATER;
                else if(GameMap::binMap[i][j]==2)color = SAND;
            }
            if(color == GREEN)painter.setBrush(QColor(120,255,120,100));//120 120 120
            else if(color == RED)painter.setBrush(QColor(255,0,100,250));
            else if (color == BLUE)painter.setBrush(QColor(0,0,255,250));
            else if (color == WATER)painter.setBrush(QColor(0,50,255,100));
            else painter.setBrush(QColor(255,255,0,100));
            painter.drawRect((i-1) * 10, (j-1) * 10, 10, 10);
        }
    }
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0,0,maxCellx*10, maxCelly * 10);
}
